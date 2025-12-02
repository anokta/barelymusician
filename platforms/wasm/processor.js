import Module from './barelymusician.js';

const RENDER_QUANTUM_SIZE = 128

export const EngineControlType = {
  COMPRESSOR_MIX: 0,
  COMPRESSOR_ATTACK: 1,
  COMPRESSOR_RELEASE: 2,
  COMPRESSOR_THRESHOLD: 3,
  COMPRESSOR_RATIO: 4,
  DELAY_MIX: 5,
  DELAY_TIME: 6,
  DELAY_FEEDBACK: 7,
  DELAY_LOW_PASS_FREQUENCY: 8,
  DELAY_HIGH_PASS_FREQUENCY: 9,
  SIDECHAIN_MIX: 10,
  SIDECHAIN_ATTACK: 11,
  SIDECHAIN_RELEASE: 12,
  SIDECHAIN_THRESHOLD: 13,
  SIDECHAIN_RATIO: 14,
  COUNT: 15,
};

class Processor extends AudioWorkletProcessor {
  constructor() {
    super();

    this._module = null;
    this._engine = null;
    this._instruments = null;
    this._performers = null;
    this._tasks = null;

    Module().then(module => {
      this._module = module;
      this._engine = new this._module.Engine(sampleRate, RENDER_QUANTUM_SIZE);
      this._instruments = {};
      this._performers = {};
      this._tasks = {};

      this.port.postMessage({type: 'init-success'});
    });

    this.port.onmessage = event => {
      if (!event.data) return;

      if (!this._module) {
        console.error('barelymusician not initialized!');
        return;
      }

      switch (event.data.type) {
        case 'engine-generate-random-integer': {
          this.port.postMessage({
            type: 'engine-generate-random-integer-response',
            value: this._engine.generateRandomInteger(event.data.min, event.data.max)
          });
        } break;
        case 'engine-generate-random-number': {
          this.port.postMessage({
            type: 'engine-generate-random-number-response',
            value: this._engine.generateRandomNumber(event.data.min, event.data.max)
          });
        } break;
        case 'engine-get-seed': {
          this.port.postMessage({type: 'engine-get-seed-response', seed: this._engine.seed});
        } break;
        case 'engine-get-tempo': {
          this.port.postMessage({type: 'engine-get-tempo-response', tempo: this._engine.tempo});
        } break;
        case 'engine-set-delay-time': {
          this._engine.setControl(EngineControlType.DELAY_TIME, event.data.delayTime);
        } break;
        case 'engine-set-delay-feedback': {
          this._engine.setControl(EngineControlType.DELAY_FEEDBACK, event.data.delayFeedback);
        } break;
        case 'engine-set-seed': {
          this._engine.seed = event.data.seed;
        } break;
        case 'engine-set-tempo': {
          this._engine.tempo = event.data.tempo;
        } break;
        case 'engine-update': {
          const latency = Math.max(1.0 / 60.0, RENDER_QUANTUM_SIZE / sampleRate);
          this._engine.update(currentTime + latency);
        } break;
        case 'instrument-create': {
          const instrument = this._engine.createInstrument();
          const handle = instrument.getHandle();
          instrument.setNoteEventCallback((eventType, pitch) => {
            const NoteEventType = {
              BEGIN: 0,
              END: 1,
              COUNT: 2,
            };
            if (eventType == NoteEventType.BEGIN) {
              this.port.postMessage({type: 'instrument-on-note-on', handle, pitch});
            } else if (eventType == NoteEventType.END) {
              this.port.postMessage({type: 'instrument-on-note-off', handle, pitch});
            } else {
              console.error(`Invalid note event type: ${eventType}`);
            }
          });
          this._instruments[handle] = instrument;

          this.port.postMessage({type: 'instrument-create-success', handle})
        } break;
        case 'instrument-destroy': {
          if (this._instruments[event.data.handle]) {
            this._instruments[event.data.handle].delete();
            delete this._instruments[event.data.handle];
            this.port.postMessage({type: 'instrument-destroy-success', handle: event.data.handle});
          }
        } break;
        case 'instrument-get-control': {
          if (this._instruments[event.data.handle]) {
            this.port.postMessage({
              type: 'instrument-get-control-response',
              handle: event.data.handle,
              value: this._instruments[event.data.handle].getControl(event.data.typeIndex)
            });
          }
        } break;
        case 'instrument-get-note-control': {
          if (this._instruments[event.data.handle]) {
            this.port.postMessage({
              type: 'instrument-get-note-control-response',
              handle: event.data.handle,
              value: this._instruments[event.data.handle].getNoteControl(
                  event.data.pitch, event.data.typeIndex)
            });
          }
        } break;
        case 'instrument-is-note-on': {
          if (this._instruments[event.data.handle]) {
            this.port.postMessage({
              type: 'instrument-is-note-on-response',
              handle: event.data.handle,
              isNoteOn: this._instruments[event.data.handle].isNoteOn(event.data.pitch)
            });
          }
        } break;
        case 'instrument-set-control': {
          this._instruments[event.data.handle]?.setControl(event.data.typeIndex, event.data.value);
        } break;
        case 'instrument-set-note-control': {
          this._instruments[event.data.handle]?.setNoteControl(
              event.data.pitch, event.data.typeIndex, event.data.value);
        } break;
        case 'instrument-set-all-notes-off': {
          this._instruments[event.data.handle]?.setAllNotesOff();
        } break;
        case 'instrument-set-note-on': {
          this._instruments[event.data.handle]?.setNoteOn(
              event.data.pitch, event.data.gain, event.data.pitchShift);
        } break;
        case 'instrument-set-note-off': {
          this._instruments[event.data.handle]?.setNoteOff(event.data.pitch);
        } break;
        case 'instrument-set-sample-data': {
          this._setInstrumentSampleData(event.data.handle, event.data.slices);
        } break;
        case 'performer-create': {
          const performer = this._engine.createPerformer();
          const handle = performer.getHandle();
          this._performers[handle] = performer;
          this.port.postMessage({type: 'performer-create-success', handle});
        } break;
        case 'performer-destroy': {
          if (this._performers[event.data.handle]) {
            this._performers[event.data.handle].delete();
            delete this._performers[event.data.handle];
            this.port.postMessage({type: 'performer-destroy-success', handle: event.data.handle});
          }
        } break;
        case 'performer-get-properties': {
          const performer = this._performers[event.data.handle];
          if (performer) {
            this.port.postMessage({
              type: 'performer-get-properties-response',
              handle: event.data.handle,
              isLooping: performer.isLooping,
              isPlaying: performer.isPlaying,
              loopBeginPosition: performer.loopBeginPosition,
              loopLength: performer.loopLength,
              position: performer.position,
            });
          }
        } break;
        case 'performer-set-loop-begin-position': {
          if (this._performers[event.data.handle]) {
            this._performers[event.data.handle].loopBeginPosition = event.data.loopBeginPosition;
          }
        } break;
        case 'performer-set-loop-length': {
          if (this._performers[event.data.handle]) {
            this._performers[event.data.handle].loopLength = event.data.loopLength;
          }
        } break;
        case 'performer-set-looping': {
          if (this._performers[event.data.handle]) {
            this._performers[event.data.handle].isLooping = event.data.isLooping;
          }
        } break;
        case 'performer-set-position': {
          if (this._performers[event.data.handle]) {
            this._performers[event.data.handle].position = event.data.position;
          }
        } break;
        case 'performer-start': {
          this._performers[event.data.handle]?.start();
        } break;
        case 'performer-stop': {
          this._performers[event.data.handle]?.stop();
        } break;
        case 'task-create': {
          if (this._performers[event.data.performerHandle]) {
            const task = this._performers[event.data.performerHandle].createTask(
                event.data.position, event.data.duration);
            const handle = task.getHandle();
            task.setEventCallback(
                (eventType) => this.port.postMessage(
                    {type: 'task-on-event', handle, position: task.position, eventType}));
            this._tasks[handle] = task;

            this.port.postMessage({
              type: 'task-create-success',
              performerHandle: event.data.performerHandle,
              handle,
            });
          }
        } break;
        case 'task-destroy': {
          if (this._tasks[event.data.handle]) {
            this._tasks[event.data.handle].delete();
            delete this._tasks[event.data.handle];
            this.port.postMessage({type: 'task-destroy-success', handle: event.data.handle});
          }
        } break;
        case 'task-get-properties': {
          const task = this._tasks[event.data.handle];
          if (task) {
            this.port.postMessage({
              type: 'task-get-properties-response',
              handle: event.data.handle,
              duration: task.duration,
              isActive: task.isActive,
              position: task.position,
            });
          }
        } break;
        case 'task-set-duration': {
          if (this._tasks[event.data.handle]) {
            this._tasks[event.data.handle].duration = event.data.duration;
          }
        } break;
        case 'task-set-position': {
          if (this._tasks[event.data.handle]) {
            this._tasks[event.data.handle].position = event.data.position;
          }
        } break;
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
    const outputChannelCount = output.length;
    const outputFrameCount = output[0].length;
    const outputSampleCount = outputChannelCount * outputFrameCount;

    if (outputSampleCount == 0) return true;
    if (outputFrameCount > RENDER_QUANTUM_SIZE) return true;

    const outputSamplesPtr =
        this._module._malloc(outputSampleCount * Float32Array.BYTES_PER_ELEMENT);
    const outputSamples =
        new Float32Array(this._module.HEAPF32.buffer, outputSamplesPtr, outputSampleCount);

    this._engine.process(outputSamplesPtr, outputChannelCount, outputFrameCount, currentTime);

    for (let frame = 0; frame < outputFrameCount; ++frame) {
      for (let channel = 0; channel < outputChannelCount; ++channel) {
        output[channel][frame] = outputSamples[frame * outputChannelCount + channel];
      }
    }

    this._module._free(outputSamplesPtr);

    return true;
  }

  /**
   * Sets instrument sample data.
   * @private
   */
  _setInstrumentSampleData(handle, slices) {
    if (!this._instruments[handle]) return;

    const sliceCount = slices.length;
    const sliceStructSize = 24;  // sizeof(BarelySlice)

    const slicesPtr = this._module._malloc(sliceCount * sliceStructSize);
    const samplePtrs = [];

    for (let i = 0; i < sliceCount; ++i) {
      const samples = slices[i].samples;
      const sampleCount = samples.length;

      const samplesPtr = this._module._malloc(sampleCount * Float32Array.BYTES_PER_ELEMENT);
      this._module.HEAPF32.set(samples, samplesPtr / Float32Array.BYTES_PER_ELEMENT);
      samplePtrs.push(samplesPtr);

      const offset = slicesPtr + i * sliceStructSize;
      this._module.HEAPF32[offset / 4] = slices[i].root_pitch;
      this._module.HEAP32[offset / 4 + 1] = slices[i].sample_rate;
      this._module.HEAP32[offset / 4 + 2] = samplesPtr;
      this._module.HEAP32[offset / 4 + 3] = sampleCount;
    }

    this._instruments[handle].setSampleData(slicesPtr, sliceCount);

    for (const ptr of samplePtrs) {
      this._module._free(ptr);
    }
    this._module._free(slicesPtr);
  }
}

registerProcessor('barelymusician-processor', Processor);
