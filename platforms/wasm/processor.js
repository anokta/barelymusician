import Module from '../../build/WebAssembly/bin/barelymusician.js';

import {ControlType} from './instrument.js'

const REFERENCE_FREQUENCY = 261.62555;

class Processor extends AudioWorkletProcessor {
  constructor() {
    super();

    this.module = null;
    this.engine = null;
    this.instruments = {};
    this.performers = {};

    this.port.onmessage = async (event) => {
      if (!event.data) {
        return;
      }

      if (!this.module) {
        this.module = await Module();
        this.engine = new this.module.Engine(sampleRate, REFERENCE_FREQUENCY);
        this.instruments = {};
        this.performers = {};
        console.log('barelymusician initialized');
      }

      switch (event.data.type) {
        case 'instrument-create': {
          let instrument = this.engine.createInstrument();
          let instrumentHandle = instrument.getHandle();
          this.instruments[instrumentHandle] = instrument;

          instrument.setControl(ControlType.ATTACK, 0.05);
          instrument.setControl(ControlType.GAIN, 0.5);
          instrument.setControl(ControlType.RELEASE, 0.1);
          instrument.setNoteOnCallback((pitch) => {
            this.port.postMessage({
              type: 'instrument-on-note-on',
              handle: instrumentHandle,
              pitch: pitch
            });
          });
          instrument.setNoteOffCallback((pitch) => {
            this.port.postMessage({
              type: 'instrument-on-note-off',
              handle: instrumentHandle,
              pitch: pitch
            });
          });

          this.port.postMessage(
              {type: 'instrument-create-success', handle: instrumentHandle})
        } break;
        case 'instrument-destroy': {
          delete this.instruments[event.data.handle];
        } break;
        case 'instrument-set-control': {
          this.instruments[event.data.handle].setControl(
              event.data.typeIndex, event.data.value);
        } break;
        case 'instrument-set-note-on': {
          this.instruments[event.data.handle].setNoteOn(
              event.data.pitch, event.data.gain);
        } break;
        case 'instrument-set-note-off': {
          this.instruments[event.data.handle].setNoteOff(event.data.pitch);
        } break;
        default:
          console.log('ERROR: Unknown message!');
      }
    };
  }

  process(inputs, outputs, parameters) {
    if (!this.engine || !this.module || !this.module.HEAPF32) {
      return true;
    }

    const output = outputs[0][0];
    const output_length = output.length;
    const output_size = output_length * Float32Array.BYTES_PER_ELEMENT;

    this.engine.update(currentTime + output_length / sampleRate);

    const output_ptr = this.module._malloc(output_size);
    const output_samples =
        new Float32Array(this.module.HEAPF32.buffer, output_ptr, output_length);
    output_samples.fill(0);

    const temp_ptr = this.module._malloc(output_size);
    const temp_samples =
        new Float32Array(this.module.HEAPF32.buffer, temp_ptr, output_length);

    Object.values(this.instruments).forEach(instrument => {
      instrument.process(temp_ptr, output_length, currentTime);
      for (let i = 0; i < output_length; ++i) {
        output_samples[i] += temp_samples[i];
      }
    });
    output.set(output_samples);

    this.module._free(temp_ptr);
    this.module._free(output_ptr);

    return true;
  }
}

registerProcessor('barelymusician-processor', Processor);
