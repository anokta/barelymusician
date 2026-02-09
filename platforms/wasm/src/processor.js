import Module from './barelymusician.js';
import {CommandType, Context, MessageType} from './context.js'
import {INSTRUMENT_CONTROLS, NoteControlType} from './control.js';

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
    this._context = null;

    this._doublePtr = null;
    this._uint8Ptr = null;
    this._uint32Ptr = null;
    this._instrumentControlOverridesPtr = null;
    this._noteControlOverridesPtr = null;
    this._outputSamplesPtr = null;

    this._instruments = new Map();
    this._performers = new Map();
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
      if (this._module && this._engine) {
        this._context = new Context(this._module, this._engine);
      }

      this.port.postMessage({type: MessageType.INIT_SUCCESS});
    });

    this.port.onmessage = event => {
      if (!event.data) return;
      if (!this._context) {
        console.error('barelymusician not initialized!');
        return;
      }
      if (event.data.type !== MessageType.UPDATE) {
        console.error(`Invalid message: ${event.data.type}`);
        return;
      }

      if (event.data.commands) {
        for (const command of event.data.commands) {
          this._processCommand(command);
        }
      }
      const deltaFrameTime = 1.0 / 60.0;
      const latency = Math.max(deltaFrameTime, RENDER_QUANTUM_SIZE / sampleRate);
      this._timestamp = currentTime + latency;
      this._module._BarelyEngine_Update(this._engine, this._timestamp);
      for (const [id, value] of this._performers) {
        this._module._BarelyPerformer_GetPosition(this._engine, value.performerId, this._doublePtr);
        const position = this._module.getValue(this._doublePtr, 'double');
        this._context._pendingCommands.push(
            {type: CommandType.PERFORMER_GET_PROPERTIES_SUCCESS, id, position});
      }
      for (const [id, value] of this._tasks) {
        this._module._BarelyTask_IsActive(this._engine, value.taskId, this._uint8Ptr);
        const isActive = (this._module.getValue(this._uint8Ptr) !== 0);
        this._context._pendingCommands.push(
            {type: CommandType.TASK_GET_PROPERTIES_SUCCESS, id, isActive});
      }

      if (this._context._pendingCommands.length > 0) {
        this.port.postMessage(
            {type: MessageType.UPDATE_SUCCESS, commands: this._context._pendingCommands});
        this._context._pendingCommands = [];
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
   * Processes a command.
   * @param {!Object} command
   * @private
   */
  _processCommand(command) {
    switch (command.type) {
      case CommandType.ENGINE_SET_CONTROL:
        this._context.engineSetControl(command.typeIndex, command.value);
        break;
      case CommandType.ENGINE_SET_TEMPO:
        this._context.engineSetTempo(command.tempo);
        break;
      case CommandType.INSTRUMENT_CREATE: {
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
            this._context._pendingCommands.push(
                {type: CommandType.INSTRUMENT_ON_NOTE_ON, id: command.id, pitch});
          } else if (eventType === NoteEventType.END) {
            this._context._pendingCommands.push(
                {type: CommandType.INSTRUMENT_ON_NOTE_OFF, id: command.id, pitch});
          }
        };
        const noteEventCallbackPtr = this._module.addFunction((eventType, pitch, userData) => {
          const callback = this._instruments.get(userData)?.noteEventCallback;
          if (callback) {
            callback(eventType, pitch);
          }
        }, 'vifi');
        this._module._BarelyInstrument_SetNoteEventCallback(
            this._engine, instrumentId, noteEventCallbackPtr, command.id);
        this._instruments.set(command.id, {instrumentId, noteEventCallback, noteEventCallbackPtr});
      } break;
      case CommandType.INSTRUMENT_DESTROY: {
        const instrumentId = this._instruments.get(command.id)?.instrumentId ?? 0;
        this._module._BarelyEngine_DestroyInstrument(this._engine, instrumentId);
        if (this._slices[instrumentId]) {
          this._slicesToRemove.push({
            ...this._slices[instrumentId],
            timestamp: this._timestamp,
          });
          delete this._slices[instrumentId];
        }
        this._module.removeFunction(this._instruments.get(command.id).noteEventCallbackPtr);
        this._instruments.delete(command.id);
      } break;
      case CommandType.INSTRUMENT_SET_ALL_NOTES_OFF:
        this._module._BarelyInstrument_SetAllNotesOff(
            this._engine, this._instruments.get(command.id)?.instrumentId ?? 0);
        break;
      case CommandType.INSTRUMENT_SET_CONTROL:
        this._module._BarelyInstrument_SetControl(
            this._engine, this._instruments.get(command.id)?.instrumentId ?? 0, command.typeIndex,
            command.value);
        break;
      case CommandType.INSTRUMENT_SET_NOTE_CONTROL:
        this._module._BarelyInstrument_SetNoteControl(
            this._engine, this._instruments.get(command.id)?.instrumentId ?? 0, command.pitch,
            command.typeIndex, command.value);
        break;
      case CommandType.INSTRUMENT_SET_NOTE_OFF:
        this._module._BarelyInstrument_SetNoteOff(
            this._engine, this._instruments.get(command.id)?.instrumentId ?? 0, command.pitch);
        break;
      case CommandType.INSTRUMENT_SET_NOTE_ON: {
        const gainOffset = this._noteControlOverridesPtr / Uint32Array.BYTES_PER_ELEMENT;
        this._module.HEAP32[gainOffset] = NoteControlType.GAIN;
        this._module.HEAPF32[gainOffset + 1] = command.gain;

        const pitchShiftOffset = (this._noteControlOverridesPtr + NOTE_CONTROL_OVERRIDE_SIZE) /
            Uint32Array.BYTES_PER_ELEMENT;
        this._module.HEAP32[pitchShiftOffset] = NoteControlType.PITCH_SHIFT;
        this._module.HEAPF32[pitchShiftOffset + 1] = command.pitchShift;

        this._module._BarelyInstrument_SetNoteOn(
            this._engine, this._instruments.get(command.id)?.instrumentId ?? 0, command.pitch,
            this._noteControlOverridesPtr, NoteControlType.COUNT);
      } break;
      case CommandType.INSTRUMENT_SET_SAMPLE_DATA:
        this._setInstrumentSampleData(
            this._instruments.get(command.id)?.instrumentId ?? 0, command.slices);
        break;
      case CommandType.PERFORMER_CREATE: {
        this._module._BarelyEngine_CreatePerformer(this._engine, this._uint32Ptr);
        const performerId = this._module.getValue(this._uint32Ptr, 'i32');
        this._performers.set(command.id, {performerId});
      } break;
      case CommandType.PERFORMER_DESTROY:
        this._module._BarelyEngine_DestroyPerformer(
            this._engine, this._performers.get(command.id)?.performerId ?? 0);
        this._performers.delete(command.id);
        break;
      case CommandType.PERFORMER_SET_LOOP_BEGIN_POSITION:
        this._module._BarelyPerformer_SetLoopBeginPosition(
            this._engine, this._performers.get(command.id)?.performerId ?? 0,
            command.loopBeginPosition);
        break;
      case CommandType.PERFORMER_SET_LOOP_LENGTH:
        this._module._BarelyPerformer_SetLoopLength(
            this._engine, this._performers.get(command.id)?.performerId ?? 0, command.loopLength);
        break;
      case CommandType.PERFORMER_SET_LOOPING:
        this._module._BarelyPerformer_SetLooping(
            this._engine, this._performers.get(command.id)?.performerId ?? 0, command.isLooping);
        break;
      case CommandType.PERFORMER_SET_POSITION:
        this._context.performerSetPosition(
            this._performers.get(command.id)?.performerId ?? 0, command.position);
        break;
      case CommandType.PERFORMER_START:
        this._context.performerStart(this._performers.get(command.id)?.performerId ?? 0);
        break;
      case CommandType.PERFORMER_STOP:
        this._context.performerStop(this._performers.get(command.id)?.performerId ?? 0);
        break;
      case CommandType.TASK_CREATE: {
        this._module._BarelyEngine_CreateTask(
            this._engine, this._performers.get(command.performerId)?.performerId ?? 0,
            command.position, command.duration, command.priority, null, null, this._uint32Ptr);
        const taskId = this._module.getValue(this._uint32Ptr, 'i32');
        const eventCallback = (eventType) => this._context._pendingCommands.push(
            {type: CommandType.TASK_ON_EVENT, id: command.id, eventType});
        const eventCallbackPtr = this._module.addFunction((eventType, userData) => {
          const callback = this._tasks.get(userData)?.eventCallback;
          if (callback) {
            callback(eventType);
          }
        }, 'vii');
        this._module._BarelyTask_SetEventCallback(
            this._engine, taskId, eventCallbackPtr, command.id);
        this._tasks.set(command.id, {taskId, eventCallback, eventCallbackPtr});
      } break;
      case CommandType.TASK_DESTROY: {
        this._module._BarelyEngine_DestroyTask(
            this._engine, this._tasks.get(command.id)?.taskId ?? 0);
        this._module.removeFunction(this._tasks.get(command.id).eventCallbackPtr);
        this._tasks.delete(command.id);
      } break;
      case CommandType.TASK_SET_DURATION:
        this._module._BarelyTask_SetDuration(
            this._engine, this._tasks.get(command.id)?.taskId ?? 0, command.duration);
        break;
      case CommandType.TASK_SET_POSITION:
        this._module._BarelyTask_SetPosition(
            this._engine, this._tasks.get(command.id)?.taskId ?? 0, command.position);
        break;
      case CommandType.TASK_SET_PRIORITY:
        this._module._BarelyTask_SetPriority(
            this._engine, this._tasks.get(command.id)?.taskId ?? 0, command.priority);
        break;
      default:
        console.error(`Invalid command: ${command.type}`);
    }
  }

  /**
   * Sets instrument sample data.
   * @param {number} instrumentId
   * @param {!Array<{rootPitch: number, sampleRate: string, samples: Array<float>}>} slices
   * @private
   */
  _setInstrumentSampleData(instrumentId, slices) {
    if (this._slices[instrumentId]) {
      this._slicesToRemove.push({
        ...this._slices[instrumentId],
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

    this._module._BarelyInstrument_SetSampleData(this._engine, instrumentId, slicesPtr, sliceCount);

    this._slices[instrumentId] = {
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
