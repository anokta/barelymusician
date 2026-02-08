import Module from './barelymusician.js';
import {INSTRUMENT_CONTROLS, NoteControlType} from './control.js';
import {MessageType} from './message.js'

const INSTRUMENT_CONTROL_OVERRIDE_SIZE = 8;  // sizeof(BarelyInstrumentControlOverride)
const NOTE_CONTROL_OVERRIDE_SIZE = 8;        // sizeof(BarelyNoteControlOverride)
const SLICE_SIZE = 24;                       // sizeof(BarelySlice)

const RENDER_QUANTUM_SIZE = 128;
const STEREO_CHANNEL_COUNT = 2;

class Processor extends AudioWorkletProcessor {
  constructor() {
    super();

    this._module = null;
    this._engine = null;

    this._doublePtr = null;
    this._uint8Ptr = null;
    this._uint32Ptr = null;
    this._instrumentControlOverridesPtr = null;
    this._noteControlOverridesPtr = null;
    this._outputSamplesPtr = null;

    this._instruments = new Map();
    this._performers = new Set();
    this._tasks = new Map();

    this._slices = {};
    this._slicesToRemove = [];

    this._timestamp = 0;

    Module().then(module => {
      this._module = module;

      // No need to call `_free` as these will be freed as part of the module teardown.
      this._doublePtr = this._module._malloc(Float64Array.BYTES_PER_ELEMENT);
      this._uint8Ptr = this._module._malloc(Uint8Array.BYTES_PER_ELEMENT);
      this._uint32Ptr = this._module._malloc(Uint32Array.BYTES_PER_ELEMENT);
      this._instrumentControlOverridesPtr = this._module._malloc(
          Object.keys(INSTRUMENT_CONTROLS).length * INSTRUMENT_CONTROL_OVERRIDE_SIZE);
      this._noteControlOverridesPtr =
          this._module._malloc(NoteControlType.COUNT * NOTE_CONTROL_OVERRIDE_SIZE);
      this._outputSamplesPtr = this._module._malloc(
          STEREO_CHANNEL_COUNT * RENDER_QUANTUM_SIZE * Float32Array.BYTES_PER_ELEMENT);

      this._module._BarelyEngine_Create(sampleRate, this._uint32Ptr);
      this._engine = this._module.getValue(this._uint32Ptr, 'i32');

      this.port.postMessage({type: MessageType.INIT_SUCCESS});
    });

    this.port.onmessage = event => {
      if (!event.data) return;

      if (!this._module || !this._engine) {
        console.error('barelymusician not initialized!');
        return;
      }

      switch (event.data.type) {
        case MessageType.ENGINE_SET_CONTROL:
          this._module._BarelyEngine_SetControl(
              this._engine, event.data.typeIndex, event.data.value);
          break;
        case MessageType.ENGINE_SET_TEMPO:
          this._module._BarelyEngine_SetTempo(this._engine, event.data.tempo);
          break;
        case MessageType.ENGINE_UPDATE: {
          const deltaFrameTime = 1.0 / 60.0;
          const latency = Math.max(deltaFrameTime, RENDER_QUANTUM_SIZE / sampleRate);
          this._timestamp = currentTime + latency;
          this._module._BarelyEngine_Update(this._engine, this._timestamp);
          for (const performerId of this._performers) {
            this._module._BarelyPerformer_GetPosition(this._engine, performerId, this._doublePtr);
            const position = this._module.getValue(this._doublePtr, 'double');
            this.port.postMessage({
              type: MessageType.PERFORMER_GET_PROPERTIES_SUCCESS,
              id: performerId,
              position,
            });
          }
          for (const taskId of this._tasks.keys()) {
            this._module._BarelyTask_IsActive(this._engine, taskId, this._uint8Ptr);
            const isActive = (this._module.getValue(this._uint8Ptr) !== 0);
            this.port.postMessage({
              type: MessageType.TASK_GET_PROPERTIES_SUCCESS,
              id: taskId,
              isActive,
            });
          }
        } break;
        case MessageType.INSTRUMENT_CREATE: {
          let i = 0;
          for (const controlTypeIndex in INSTRUMENT_CONTROLS) {
            const controlOffset =
                (this._instrumentControlOverridesPtr + i * INSTRUMENT_CONTROL_OVERRIDE_SIZE) /
                Uint32Array.BYTES_PER_ELEMENT;
            this._module.HEAP32[controlOffset] = controlTypeIndex;
            this._module.HEAPF32[controlOffset + 1] =
                INSTRUMENT_CONTROLS[controlTypeIndex].defaultValue;
            ++i;
          }
          this._module._BarelyEngine_CreateInstrument(
              this._engine, this._instrumentControlOverridesPtr,
              Object.keys(INSTRUMENT_CONTROLS).length, this._uint32Ptr);
          const instrumentId = this._module.getValue(this._uint32Ptr, 'i32');

          const NoteEventType = {
            BEGIN: 0,
            END: 1,
            COUNT: 2,
          };
          const noteEventCallback = (eventType, pitch) => {
            if (eventType === NoteEventType.BEGIN) {
              this.port.postMessage(
                  {type: MessageType.INSTRUMENT_ON_NOTE_ON, id: instrumentId, pitch});
            } else if (eventType === NoteEventType.END) {
              this.port.postMessage(
                  {type: MessageType.INSTRUMENT_ON_NOTE_OFF, id: instrumentId, pitch});
            }
          };
          const noteEventCallbackPtr = this._module.addFunction((eventType, pitch, userData) => {
            const callback = this._instruments.get(userData)?.eventCallback;
            if (callback) {
              callback(eventType, pitch);
            }
          }, 'vifi');
          this._module._BarelyInstrument_SetNoteEventCallback(
              this._engine, instrumentId, noteEventCallbackPtr, instrumentId);
          this._instruments.set(instrumentId, {noteEventCallback, noteEventCallbackPtr});

          this.port.postMessage({
            type: MessageType.INSTRUMENT_CREATE_SUCCESS,
            id: instrumentId,
            requestId: event.data.requestId,
          });
        } break;
        case MessageType.INSTRUMENT_DESTROY:
          this._module._BarelyEngine_DestroyInstrument(this._engine, event.data.id);
          if (this._slices[event.data.id]) {
            this._slicesToRemove.push({
              ...this._slices[event.data.id],
              timestamp: this._timestamp,
            });
            delete this._slices[event.data.id];
          }
          this.port.postMessage({type: MessageType.INSTRUMENT_DESTROY_SUCCESS, id: event.data.id});
          this._module.removeFunction(this._instruments.get(event.data.id).noteEventCallbackPtr);
          this._instruments.delete(event.data.id);
          break;
        case MessageType.INSTRUMENT_SET_ALL_NOTES_OFF:
          this._module._BarelyInstrument_SetAllNotesOff(this._engine, event.data.id);
          break;
        case MessageType.INSTRUMENT_SET_CONTROL:
          this._module._BarelyInstrument_SetControl(
              this._engine, event.data.id, event.data.typeIndex, event.data.value);
          break;
        case MessageType.INSTRUMENT_SET_NOTE_CONTROL:
          this._module._BarelyInstrument_SetNoteControl(
              this._engine, event.data.id, event.data.pitch, event.data.typeIndex,
              event.data.value);
          break;
        case MessageType.INSTRUMENT_SET_NOTE_OFF:
          this._module._BarelyInstrument_SetNoteOff(this._engine, event.data.id, event.data.pitch);
          break;
        case MessageType.INSTRUMENT_SET_NOTE_ON: {
          const gainOffset = this._noteControlOverridesPtr / Uint32Array.BYTES_PER_ELEMENT;
          this._module.HEAP32[gainOffset] = NoteControlType.GAIN;
          this._module.HEAPF32[gainOffset + 1] = event.data.gain;

          const pitchShiftOffset = (this._noteControlOverridesPtr + NOTE_CONTROL_OVERRIDE_SIZE) /
              Uint32Array.BYTES_PER_ELEMENT;
          this._module.HEAP32[pitchShiftOffset] = NoteControlType.PITCH_SHIFT;
          this._module.HEAPF32[pitchShiftOffset + 1] = event.data.pitchShift;

          this._module._BarelyInstrument_SetNoteOn(
              this._engine, event.data.id, event.data.pitch, this._noteControlOverridesPtr,
              NoteControlType.COUNT);
        } break;
        case MessageType.INSTRUMENT_SET_SAMPLE_DATA:
          this._setInstrumentSampleData(event.data.id, event.data.slices);
          break;
        case MessageType.PERFORMER_CREATE: {
          this._module._BarelyEngine_CreatePerformer(this._engine, this._uint32Ptr);
          const performerId = this._module.getValue(this._uint32Ptr, 'i32');
          this._performers.add(performerId);
          this.port.postMessage({
            type: MessageType.PERFORMER_CREATE_SUCCESS,
            id: performerId,
            requestId: event.data.requestId,
          });
        } break;
        case MessageType.PERFORMER_DESTROY:
          this._module._BarelyEngine_DestroyPerformer(this._engine, event.data.id);
          this.port.postMessage({type: MessageType.PERFORMER_DESTROY_SUCCESS, id: event.data.id});
          this._performers.delete(event.data.id);
          break;
        case MessageType.PERFORMER_SET_LOOP_BEGIN_POSITION:
          this._module._BarelyPerformer_SetLoopBeginPosition(
              this._engine, event.data.id, event.data.loopBeginPosition);
          break;
        case MessageType.PERFORMER_SET_LOOP_LENGTH:
          this._module._BarelyPerformer_SetLoopLength(
              this._engine, event.data.id, event.data.loopLength);
          break;
        case MessageType.PERFORMER_SET_LOOPING:
          this._module._BarelyPerformer_SetLooping(
              this._engine, event.data.id, event.data.isLooping);
          break;
        case MessageType.PERFORMER_SET_POSITION:
          this._module._BarelyPerformer_SetPosition(
              this._engine, event.data.id, event.data.position);
          break;
        case MessageType.PERFORMER_START:
          this._module._BarelyPerformer_Start(this._engine, event.data.id);
          break;
        case MessageType.PERFORMER_STOP:
          this._module._BarelyPerformer_Stop(this._engine, event.data.id);
          break;
        case MessageType.TASK_CREATE: {
          this._module._BarelyEngine_CreateTask(
              this._engine, event.data.performerId, event.data.position, event.data.duration,
              event.data.priority, null, null, this._uint32Ptr);
          const taskId = this._module.getValue(this._uint32Ptr, 'i32');

          const eventCallback = (eventType) =>
              this.port.postMessage({type: MessageType.TASK_ON_EVENT, id: taskId, eventType});
          const eventCallbackPtr = this._module.addFunction((eventType, userData) => {
            const callback = this._tasks.get(userData)?.eventCallback;
            if (callback) {
              callback(eventType);
            }
          }, 'vii');
          this._module._BarelyTask_SetEventCallback(this._engine, taskId, eventCallbackPtr, taskId);
          this._tasks.set(taskId, {eventCallback, eventCallbackPtr});

          this.port.postMessage({
            type: MessageType.TASK_CREATE_SUCCESS,
            id: taskId,
            requestId: event.data.requestId,
          });
        } break;
        case MessageType.TASK_DESTROY:
          this._module._BarelyEngine_DestroyTask(this._engine, event.data.id);
          this.port.postMessage({type: MessageType.TASK_DESTROY_SUCCESS, id: event.data.id});
          this._module.removeFunction(this._tasks.get(event.data.id).eventCallbackPtr);
          this._tasks.delete(event.data.id);
          break;
        case MessageType.TASK_SET_DURATION:
          this._module._BarelyTask_SetDuration(this._engine, event.data.id, event.data.duration);
          break;
        case MessageType.TASK_SET_POSITION:
          this._module._BarelyTask_SetPosition(this._engine, event.data.id, event.data.position);
          break;
        case MessageType.TASK_SET_PRIORITY:
          this._module._BarelyTask_SetPriority(this._engine, event.data.id, event.data.priority);
          break;
        default:
          console.error(`Invalid message: ${event.data.type}`);
      }
    };
  }

  /**
   * Audio processing callback.
   */
  process(inputs, outputs, parameters) {
    if (!this._engine || !this._module || !this._module.HEAPF32) return true;

    const output = outputs[0];
    const outputChannelCount = Math.min(output.length, STEREO_CHANNEL_COUNT);
    const outputFrameCount = output[0].length;
    const outputSampleCount = outputChannelCount * outputFrameCount;

    if (outputSampleCount == 0) return true;
    if (outputFrameCount > RENDER_QUANTUM_SIZE) return true;

    const outputSamples =
        new Float32Array(this._module.HEAPF32.buffer, this._outputSamplesPtr, outputSampleCount);

    this._module._BarelyEngine_Process(
        this._engine, this._outputSamplesPtr, outputChannelCount, outputFrameCount, currentTime);
    this._cleanUpInstrumentSampleData(currentTime);

    for (let frame = 0; frame < outputFrameCount; ++frame) {
      for (let channel = 0; channel < outputChannelCount; ++channel) {
        output[channel][frame] = outputSamples[frame * outputChannelCount + channel];
      }
    }

    return true;
  }

  /**
   * Sets instrument sample data.
   * @param {number} id
   * @param {!Array<{rootPitch: number, sampleRate: string, samples: Array<float>}>} slices
   * @private
   */
  _setInstrumentSampleData(id, slices) {
    if (this._slices[id]) {
      this._slicesToRemove.push({
        ...this._slices[id],
        timestamp: this._timestamp,
      });
    }

    const sliceCount = slices.length;
    const slicesPtr = this._module._malloc(sliceCount * SLICE_SIZE);
    const samplePtrs = [];

    for (let i = 0; i < sliceCount; ++i) {
      const samples = slices[i].samples;
      const sampleCount = samples.length;

      const samplesPtr = this._module._malloc(sampleCount * Float32Array.BYTES_PER_ELEMENT);
      this._module.HEAPF32.set(samples, samplesPtr / Float32Array.BYTES_PER_ELEMENT);
      samplePtrs.push(samplesPtr);

      const offset = (slicesPtr + i * SLICE_SIZE) / Uint32Array.BYTES_PER_ELEMENT;
      this._module.HEAP32[offset] = samplesPtr;
      this._module.HEAP32[offset + 1] = sampleCount;
      this._module.HEAP32[offset + 2] = slices[i].sampleRate;
      this._module.HEAPF32[offset + 3] = slices[i].rootPitch;
    }

    this._module._BarelyInstrument_SetSampleData(this._engine, id, slicesPtr, sliceCount);

    this._slices[id] = {
      slicesPtr: slicesPtr,
      samplePtrs: samplePtrs,
    };
  }

  /**
   * Cleans up instrument sample data.
   * @param {number} timestamp
   * @private
   */
  _cleanUpInstrumentSampleData(timestamp) {
    let keepIndex = 0;
    for (let i = 0; i < this._slicesToRemove.length; ++i) {
      const slice = this._slicesToRemove[i];
      if (timestamp > slice.timestamp) {
        for (const ptr of slice.samplePtrs) {
          this._module._free(ptr);
        }
        this._module._free(slice.slicesPtr);
        keepIndex = i + 1;
      } else {
        break;  // timestamps are guaranteed to be sorted.
      }
    }
    if (keepIndex > 0) {
      this._slicesToRemove = this._slicesToRemove.slice(keepIndex);
    }
  }
}

registerProcessor('barelymusician-processor', Processor);
