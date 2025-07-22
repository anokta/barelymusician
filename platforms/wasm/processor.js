import Module from './barelymusician.js';

const REFERENCE_FREQUENCY = 261.62555;

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
      this._engine = new this._module.Engine(sampleRate, REFERENCE_FREQUENCY);
      this._instruments = {};
      this._performers = {};
      this._tasks = {};

      // TODO(#164): Temp workaround to sync performers.
      this._metronome = null;

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
        case 'engine-get-timestamp': {
          this.port.postMessage(
              {type: 'engine-get-timestamp-response', timestamp: this._engine.timestamp});
        } break;
        case 'engine-set-seed': {
          this._engine.seed = event.data.seed;
        } break;
        case 'engine-set-tempo': {
          this._engine.tempo = event.data.tempo;
        } break;
        case 'instrument-create': {
          const instrument = this._engine.createInstrument();
          const handle = instrument.getHandle();
          instrument.setNoteOnCallback(
              (pitch) => this.port.postMessage({type: 'instrument-on-note-on', handle, pitch}));
          instrument.setNoteOffCallback(
              (pitch) => this.port.postMessage({type: 'instrument-on-note-off', handle, pitch}));
          this._instruments[handle] = instrument;

          this.port.postMessage({type: 'instrument-create-success', handle})
        } break;
        case 'instrument-destroy': {
          if (this._instruments[event.data.handle]) {
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
        case 'performer-create': {
          const performer = this._engine.createPerformer();
          const handle = performer.getHandle();
          this._performers[handle] = performer;
          this.port.postMessage({type: 'performer-create-success', handle});

          // TODO(#164): Temp workaround to sync performers.
          if (Object.keys(this._performers).length === 1) {
            this._metronome = performer;
          } else if (this._metronome && this._metronome.isPlaying) {
            performer.position = this._metronome.position;
            performer.start();
          }
        } break;
        case 'performer-destroy': {
          if (this._performers[event.data.handle]) {
            delete this._performers[event.data.handle];
            this.port.postMessage({type: 'performer-destroy-success', handle: event.data.handle});
          }

          // TODO(#164): Temp workaround to sync performers.
          if (Object.keys(this._performers).length === 0) {
            this._metronome = null;
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

          // TODO(#164): Temp workaround to sync performers.
          if (this._performers[event.data.handle] && this._metronome) {
            this._performers[event.data.handle].position = this._metronome.position;
          }
        } break;
        case 'performer-set-loop-length': {
          if (this._performers[event.data.handle]) {
            this._performers[event.data.handle].loopLength = event.data.loopLength;
          }

          // TODO(#164): Temp workaround to sync performers.
          if (this._performers[event.data.handle] && this._metronome) {
            this._performers[event.data.handle].position = this._metronome.position;
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

          // TODO(#164): Temp workaround to sync performers.
          if (this._performers[event.data.handle] && this._metronome) {
            this._performers[event.data.handle].position = this._metronome.position;
          }
        } break;
        case 'performer-start': {
          this._performers[event.data.handle]?.start();

          // TODO(#164): Temp workaround to sync performers.
          if (this._performers[event.data.handle] && this._metronome) {
            this._performers[event.data.handle].position = this._metronome.position;
          }
        } break;
        case 'performer-stop': {
          this._performers[event.data.handle]?.stop();

          // TODO(#164): Temp workaround to sync performers.
          if (this._performers[event.data.handle] && this._metronome) {
            this._performers[event.data.handle].position = this._metronome.position;
          }
        } break;
        case 'task-create': {
          if (this._performers[event.data.performerHandle]) {
            const task = this._performers[event.data.performerHandle].createTask(
                event.data.position, event.data.duration);
            const handle = task.getHandle();
            task.setProcessCallback(
                (state) => this.port.postMessage(
                    {type: 'task-on-process', handle, position: task.position, state}));
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
          console.error('Unknown message!');
      }
    };
  }

  /**
   * Audio processing callback.
   */
  process(inputs, outputs, parameters) {
    if (!this._engine || !this._module || !this._module.HEAPF32) return true;

    const output = outputs[0][0];
    const outputLength = output.length;
    const outputSize = outputLength * Float32Array.BYTES_PER_ELEMENT;

    const latency = Math.max(1.0 / 60.0, outputLength / sampleRate);
    this._engine.update(currentTime + latency);

    const outputPtr = this._module._malloc(outputSize);
    const outputSamples = new Float32Array(this._module.HEAPF32.buffer, outputPtr, outputLength);

    this._engine.process(outputPtr, outputLength, currentTime);
    output.set(outputSamples);

    this._module._free(outputPtr);

    return true;
  }
}

registerProcessor('barelymusician-processor', Processor);
