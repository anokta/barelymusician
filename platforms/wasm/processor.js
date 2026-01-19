import Module from './barelymusician.js';
import {CONTROLS, InstrumentControlType, NoteControlType} from './control.js';

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

    this._instrumentCallbacks = {};
    this._taskCallbacks = {};

    this._slices = {};
    this._slicesToRemove = [];

    this._timestamp = 0;

    Module().then(module => {
      this._module = module;

      // No need to call `_free` as these will be freed as part of the module teardown.
      this._doublePtr = this._module._malloc(Float64Array.BYTES_PER_ELEMENT);
      this._uint8Ptr = this._module._malloc(Uint8Array.BYTES_PER_ELEMENT);
      this._uint32Ptr = this._module._malloc(Uint32Array.BYTES_PER_ELEMENT);
      this._instrumentControlOverridesPtr =
          this._module._malloc(Object.keys(CONTROLS).length * INSTRUMENT_CONTROL_OVERRIDE_SIZE);
      this._noteControlOverridesPtr =
          this._module._malloc(NoteControlType.COUNT * NOTE_CONTROL_OVERRIDE_SIZE);
      this._outputSamplesPtr = this._module._malloc(
          STEREO_CHANNEL_COUNT * RENDER_QUANTUM_SIZE * Float32Array.BYTES_PER_ELEMENT);

      this._module._BarelyEngine_Create(sampleRate, this._uint32Ptr);
      this._engine = this._module.getValue(this._uint32Ptr, 'i32');

      this.port.postMessage({type: 'init-success'});
    });

    this.port.onmessage = event => {
      if (!event.data) return;

      if (!this._module || !this._engine) {
        console.error('barelymusician not initialized!');
        return;
      }

      switch (event.data.type) {
        case 'engine-set-control':
          this._module._BarelyEngine_SetControl(
              this._engine, event.data.typeIndex, event.data.value);
          break;
        case 'engine-set-tempo':
          this._module._BarelyEngine_SetTempo(this._engine, event.data.tempo);
          break;
        case 'engine-update': {
          const deltaFrameTime = 1.0 / 60.0;
          const latency = Math.max(deltaFrameTime, RENDER_QUANTUM_SIZE / sampleRate);
          this._timestamp = currentTime + latency;
          this._module._BarelyEngine_Update(this._engine, this._timestamp);
        } break;
        case 'instrument-create': {
          let i = 0;
          for (const controlTypeIndex in CONTROLS) {
            const controlOffset =
                (this._instrumentControlOverridesPtr + i * INSTRUMENT_CONTROL_OVERRIDE_SIZE) /
                Uint32Array.BYTES_PER_ELEMENT;
            this._module.HEAP32[controlOffset] = controlTypeIndex;
            this._module.HEAPF32[controlOffset + 1] = CONTROLS[controlTypeIndex].defaultValue;
            ++i;
          }
          this._module._BarelyEngine_CreateInstrument(
              this._engine, this._instrumentControlOverridesPtr, Object.keys(CONTROLS).length,
              this._uint32Ptr);
          const instrumentId = this._module.getValue(this._uint32Ptr, 'i32');

          const NoteEventType = {
            BEGIN: 0,
            END: 1,
            COUNT: 2,
          };
          const noteEventCallback = (eventType, pitch) => {
            if (eventType === NoteEventType.BEGIN) {
              this.port.postMessage({type: 'instrument-on-note-on', id: instrumentId, pitch});
            } else if (eventType === NoteEventType.END) {
              this.port.postMessage({type: 'instrument-on-note-off', id: instrumentId, pitch});
            }
          };
          this._instrumentCallbacks[instrumentId] = noteEventCallback;
          const noteEventCallbackPtr = this._module.addFunction((eventType, pitch, userData) => {
            const callback = this._instrumentCallbacks[userData];
            if (callback) {
              callback(eventType, pitch);
            }
          }, 'vifi');
          this._module._BarelyInstrument_SetNoteEventCallback(
              this._engine, instrumentId, noteEventCallbackPtr, instrumentId);

          this.port.postMessage({type: 'instrument-create-success', id: instrumentId});
        } break;
        case 'instrument-destroy':
          this._module._BarelyEngine_DestroyInstrument(this._engine, event.data.id);
          if (this._slices[event.data.id]) {
            this._slicesToRemove.push({
              ...this._slices[event.data.id],
              timestamp: this._timestamp,
            });
            delete this._slices[event.data.id];
          }
          this.port.postMessage({type: 'instrument-destroy-success', id: event.data.id});
          break;
        case 'instrument-set-all-notes-off':
          this._module._BarelyInstrument_SetAllNotesOff(this._engine, event.data.id);
          break;
        case 'instrument-set-control':
          this._module._BarelyInstrument_SetControl(
              this._engine, event.data.id, event.data.typeIndex, event.data.value);
          break;
        case 'instrument-set-note-control':
          this._module._BarelyInstrument_SetNoteControl(
              this._engine, event.data.id, event.data.pitch, event.data.typeIndex,
              event.data.value);
          break;
        case 'instrument-set-note-off':
          this._module._BarelyInstrument_SetNoteOff(this._engine, event.data.id, event.data.pitch);
          break;
        case 'instrument-set-note-on': {
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
        case 'instrument-set-sample-data':
          this._setInstrumentSampleData(event.data.id, event.data.slices);
          break;
        case 'performer-create': {
          this._module._BarelyEngine_CreatePerformer(this._engine, this._uint32Ptr);
          const performerId = this._module.getValue(this._uint32Ptr, 'i32');
          this.port.postMessage({type: 'performer-create-success', id: performerId});
        } break;
        case 'performer-destroy':
          this._module._BarelyEngine_DestroyPerformer(this._engine, event.data.id);
          this.port.postMessage({type: 'performer-destroy-success', id: event.data.id});
          break;
        case 'performer-get-properties': {
          this._module._BarelyPerformer_IsPlaying(this._engine, event.data.id, this._uint8Ptr);
          const isPlaying = (this._module.getValue(this._uint8Ptr) !== 0);
          this._module._BarelyPerformer_GetPosition(this._engine, event.data.id, this._doublePtr);
          const position = this._module.getValue(this._doublePtr, 'double');
          this.port.postMessage({
            type: 'performer-get-properties-response',
            id: event.data.id,
            isPlaying,
            position,
          });
        } break;
        case 'performer-set-loop-begin-position':
          this._module._BarelyPerformer_SetLoopBeginPosition(
              this._engine, event.data.id, event.data.loopBeginPosition);
          break;
        case 'performer-set-loop-length':
          this._module._BarelyPerformer_SetLoopLength(
              this._engine, event.data.id, event.data.loopLength);
          break;
        case 'performer-set-looping':
          this._module._BarelyPerformer_SetLooping(
              this._engine, event.data.id, event.data.isLooping);
          break;
        case 'performer-set-position':
          this._module._BarelyPerformer_SetPosition(
              this._engine, event.data.id, event.data.position);
          break;
        case 'performer-start':
          this._module._BarelyPerformer_Start(this._engine, event.data.id);
          break;
        case 'performer-stop':
          this._module._BarelyPerformer_Stop(this._engine, event.data.id);
          break;
        case 'task-create': {
          this._module._BarelyEngine_CreateTask(
              this._engine, event.data.performerId, event.data.position, event.data.duration, 0,
              null, null, this._uint32Ptr);
          const taskId = this._module.getValue(this._uint32Ptr, 'i32');

          const eventCallback = (eventType) =>
              this.port.postMessage({type: 'task-on-event', id: taskId, eventType});
          this._taskCallbacks[taskId] = eventCallback;
          const eventCallbackPtr = this._module.addFunction((eventType, userData) => {
            const callback = this._taskCallbacks[userData];
            if (callback) {
              callback(eventType);
            }
          }, 'vii');
          this._module._BarelyTask_SetEventCallback(this._engine, taskId, eventCallbackPtr, taskId);

          this.port.postMessage(
              {type: 'task-create-success', performerId: event.data.performerId, id: taskId});
        } break;
        case 'task-destroy':
          this._module._BarelyEngine_DestroyTask(this._engine, event.data.id);
          this.port.postMessage({type: 'task-destroy-success', id: event.data.id});
          break;
        case 'task-get-properties': {
          this._module._BarelyTask_IsActive(this._engine, event.data.id, this._uint8Ptr);
          const isActive = (this._module.getValue(this._uint8Ptr) !== 0);
          this.port.postMessage({
            type: 'task-get-properties-response',
            id: event.data.id,
            isActive,
          });
        } break;
        case 'task-set-duration':
          this._module._BarelyTask_SetDuration(this._engine, event.data.id, event.data.duration);
          break;
        case 'task-set-position':
          this._module._BarelyTask_SetPosition(this._engine, event.data.id, event.data.position);
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
