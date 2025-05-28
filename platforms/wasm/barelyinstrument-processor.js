import Module from '../../build/WebAssembly/bin/barelymusician.js';

var barelymusician = null;

class BarelyInstrumentProcessor extends AudioWorkletProcessor {
  constructor() {
    super();

    this.instrumentHandle = null;
    this.port.onmessage = async (event) => {
      if (event.data && event.data.type === 'init') {
        if (!barelymusician) {
          barelymusician = await Module();
        }
        this.instrumentHandle = event.data.instrumentHandle;
      }
    };
  }

  process(inputs, outputs, parameters) {
    if (!barelymusician || !barelymusician.HEAPF32 || !this.instrumentHandle) {
      return true;
    }

    const output = outputs[0][0];
    const output_size = output.length * Float32Array.BYTES_PER_ELEMENT;

    const output_ptr = barelymusician._malloc(output_size);

    barelymusician.processBarelyInstrument(
        this.instrumentHandle, output_ptr, output.length, currentTime);

    const samples = new Float32Array(
        barelymusician.HEAPF32.buffer, output_ptr, output.length);
    output.set(samples);

    barelymusician._free(output_ptr);

    return true;
  }
}

registerProcessor('barelyinstrument-processor', BarelyInstrumentProcessor);
