import Module from '../../build/WebAssembly/bin/barelymusician.js';

import BarelyEngineUI from './barelyengine-ui.js';
import BarelyInstrumentUI from './barelyinstrument-ui.js';

const onLoad = async () => {
  const barelymusician = await Module();

  let engine = new barelymusician.BarelyEngine(48000, 440.0);

  const engineUI = new BarelyEngineUI(
      {container: document.getElementById('engine-ui'), engine});

  let performer = engine.createPerformer();
  console.log(performer.isPlaying);
  performer.start();
  console.log(performer.isPlaying);
  performer.stop();
  console.log(performer.isPlaying);

  const task = performer.createTrigger(0.25, () => {
    console.log('CALLBACK: ' + performer.position);
  });

  performer.start();
  performer.looping = true;

  let instrument = engine.createInstrument();

  const instrumentUi = new BarelyInstrumentUI(
      {container: document.getElementById('instrument-ui'), instrument});

  instrument.setNoteOnCallback((pitch) => {
    console.log('ON: ' + pitch);
  });
  instrument.setNoteOffCallback((pitch) => {
    console.log('OFF: ' + pitch);
  });
  instrument.setNoteOn(0.0, 1.0, 0.0);
  instrument.setNoteOn(2.5);
  instrument.setAllNotesOff();

  const audioContext = new (window.AudioContext || window.webkitAudioContext)();
  await audioContext.audioWorklet.addModule('barelyinstrument-processor.js');
  document.body.addEventListener('click', () => audioContext.resume());

  const node = new AudioWorkletNode(audioContext, 'barelyinstrument-processor');
  node.port.postMessage(
      {type: 'init', instrumentHandle: instrument.getHandle()});
  node.connect(audioContext.destination);
};

window.addEventListener('load', onLoad);
