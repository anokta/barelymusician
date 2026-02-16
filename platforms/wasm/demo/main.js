import {EngineUi} from './engine-ui.js';

const DEFAULT_STATE =
    '';

/**
 * Initializes the barelymusician engine on page load.
 */
const onLoad = async () => {
  const audioContext = new (window.AudioContext || window.webkitAudioContext)();
  await audioContext.audioWorklet.addModule('../src/processor.js');
  document.body.addEventListener('click', () => audioContext.resume());

  const engineContainer = document.createElement('div');
  engineContainer.className = 'engine';
  document.body.appendChild(engineContainer);

  new EngineUi({
    container: engineContainer,
    audioContext: audioContext,
    state: (location.hash ? location.hash.slice(1) : DEFAULT_STATE),
  });
};

window.addEventListener('load', onLoad);
