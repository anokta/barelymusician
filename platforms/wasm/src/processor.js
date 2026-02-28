import Module from './barelymusician.js';
import {CommandType, EventCallbackType, MessageType} from './command.js'
import {INSTRUMENT_CONTROLS, NoteControlType} from './control.js';

const RENDER_QUANTUM_SIZE = 128;
const STEREO_CHANNEL_COUNT = 2;

const SLICE_SIZE = 24;  // sizeof(BarelySlice)

class Processor extends AudioWorkletProcessor {
  constructor() {
    super();

    this._module = null;
    this._engine = null;

    this._doublePtr = null;
    this._uint8Ptr = null;
    this._uint32Ptr = null;
    this._outputSamplesPtr = null;

    this._pendingEventCallbacks = [];

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
      this._outputSamplesPtr = this._module._malloc(
          STEREO_CHANNEL_COUNT * RENDER_QUANTUM_SIZE * Float32Array.BYTES_PER_ELEMENT);

      this._module._BarelyEngine_Create(sampleRate, this._uint32Ptr);
      this._engine = this._module.getValue(this._uint32Ptr, 'i32');

      this.port.postMessage({type: MessageType.INIT_SUCCESS});
    });

    this.port.onmessage = (event) => {
      if (!event.data) return;

      if (!this._engine) {
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

      const performer_properties = [];
      for (const [handle, value] of this._performers) {
        this._module._BarelyPerformer_GetPosition(this._engine, value.performerId, this._doublePtr);
        performer_properties.push(
            {handle, position: this._module.getValue(this._doublePtr, 'double')});
      }
      const task_properties = [];
      for (const [handle, value] of this._tasks) {
        this._module._BarelyTask_IsActive(this._engine, value.taskId, this._uint8Ptr);
        task_properties.push({handle, isActive: (this._module.getValue(this._uint8Ptr) !== 0)});
      }

      if (performer_properties.length > 0 || task_properties.length > 0 ||
          this._pendingEventCallbacks.length > 0) {
        this.port.postMessage({
          type: MessageType.UPDATE_SUCCESS,
          eventCallbacks: this._pendingEventCallbacks,
          performer_properties,
          task_properties,
        });
        this._pendingEventCallbacks = [];
      }
    };
  }

  /**
   * Audio processing callback.
   */
  process(inputs, outputs, parameters) {
    if (!this._engine || !this._module) return true;

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
        this._module._BarelyEngine_SetControl(this._engine, command.typeIndex, command.value);
        break;
      case CommandType.ENGINE_SET_TEMPO:
        this._module._BarelyEngine_SetTempo(this._engine, command.tempo);
        break;
      case CommandType.INSTRUMENT_CREATE: {
        this._module._BarelyEngine_CreateInstrument(this._engine, this._uint32Ptr);
        const instrumentId = this._module.getValue(this._uint32Ptr, 'i32');
        if (!instrumentId) return;

        for (const controlTypeIndex in INSTRUMENT_CONTROLS) {
          this._module._BarelyInstrument_SetControl(
              this._engine, instrumentId, controlTypeIndex,
              INSTRUMENT_CONTROLS[controlTypeIndex].defaultValue);
        }

        const NoteEventType = {
          BEGIN: 0,
          END: 1,
        };
        const noteEventCallback = (eventType, pitch) => {
          if (eventType === NoteEventType.BEGIN) {
            this._pendingEventCallbacks.push(
                {type: EventCallbackType.INSTRUMENT_ON_NOTE_BEGIN, handle: command.handle, pitch});
          } else if (eventType === NoteEventType.END) {
            this._pendingEventCallbacks.push(
                {type: EventCallbackType.INSTRUMENT_ON_NOTE_END, handle: command.handle, pitch});
          } else {
            console.error(`Invalid note event: ${eventType}`);
          }
        };
        const noteEventCallbackPtr = this._module.addFunction((eventType, pitch, userData) => {
          const callback = this._instruments.get(userData)?.noteEventCallback;
          if (callback) {
            callback(eventType, pitch);
          }
        }, 'vifi');
        this._module._BarelyInstrument_SetNoteEventCallback(
            this._engine, instrumentId, noteEventCallbackPtr, command.handle);
        this._instruments.set(
            command.handle, {instrumentId, noteEventCallback, noteEventCallbackPtr});
      } break;
      case CommandType.INSTRUMENT_DESTROY: {
        const {instrumentId, noteEventCallbackPtr} = this._instruments.get(command.handle);
        if (!instrumentId) return;
        this._module._BarelyEngine_DestroyInstrument(this._engine, instrumentId);
        if (this._slices[instrumentId]) {
          this._slicesToRemove.push({
            ...this._slices[instrumentId],
            timestamp: this._timestamp,
          });
          delete this._slices[instrumentId];
        }
        this._module.removeFunction(noteEventCallbackPtr);
        this._instruments.delete(command.handle);
      } break;
      case CommandType.INSTRUMENT_SET_ALL_NOTES_OFF: {
        const instrumentId = this._instruments.get(command.handle)?.instrumentId;
        if (!instrumentId) return;
        this._module._BarelyInstrument_SetAllNotesOff(this._engine, instrumentId);
      } break;
      case CommandType.INSTRUMENT_SET_CONTROL: {
        const instrumentId = this._instruments.get(command.handle)?.instrumentId;
        if (!instrumentId) return;
        this._module._BarelyInstrument_SetControl(
            this._engine, instrumentId, command.typeIndex, command.value);
      } break;
      case CommandType.INSTRUMENT_SET_NOTE_CONTROL: {
        const instrumentId = this._instruments.get(command.handle)?.instrumentId;
        if (!instrumentId) return;
        this._module._BarelyInstrument_SetNoteControl(
            this._engine, instrumentId, command.pitch, command.typeIndex, command.value);
      } break;
      case CommandType.INSTRUMENT_SET_NOTE_OFF: {
        const instrumentId = this._instruments.get(command.handle)?.instrumentId;
        if (!instrumentId) return;
        this._module._BarelyInstrument_SetNoteOff(this._engine, instrumentId, command.pitch);
      } break;
      case CommandType.INSTRUMENT_SET_NOTE_ON: {
        const instrumentId = this._instruments.get(command.handle)?.instrumentId;
        if (!instrumentId) return;
        this._module._BarelyInstrument_SetNoteOn(this._engine, instrumentId, command.pitch);
        if (command.gain != 1.0) {
          this._module._BarelyInstrument_SetControl(
              this._engine, instrumentId, NoteControlType.GAIN, command.gain);
        }
        if (command.pitchShift != 0.0) {
          this._module._BarelyInstrument_SetControl(
              this._engine, instrumentId, NoteControlType.PITCH_SHIFT, command.pitchShift);
        }
      } break;
      case CommandType.INSTRUMENT_SET_SAMPLE_DATA: {
        const instrumentId = this._instruments.get(command.handle)?.instrumentId;
        if (!instrumentId) return;
        this._setInstrumentSampleData(instrumentId, command.slices);
      } break;
      case CommandType.PERFORMER_CREATE: {
        this._module._BarelyEngine_CreatePerformer(this._engine, this._uint32Ptr);
        const performerId = this._module.getValue(this._uint32Ptr, 'i32');
        this._performers.set(command.handle, {performerId});
      } break;
      case CommandType.PERFORMER_DESTROY: {
        const performerId = this._performers.get(command.handle)?.performerId;
        if (!performerId) return;
        this._module._BarelyEngine_DestroyPerformer(this._engine, performerId);
        this._performers.delete(command.handle);
      } break;
      case CommandType.PERFORMER_SET_LOOP_BEGIN_POSITION: {
        const performerId = this._performers.get(command.handle)?.performerId;
        if (!performerId) return;
        this._module._BarelyPerformer_SetLoopBeginPosition(
            this._engine, performerId, command.loopBeginPosition);
      } break;
      case CommandType.PERFORMER_SET_LOOP_LENGTH: {
        const performerId = this._performers.get(command.handle)?.performerId;
        if (!performerId) return;
        this._module._BarelyPerformer_SetLoopLength(this._engine, performerId, command.loopLength);
      } break;
      case CommandType.PERFORMER_SET_LOOPING: {
        const performerId = this._performers.get(command.handle)?.performerId;
        if (!performerId) return;
        this._module._BarelyPerformer_SetLooping(this._engine, performerId, command.isLooping);
      } break;
      case CommandType.PERFORMER_SET_POSITION: {
        const performerId = this._performers.get(command.handle)?.performerId;
        if (!performerId) return;
        this._module._BarelyPerformer_SetPosition(this._engine, performerId, command.position);
      } break;
      case CommandType.PERFORMER_START: {
        const performerId = this._performers.get(command.handle)?.performerId;
        if (!performerId) return;
        this._module._BarelyPerformer_Start(this._engine, performerId);
      } break;
      case CommandType.PERFORMER_STOP: {
        const performerId = this._performers.get(command.handle)?.performerId;
        if (!performerId) return;
        this._module._BarelyPerformer_Stop(this._engine, performerId);
      } break;
      case CommandType.PERFORMER_SYNC_TO: {
        const performerId = this._performers.get(command.handle)?.performerId;
        if (!performerId) return;
        const otherPerformerId = this._performers.get(command.otherHandle)?.performerId;
        if (!otherPerformerId) return;
        this._module._BarelyPerformer_GetPosition(this._engine, otherPerformerId, this._doublePtr);
        this._module._BarelyPerformer_SetPosition(
            this._engine, performerId, this._module.getValue(this._doublePtr, 'double'));
      } break;
      case CommandType.TASK_CREATE: {
        this._module._BarelyEngine_CreateTask(
            this._engine, this._performers.get(command.performerHandle)?.performerId ?? 0,
            command.position, command.duration, command.priority, null, null, this._uint32Ptr);
        const taskId = this._module.getValue(this._uint32Ptr, 'i32');

        const TaskEventType = {
          BEGIN: 0,
          END: 1,
        };
        const eventCallback = (task, eventType) => {
          if (eventType === TaskEventType.BEGIN) {
            if (task.beginCommands) {
              for (const command of task.beginCommands) {
                this._processCommand(command);
              }
            }
            this._pendingEventCallbacks.push(
                {type: EventCallbackType.TASK_ON_BEGIN, handle: command.handle});
          } else if (eventType === TaskEventType.END) {
            if (task.endCommands) {
              for (const command of task.endCommands) {
                this._processCommand(command);
              }
            }
            this._pendingEventCallbacks.push(
                {type: EventCallbackType.TASK_ON_END, handle: command.handle});
          } else {
            console.error(`Invalid task event: ${eventType}`);
          }
        };
        const eventCallbackPtr = this._module.addFunction((eventType, userData) => {
          const task = this._tasks.get(userData);
          if (!task) return;
          if (task.eventCallback) {
            task.eventCallback(task, eventType);
          }
        }, 'vii');
        this._module._BarelyTask_SetEventCallback(
            this._engine, taskId, eventCallbackPtr, command.handle);

        this._tasks.set(command.handle, {taskId, eventCallback, eventCallbackPtr});
      } break;
      case CommandType.TASK_DESTROY: {
        const {taskId, eventCallbackPtr} = this._tasks.get(command.handle);
        if (!taskId) return;
        this._module._BarelyEngine_DestroyTask(this._engine, taskId);
        this._module.removeFunction(eventCallbackPtr);
        this._tasks.delete(command.handle);
      } break;
      case CommandType.TASK_SET_COMMANDS: {
        const task = this._tasks.get(command.handle);
        if (!task) return;
        if (task.endCommands) {  // stop the current task
          this._module._BarelyTask_IsActive(this._engine, task.taskId, this._uint8Ptr);
          if (this._module.getValue(this._uint8Ptr) !== 0) {
            for (const command of task.endCommands) {
              this._processCommand(command);
            }
          }
        }
        task.beginCommands = command.beginCommands;
        task.endCommands = command.endCommands;
      } break;
      case CommandType.TASK_SET_DURATION: {
        const taskId = this._tasks.get(command.handle)?.taskId;
        if (!taskId) return;
        this._module._BarelyTask_SetDuration(this._engine, taskId, command.duration);
      } break;
      case CommandType.TASK_SET_POSITION: {
        const taskId = this._tasks.get(command.handle)?.taskId;
        if (!taskId) return;
        this._module._BarelyTask_SetPosition(this._engine, taskId, command.position);
      } break;
      case CommandType.TASK_SET_PRIORITY: {
        const taskId = this._tasks.get(command.handle)?.taskId;
        if (!taskId) return;
        this._module._BarelyTask_SetPriority(this._engine, taskId, command.priority);
      } break;
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
