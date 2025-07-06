import {Engine} from './engine.js';

const onLoad = async () => {
  const audioContext = new (window.AudioContext || window.webkitAudioContext)();
  await audioContext.audioWorklet.addModule('processor.js');
  document.body.addEventListener('click', () => audioContext.resume());

  const engineContainer = document.createElement('div');
  engineContainer.className = 'engine';
  document.body.appendChild(engineContainer);

  new Engine({
    container: engineContainer,
    audioContext: audioContext,
    state: location.hash.slice(1),
  });
};

window.addEventListener('load', onLoad);
