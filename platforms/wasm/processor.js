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
        case 'instrument-create': {
          let instrument = this.engine.createInstrument();
          instrument.setNoteOnCallback(
              (pitch) => this.port.postMessage({type: 'instrument-on-note-on', handle, pitch}));
          instrument.setNoteOffCallback(
              (pitch) => this.port.postMessage({type: 'instrument-on-note-off', handle, pitch}));

          let handle = instrument.getHandle();
          this.instruments[handle] = instrument;
          this.port.postMessage({type: 'instrument-create-success', handle})
        } break;
        case 'instrument-create': {
          let instrument = this.engine.createInstrument();
          instrument.setNoteOnCallback(
              (pitch) => this.port.postMessage({type: 'instrument-on-note-on', handle, pitch}));
          instrument.setNoteOffCallback(
              (pitch) => this.port.postMessage({type: 'instrument-on-note-off', handle, pitch}));

          let handle = instrument.getHandle();
          this.instruments[handle] = instrument;
          this.port.postMessage({type: 'instrument-create-success', handle})
        } break;
        case 'instrument-destroy': {
          delete this.instruments[event.data.handle];
        } break;
        case 'instrument-get-control': {
          if (event.data.handle in instruments) {
            this.port.postMessage({
              type: 'instrument-get-control-response',
              handle: event.data.handle,
              value: this.instruments[event.data.handle].getControl(event.data.typeIndex)
            });
          }
        } break;
        case 'instrument-get-note-control': {
          if (event.data.handle in instruments) {
            this.port.postMessage({
              type: 'instrument-get-note-control-response',
              handle: event.data.handle,
              value: this.instruments[event.data.handle].getNoteControl(
                  event.data.pitch, event.data.typeIndex)
            });
          }
        } break;
        case 'instrument-is-note-on': {
          if (event.data.handle in instruments) {
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
          this.instruments[event.data.handle]?.setNoteOn(event.data.pitch, event.data.gain);
        } break;
        case 'instrument-set-note-off': {
          this.instruments[event.data.handle]?.setNoteOff(event.data.pitch);
        } break;
        case 'performer-create': {
          let performer = this.engine.createPerformer();
          let handle = performer.getHandle();
          this.performers[handle] = performer;
          this.port.postMessage({type: 'performer-create-success', handle})
        } break;
        case 'performer-destroy': {
          delete this.performers[event.data.handle];
        } break;
        case 'performer-set-position': {
          if (event.data.handle in this.performers) {
            this.performers[event.data.handle].position = event.data.position;
          }
        } break;
        case 'performer-set-loop-begin-position': {
          if (event.data.handle in this.performers) {
            this.performers[event.data.handle].loopBeginPosition = event.data.loopBeginPosition;
          }
        } break;
        case 'performer-set-loop-length': {
          if (event.data.handle in this.performers) {
            this.performers[event.data.handle].loopLength = event.data.loopLength;
          }
        } break;
        case 'performer-set-looping': {
          if (event.data.handle in this.performers) {
            this.performers[event.data.handle].looping = event.data.looping;
          }
        } break;
        case 'performer-start': {
          this.performers[event.data.handle]?.start();
        } break;
        case 'performer-stop': {
          this.performers[event.data.handle]?.stop();
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
