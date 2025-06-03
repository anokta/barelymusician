import {Instrument} from './instrument.js';

const onLoad = async () => {
  // Initialize audio processor.
  const audioContext = new (window.AudioContext || window.webkitAudioContext)();
  await audioContext.audioWorklet.addModule('processor.js');
  document.body.addEventListener('click', () => audioContext.resume());

  const audioNode = new AudioWorkletNode(audioContext, 'barelymusician-processor');
  audioNode.connect(audioContext.destination);

  // Initialize the engine.
  let instruments = {};

  audioNode.port.onmessage = (event) => {
    if (!event.data) {
      return;
    }

    switch (event.data.type) {
      case 'init-success': {
        audioNode.port.postMessage({type: 'instrument-create'});
        audioNode.port.postMessage({type: 'instrument-create'});
      } break;
      case 'instrument-create-success': {
        instruments[event.data.handle] = new Instrument({
          container: document.getElementById('instrument-ui-' + Object.keys(instruments).length),
          audioNode,
          handle: event.data.handle,
          noteOnCallback: (pitch) => {
            console.log('NoteOn: ' + pitch);
          },
          noteOffCallback: (pitch) => {
            console.log('NoteOff: ' + pitch);
          },
        });
      } break;
      case 'instrument-on-note-on': {
        if (event.data.handle in instruments) {
          instruments[event.data.handle].noteOnCallback(event.data.pitch);
        }
      } break;
      case 'instrument-on-note-off': {
        if (event.data.handle in instruments) {
          instruments[event.data.handle].noteOffCallback(event.data.pitch);
        }
      } break;
    }
  };

  // audioNode.port.postMessage({type: 'instrument-create'});
  // let engine = new barelymusician.BarelyEngine(48000, 440.0);

  // const engineUI =
  //     new Engine({container: document.getElementById('engine-ui'), engine});

  // let performer = engine.createPerformer();
  // console.log(performer.isPlaying);
  // performer.start();
  // console.log(performer.isPlaying);
  // performer.stop();
  // console.log(performer.isPlaying);

  // const task = performer.createTrigger(0.25, () => {
  //   console.log('CALLBACK: ' + performer.position);
  // });

  // performer.start();
  // performer.looping = true;

  // let instrument = engine.createInstrument();

  // const instrumentUi = new Instrument(
  //     {container: document.getElementById('instrument-ui'), instrument});

  // instrument.setNoteOnCallback((pitch) => {
  //   console.log('ON: ' + pitch);
  // });
  // instrument.setNoteOffCallback((pitch) => {
  //   console.log('OFF: ' + pitch);
  // });
  // instrument.setNoteOn(0.0, 1.0, 0.0);
  // instrument.setNoteOn(2.5);
  // instrument.setAllNotesOff();
};

window.addEventListener('load', onLoad);
