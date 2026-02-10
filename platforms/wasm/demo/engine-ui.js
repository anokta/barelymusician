import {EngineControlType, INSTRUMENT_CONTROLS} from '../src/control.js';
import {Engine} from '../src/engine.js';

import {InstrumentUi} from './instrument-ui.js';
import {PerformerUi} from './performer-ui.js';

const MAX_INSTRUMENT_COUNT = 10;
const MAX_PERFORMER_COUNT = 10;

export class EngineUi {
  /**
   * @param {!Object} options
   */
  constructor({container, audioContext, state}) {
    /** @public @const {!Element} */
    this.container = container;

    /** @private {number} */
    this._tempo = 120.0;

    /** @private {number} */
    this._delayTime = 0.0;

    /** @private {number} */
    this._delayFeedback = 0.0;

    /** @private {!Map<number, !InstrumentUi>} */
    this._instruments = new Map();

    /** @private {!Map<number, !PerformerUi>} */
    this._performers = new Map();

    /** @private {?PerformerUi} */
    this._metronome = null;

    this._render();

    const onInit = () => {
      this._reset();
      this._attachEvents();

      if (state) {
        this.loadState(state);
      } else {
        this._tempo = 120.0;
        this.delayTime = 0.5;
        this.delayFeedback = 0.2;
      }

      this._startUpdateLoop();
    };

    this.engine = new Engine(audioContext, onInit);
  }

  /**
   * Loads engine state from a string.
   * @param {string} stateStr
   */
  loadState(stateStr) {
    const {tempoJson, delayTimeJson, delayFeedbackJson, instrumentsJson, performersJson} =
        JSON.parse(decodeURIComponent(atob(stateStr)));

    this.engine.setTempo(tempoJson);
    this.delayTime = delayTimeJson;
    this.delayFeedback = delayFeedbackJson;

    /** @type {!Map<number, !InstrumentUi>} */
    const tempInstruments = new Map();

    const instrumentsContainer = this.container.querySelector('.instruments');
    instrumentsJson.forEach(({id, controlValues}) => {
      const instrumentContainer = document.createElement('div');
      instrumentContainer.className = 'instrument';
      instrumentsContainer.appendChild(instrumentContainer);

      const instrumentUi = this._createInstrument(instrumentContainer);
      for (const controlTypeIndex in controlValues) {
        instrumentUi.setControl(controlTypeIndex, controlValues[controlTypeIndex]);
      }

      tempInstruments.set(id, instrumentUi);
    });

    const performersContainer = this.container.querySelector('.performers');
    performersJson.forEach(performerJson => {
      const performerContainer = document.createElement('div');
      performerContainer.className = 'performer';
      performersContainer.appendChild(performerContainer);

      const performerUi = this._createPerformer(performerContainer);
      performerUi.performer.setLooping(true);
      performerUi.performer.setLoopLength(performerJson.loopLength);
      performerUi._loopLength = performerJson.loopLength;

      performerJson.notes.forEach(note => {
        performerUi._addNote(note.position, note.duration, note.pitch, note.gain);
      });

      const instrumentUi = tempInstruments.get(performerJson.instrumentHandle);
      if (instrumentUi) {
        performerUi.updateInstrumentSelect(this._instruments);
        performerUi._container.querySelector('#instrumentSelect').value =
            instrumentUi._instrument.handle;
      }

      this._updateAllPerformerInstrumentSelects();
    });
  }

  /**
   * Saves engine state to a string.
   * @return {string}
   */
  saveState() {
    const tempoJson = this._tempo;
    const delayTimeJson = this._delayTime;
    const delayFeedbackJson = this._delayFeedback;

    const instrumentsJson = Array.from(this._instruments.entries()).map(([id, instrument]) => {
      const controlValues = {};
      const controlsContainer = instrument._container.querySelector('#controls');
      if (!controlsContainer) return {id, controlValues};

      for (const controlTypeIndex in INSTRUMENT_CONTROLS) {
        const controlContainer = controlsContainer.querySelector(`#control-${controlTypeIndex}`);
        if (!controlContainer) continue;

        const input = controlContainer.querySelector('input');
        controlValues[controlTypeIndex] = input ?
            (input.type === 'checkbox' ? Number(input.checked) : input.value) :
            controlContainer.querySelector('select')?.value;
      }

      return {id, controlValues};
    });

    const performersJson = Array.from(this._performers.values())
                               .filter(p => p !== this._metronome)
                               .map(performer => ({
                                      instrumentHandle: performer.getSelectedInstrumentHandle(),
                                      loopLength: performer.loopLength,
                                      notes: performer._notes.map(n => ({
                                                                    position: n.position,
                                                                    duration: n.duration,
                                                                    pitch: n.pitch,
                                                                    gain: n.gain,
                                                                  })),
                                    }));

    return btoa(encodeURIComponent(JSON.stringify({
      tempoJson,
      delayTimeJson,
      delayFeedbackJson,
      instrumentsJson,
      performersJson,
    })));
  }

  _render() {
    this.container.innerHTML = `
      <div class="engine-actions">
        <button id="createInstrumentBtn">Create Instrument</button>
        <button id="createPerformerBtn">Create Performer</button>
        <button id="resetBtn">Reset</button>
        <button id="saveBtn">Save</button>
      </div>

      <div class="engine-controls">
        <button id="playPauseBtn">Play</button>
        <button id="stopBtn">Stop</button>

        <div class="engine-tempo">
          <label for="tempoSlider">Tempo</label>
          <input type="range" min="30" max="480" value="120" step="1" id="tempoSlider">
          <span id="tempoValue">120</span>
        </div>

        <div class="engine-delay">
          <label for="delayTimeSlider">Delay Time</label>
          <input type="range" min="0.0" max="10.0" value="${this._delayTime}"
                 step="0.1" id="delayTimeSlider">
          <span id="delayTimeValue">${this._delayTime.toFixed(1)}</span>

          <label for="delayFeedbackSlider">Feedback</label>
          <input type="range" min="0.0" max="1.0" value="${this._delayFeedback}"
                 step="0.05" id="delayFeedbackSlider">
          <span id="delayFeedbackValue">${this._delayFeedback.toFixed(2)}</span>
        </div>
      </div>

      <div class="engine-status"></div>

      <div class="engine-components">
        <div class="instruments"></div>
        <div class="performers"></div>
      </div>
    `;
  }

  _updateAllPerformerInstrumentSelects() {
    for (const performerUi of this._performers.values()) {
      performerUi.updateInstrumentSelect(this._instruments);
    }
  }

  _createInstrument(container) {
    const instrument = this.engine.createInstrument();
    const instrumentUi = new InstrumentUi({
      container,
      instrument,
      destroyCallback: () => {
        this._instruments.delete(instrument.handle);
        this._updateAllPerformerInstrumentSelects();
      },
    });

    this._instruments.set(instrument.handle, instrumentUi);
    this._updateAllPerformerInstrumentSelects();

    return instrumentUi;
  }

  _createPerformer(container = null) {
    const performer = this.engine.createPerformer();
    const performerUi = new PerformerUi({
      container,
      engine: this.engine,
      performer,
      instruments: this._instruments,
      destroyCallback: () => {
        this._performers.delete(performer.handle);
      },
    });

    this._performers.set(performer.handle, performerUi);
    this._updateAllPerformerInstrumentSelects();

    return performerUi;
  }

  set tempo(newTempo) {
    if (this._tempo === newTempo) return;

    this.tempoSlider.value = newTempo;
    this.tempoValue.textContent = newTempo;
    this._tempo = newTempo;
    this.engine.setTempo(this._tempo);
  }

  set delayTime(newDelayTime) {
    if (this._delayTime === newDelayTime) return;

    this._delayTime = newDelayTime;
    this.delayTimeSlider.value = this._delayTime;
    this.delayTimeValue.textContent = this._delayTime.toFixed(1);
    this.engine.setControl(EngineControlType.DELAY_TIME, newDelayTime);
  }

  set delayFeedback(newDelayFeedback) {
    if (this._delayFeedback === newDelayFeedback) return;

    this._delayFeedback = newDelayFeedback;
    this.delayFeedbackSlider.value = this._delayFeedback;
    this.delayFeedbackValue.textContent = this._delayFeedback.toFixed(2);
    this.engine.setControl(EngineControlType.DELAY_FEEDBACK, newDelayFeedback);
  }

  get delayTime() {
    return this._delayTime;
  }

  get delayFeedback() {
    return this._delayFeedback;
  }

  _attachEvents() {
    // Instruments.
    const instrumentsContainer = this.container.querySelector('.instruments');
    this.container.querySelector('#createInstrumentBtn').addEventListener('click', () => {
      const instrumentContainer = document.createElement('div');
      instrumentContainer.className = 'instrument';
      instrumentsContainer.appendChild(instrumentContainer);
      this._createInstrument(instrumentContainer);
    });

    // Performers.
    const performersContainer = this.container.querySelector('.performers');
    this.container.querySelector('#createPerformerBtn').addEventListener('click', () => {
      const performerContainer = document.createElement('div');
      performerContainer.className = 'performer';
      performersContainer.appendChild(performerContainer);

      const performerUi = this._createPerformer(performerContainer);
      performerUi.performer.setLooping(true);
      performerUi.performer.setPosition(this._metronome.performer.position);  // TODO
      if (this._metronome.isPlaying) {
        performerUi.start();
      }
    });

    // Transport controls.
    const playPauseButton = this.container.querySelector('#playPauseBtn');
    playPauseButton.addEventListener('click', () => {
      if (this._metronome.isPlaying) {
        this._performers.values().forEach(performerUi => performerUi.stop());
        playPauseButton.textContent = ' Play ';
      } else {
        this._performers.values().forEach(performerUi => performerUi.start());
        playPauseButton.textContent = 'Pause';
      }
    });

    document.addEventListener('visibilitychange', () => {
      if (document.hidden && this._metronome.isPlaying) {
        this._performers.values().forEach(performerUi => performerUi.stop());
        playPauseButton.textContent = 'Play';
      }
    });

    const stopButton = this.container.querySelector('#stopBtn');
    stopButton.addEventListener('click', () => {
      this._performers.values().forEach(performerUi => {
        performerUi.stop();
        performerUi.performer.setPosition(0.0);
      });
      playPauseButton.textContent = 'Play';
    });

    this.tempoSlider = this.container.querySelector('#tempoSlider');
    this.tempoValue = this.container.querySelector('#tempoValue');
    this.tempoSlider.addEventListener('input', () => {
      this.tempo = Number(this.tempoSlider.value);
    });

    this.delayTimeSlider = this.container.querySelector('#delayTimeSlider');
    this.delayTimeValue = this.container.querySelector('#delayTimeValue');
    this.delayTimeSlider.addEventListener('input', () => {
      this.delayTime = Number(this.delayTimeSlider.value);
    });
    this.delayFeedbackSlider = this.container.querySelector('#delayFeedbackSlider');
    this.delayFeedbackValue = this.container.querySelector('#delayFeedbackValue');
    this.delayFeedbackSlider.addEventListener('input', () => {
      this.delayFeedback = Number(this.delayFeedbackSlider.value);
    });

    // Reset
    this.container.querySelector('#resetBtn').addEventListener('click', () => {
      stopButton.click();
      this._reset();
    });

    // Save
    this.container.querySelector('#saveBtn').addEventListener('click', () => {
      const stateStr = this.saveState();
      const url = `${location.origin}${location.pathname}#${stateStr}`;
      navigator.clipboard.writeText(url);
      alert('Link copied to clipboard!');
    });
  }

  _reset() {
    this._metronome = null;

    for (const performer of this._performers.values()) {
      performer.destroy();
    }
    this._performers.clear();
    for (const instrument of this._instruments.values()) {
      instrument.destroy();
    }
    this._instruments.clear();

    this._metronome = this._createPerformer();
  }

  _startUpdateLoop() {
    const update = () => {
      this._updateStatus();
      requestAnimationFrame(update);
    };
    update();
  }

  _updateStatus() {
    this.engine.update();
    const instrumentCount = this._instruments.size;
    const performerCount = Math.max(this._performers.size - 1, 0);
    const status = `
      Instruments: ${instrumentCount} |
      Performers: ${performerCount} |
      Position: ${this._metronome.performer.position.toFixed(1)}
    `;
    this.container.querySelector('#createInstrumentBtn').disabled =
        (instrumentCount >= MAX_INSTRUMENT_COUNT);
    this.container.querySelector('#createPerformerBtn').disabled =
        (performerCount >= MAX_PERFORMER_COUNT);
    this.container.querySelector('.engine-status').textContent = status;
  }
}
