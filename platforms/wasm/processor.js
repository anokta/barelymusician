import Module from '../../build/WebAssembly/bin/barelymusician.js';

const REFERENCE_FREQUENCY = 261.62555;

class Processor extends AudioWorkletProcessor {
  constructor() {
    super();

    this.module = null;
    this.engine = null;
    this.instruments = null;
    this.performers = null;
    this.tasks = null;
    this.triggers = null;

    Module().then((module) => {
      this.module = module;
      this.engine = new this.module.Engine(sampleRate, REFERENCE_FREQUENCY);
      this.instruments = {};
      this.performers = {};
      this.tasks = {};
      this.triggers = {};

      this.port.postMessage({type: 'init-success'});
    });

    this.port.onmessage = (event) => {
      if (!event.data) return;

      if (!this.module) {
        console.error('barelymusician not initialized!');
        return;
      }

      switch (event.data.type) {
        case 'engine-generate-random-integer': {
          this.port.postMessage({
            type: 'engine-generate-random-integer-response',
            value: this.engine.generateRandomInteger(event.data.min, event.data.max)
          });
        } break;
        case 'engine-generate-random-number': {
          this.port.postMessage({
            type: 'engine-generate-random-number-response',
            value: this.engine.generateRandomNumber(event.data.min, event.data.max)
          });
        } break;
        case 'engine-get-seed': {
          this.port.postMessage({type: 'engine-get-seed-response', seed: this.engine.seed});
        } break;
        case 'engine-get-tempo': {
          this.port.postMessage({type: 'engine-get-tempo-response', tempo: this.engine.tempo});
        } break;
        case 'engine-get-timestamp': {
          this.port.postMessage(
              {type: 'engine-get-timestamp-response', timestamp: this.engine.timestamp});
        } break;
        case 'engine-set-seed': {
          this.engine.seed = event.data.seed;
        } break;
        case 'engine-set-tempo': {
          this.engine.tempo = event.data.tempo;
        } break;
        case 'engine-start': {
          for (const [handle, performer] of this.performers) {
            performer.start();
          }
        } break;
        case 'engine-stop': {
          for (const [handle, performer] of this.performers) {
            performer.stop();
          }
        } break;
        case 'instrument-create': {
          const instrument = this.engine.createInstrument();
          const handle = instrument.getHandle();
          instrument.setNoteOnCallback(
              (pitch) => this.port.postMessage({type: 'instrument-on-note-on', handle, pitch}));
          instrument.setNoteOffCallback(
              (pitch) => this.port.postMessage({type: 'instrument-on-note-off', handle, pitch}));
          this.instruments[handle] = instrument;

          this.port.postMessage({type: 'instrument-create-success', handle})
        } break;
        case 'instrument-destroy': {
          delete this.instruments[event.data.handle];
        } break;
        case 'instrument-get-control': {
          if (this.instruments[event.data.handle]) {
            this.port.postMessage({
              type: 'instrument-get-control-response',
              handle: event.data.handle,
              value: this.instruments[event.data.handle].getControl(event.data.typeIndex)
            });
          }
        } break;
        case 'instrument-get-note-control': {
          if (this.instruments[event.data.handle]) {
            this.port.postMessage({
              type: 'instrument-get-note-control-response',
              handle: event.data.handle,
              value: this.instruments[event.data.handle].getNoteControl(
                  event.data.pitch, event.data.typeIndex)
            });
          }
        } break;
        case 'instrument-is-note-on': {
          if (this.instruments[event.data.handle]) {
            this.port.postMessage({
              type: 'instrument-is-note-on-response',
              handle: event.data.handle,
              isNoteOn: this.instruments[event.data.handle].isNoteOn(event.data.pitch)
            });
          }
        } break;
        case 'instrument-set-control': {
          this.instruments[event.data.handle]?.setControl(event.data.typeIndex, event.data.value);
        } break;
        case 'instrument-set-note-control': {
          this.instruments[event.data.handle]?.setNoteControl(
              event.data.pitch, event.data.typeIndex, event.data.value);
        } break;
        case 'instrument-set-all-notes-off': {
          this.instruments[event.data.handle]?.setAllNotesOff();
        } break;
        case 'instrument-set-note-on': {
          this.instruments[event.data.handle]?.setNoteOn(
              event.data.pitch, event.data.gain, event.data.pitchShift);
        } break;
        case 'instrument-set-note-off': {
          this.instruments[event.data.handle]?.setNoteOff(event.data.pitch);
        } break;
        case 'performer-create': {
          const performer = this.engine.createPerformer();
          const handle = performer.getHandle();
          this.performers[handle] = performer;
          this.port.postMessage({type: 'performer-create-success', handle})
        } break;
        case 'performer-destroy': {
          delete this.performers[event.data.handle];
        } break;
        case 'performer-get-loop-begin-position': {
          this.port.postMessage({
            type: 'performer-get-loop-begin-position-response',
            loopBeginPosition: this.performers[event.data.handle].loopBeginPosition,
          });
        } break;
        case 'performer-get-loop-length': {
          this.port.postMessage({
            type: 'performer-get-loop-length',
            loopLength: this.performers[event.data.handle].loopLength,
          });
        } break;
        case 'performer-get-position': {
          this.port.postMessage({
            type: 'performer-get-position-response',
            position: this.performers[event.data.handle].position,
          });
        } break;
        case 'performer-is-looping': {
          this.port.postMessage({
            type: 'performer-is-looping-response',
            isLooping: this.performers[event.data.handle].isLooping,
          });
        } break;
        case 'performer-set-loop-begin-position': {
          if (this.performers[event.data.handle]) {
            this.performers[event.data.handle].loopBeginPosition = event.data.loopBeginPosition;
          }
        } break;
        case 'performer-set-loop-length': {
          if (this.performers[event.data.handle]) {
            this.performers[event.data.handle].loopLength = event.data.loopLength;
          }
        } break;
        case 'performer-set-looping': {
          if (this.performers[event.data.handle]) {
            this.performers[event.data.handle].looping = event.data.isLooping;
          }
        } break;
        case 'performer-set-position': {
          if (this.performers[event.data.handle]) {
            this.performers[event.data.handle].position = event.data.position;
          }
        } break;
        case 'performer-start': {
          this.performers[event.data.handle]?.start();
        } break;
        case 'performer-stop': {
          this.performers[event.data.handle]?.stop();
        } break;
        case 'task-create': {
          if (this.performers[event.data.performerHandle]) {
            const task = this.performers[event.data.performerHandle].createTask(
                event.data.position, event.data.duration, event.data.callback);
            const handle = task.getHandle();
            task.setProcessCallback(
                (state) => this.port.postMessage({type: 'task-on-process', handle, state}));
            this.tasks[handle] = task;

            this.port.postMessage({type: 'task-create-success', handle});
          }
        } break;
        case 'task-set-position': {
          if (this.tasks[event.data.handle]) {
            this.tasks[event.data.handle].position = event.data.position;
          }
        } break;
        case 'trigger-create': {
          if (this.performers[event.data.performerHandle]) {
            const trigger =
                this.performers[event.data.performerHandle].createTrigger(event.data.position);
            const handle = trigger.getHandle();
            trigger.setProcessCallback(
                () => this.port.postMessage({type: 'trigger-on-process', handle}));
            this.triggers[handle] = trigger;

            this.port.postMessage({type: 'trigger-create-success', handle});
          }
        } break;
        case 'trigger-set-position': {
          if (this.triggers[event.data.handle]) {
            this.triggers[event.data.handle].position = event.data.position;
          }
        } break;
        default:
          console.error('Unknown message!');
      }
    };
  }

  process(inputs, outputs, parameters) {
    if (!this.engine || !this.module || !this.module.HEAPF32) return true;

    const output = outputs[0][0];
    const outputLength = output.length;
    const outputSize = outputLength * Float32Array.BYTES_PER_ELEMENT;

    const latency = Math.max(1.0 / 60.0, outputLength / sampleRate);
    this.engine.update(currentTime + latency);

    const outputPtr = this.module._malloc(outputSize);
    const outputSamples = new Float32Array(this.module.HEAPF32.buffer, outputPtr, outputLength);
    outputSamples.fill(0);

    const tempPtr = this.module._malloc(outputSize);
    const tempSamples = new Float32Array(this.module.HEAPF32.buffer, tempPtr, outputLength);

    Object.values(this.instruments).forEach(instrument => {
      instrument.process(tempPtr, outputLength, currentTime);
      for (let i = 0; i < outputLength; ++i) {
        outputSamples[i] += tempSamples[i];
      }
    });
    output.set(outputSamples);

    this.module._free(tempPtr);
    this.module._free(outputPtr);

    return true;
  }
}

registerProcessor('barelymusician-processor', Processor);
