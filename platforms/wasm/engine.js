import {CONTROLS} from './control.js';
import {Instrument} from './instrument.js';
import {Performer} from './performer.js';

const MAX_INSTRUMENT_COUNT = 10;
const MAX_PERFORMER_COUNT = 20;

export class Engine {
  /**
   * @param {!Object} options
   */
  constructor({container, audioContext, state}) {
    this.container = container;

    this.seed = 0.0;
    this._tempo = 120.0;

    this._delayTime = 0.0;
    this._delayFeedback = 0.0;

    this._render();

    this._instruments = {};
    this._performers = {};
    this._tasks = {};

    this._pendingInstruments = [];
    this._pendingPerformers = [];

    this._metronome = null;

    this._initAudioNode(audioContext, state);
  }

  /**
   * Loads engine state from a string.
   * @param {string} stateStr
   */
  loadState(stateStr) {
    const {tempoJson, delayTimeJson, delayFeedbackJson, instrumentsJson, performersJson} =
        JSON.parse(decodeURIComponent(atob(stateStr)));

    this.tempo = tempoJson;
    this.delayTime = delayTimeJson;
    this.delayFeedback = delayFeedbackJson;

    const tempInstruments = {};

    const instrumentsContainer = this.container.querySelector('.instruments');
    instrumentsJson.forEach(instrumentJson => {
      const instrumentContainer = document.createElement('div');
      instrumentContainer.className = 'instrument';
      instrumentsContainer.appendChild(instrumentContainer);

      const instrument = this.createInstrument(instrumentContainer);
      for (const controlTypeIndex in instrumentJson.controlValues) {
        instrument.setControl(controlTypeIndex, instrumentJson.controlValues[controlTypeIndex]);
      }

      tempInstruments[instrumentJson.id] = instrument;
    });

    const performersContainer = this.container.querySelector('.performers');
    performersJson.forEach(performerJson => {
      const performerContainer = document.createElement('div');
      performerContainer.className = 'performer';
      performersContainer.appendChild(performerContainer);

      const performer = this.createPerformer(performerContainer);
      performer.isLooping = true;
      performer.loopLength = performerJson.loopLength;
      performerJson.notes.forEach(
          note => performer._addNote(note.position, note.duration, note.pitch, note.gain));

      if (tempInstruments[performerJson.instrumentHandle]) {
        tempInstruments[performerJson.instrumentHandle]._withId(id => {
          performer.updateInstrumentSelect(this._instruments);
          performer._container.querySelector('#instrumentSelect').value = id;
        });
      }
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
    const instrumentsJson = Object.keys(this._instruments).map(id => {
      const instrument = this._instruments[id];
      const controlValues = {};
      const controlsContainer = instrument._container.querySelector('#controls');
      for (const controlTypeIndex in CONTROLS) {
        const controlContainer = controlsContainer.querySelector(`#control-${controlTypeIndex}`);
        if (controlContainer.querySelector('input')) {
          controlValues[controlTypeIndex] = controlContainer.querySelector('input').value;
        } else {
          controlValues[controlTypeIndex] = controlContainer.querySelector('select').value;
        }
      }
      return {id, controlValues};
    });
    const performersJson = Object.values(this._performers)
                               .filter(performer => performer !== this._metronome)
                               .map(performer => ({
                                      instrumentHandle: performer.getSelectedInstrumentHandle(),
                                      loopLength: performer.loopLength,
                                      notes: performer._notes.map(note => ({
                                                                    position: note.position,
                                                                    duration: note.duration,
                                                                    pitch: note.pitch,
                                                                    gain: note.gain,
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
    </div>
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
        <input type="range" min="30" max="480" value="${this._tempo}" step="1" id="tempoSlider">
        <span id="tempoValue">${this._tempo}</span>
      </div>
      <div class="engine-delay">
        <label for="delayTimeSlider">Delay Time</label>
        <input type="range" min="0.0" max="10.0" value="${
        this._delayTime}" step="0.1" id="delayTimeSlider">
        <span id="delayTimeValue">${this._delayTime.toFixed(1)}</span>
        <label for="delayFeedbackSlider">Feedback</label>
        <input type="range" min="0.0" max="1.0" value="${
        this._delayFeedback}" step="0.05" id="delayFeedbackSlider">
        <span id="delayFeedbackValue">${this._delayFeedback.toFixed(2)}</span>
      </div>
    <div class="engine-status"></div>
    <div class="engine-components">
      <div class="instruments"></div>
      <div class="performers"></div>
    </div>
     <p></p>
    `;
  }

  /**
   * @param {!Element} instrumentContainer
   * @return {!Instrument}
   */
  createInstrument(instrumentContainer) {
    let resolveHandle;
    const handlePromise = new Promise(resolve => {
      resolveHandle = resolve;
    });

    const instrument = new Instrument({
      container: instrumentContainer,
      audioContext: this._audioContext,
      audioNode: this._audioNode,
      handlePromise: handlePromise,
      noteOnCallback: pitch => {
        console.log(`[${instrumentContainer.id}] NoteOn(${pitch.toFixed(1)})`);
      },
      noteOffCallback: pitch => {
        console.log(`[${instrumentContainer.id}] NoteOff(${pitch.toFixed(1)})`);
      },
    });

    this._pendingInstruments.push({instrument, resolveHandle});

    return instrument;
  }

  /**
   * @param {!Element} performerContainer
   * @return {!Performer}
   */
  createPerformer(performerContainer) {
    let resolveHandle;
    const handlePromise = new Promise(resolve => {
      resolveHandle = resolve;
    });
    const performer = new Performer({
      container: performerContainer,
      audioNode: this._audioNode,
      handlePromise: handlePromise,
      instruments: this._instruments,
    });

    this._pendingPerformers.push({performer, resolveHandle});

    return performer;
  }

  get tempo() {
    return this._tempo;
  }

  get delayTime() {
    return this._delayTime;
  }

  get delayFeedback() {
    return this._delayFeedback;
  }

  set tempo(newTempo) {
    if (this._tempo === newTempo) return;

    this._tempo = newTempo;
    this.tempoSlider.value = this._tempo;
    this.tempoValue.textContent = this._tempo;
    this._audioNode.port.postMessage({type: 'engine-set-tempo', tempo: this._tempo});
  }

  set delayTime(newDelayTime) {
    if (this._delayTime === newDelayTime) return;

    this._delayTime = newDelayTime;
    this.delayTimeSlider.value = this._delayTime;
    this.delayTimeValue.textContent = this._delayTime.toFixed(1);
    this._audioNode.port.postMessage({type: 'engine-set-delay-time', delayTime: this._delayTime});
  }

  set delayFeedback(newDelayFeedback) {
    if (this._delayFeedback === newDelayFeedback) return;

    this._delayFeedback = newDelayFeedback;
    this.delayFeedbackSlider.value = this._delayFeedback;
    this.delayFeedbackValue.textContent = this._delayFeedback.toFixed(2);
    this._audioNode.port.postMessage(
        {type: 'engine-set-delay-feedback', delayFeedback: this._delayFeedback});
  }

  _initAudioNode(audioContext, state) {
    const STEREO_CHANNEL_COUNT = 2;
    this._audioContext = audioContext;
    this._audioNode = new AudioWorkletNode(this._audioContext, 'barelymusician-processor', {
      numberOfInputs: 0,
      numberOfOutputs: 1,
      outputChannelCount: [STEREO_CHANNEL_COUNT],
      channelCount: STEREO_CHANNEL_COUNT,
      channelCountMode: 'explicit',
    });
    this._audioNode.connect(this._audioContext.destination);
    this._audioNode.port.onmessage = event => {
      if (!event.data) return;

      switch (event.data.type) {
        case 'init-success': {
          this._reset();
          this._attachEvents();
          if (state) {
            this.loadState(state);
          } else {
            this.delayTime = 0.5;
            this.delayFeedback = 0.2;
          }
          this._startUpdateLoop();
        } break;
        case 'instrument-create-success': {
          const {instrument, resolveHandle} = this._pendingInstruments.shift();
          resolveHandle(event.data.id);
          this._instruments[event.data.id] = instrument;
          for (const id in this._performers) {
            this._performers[id].updateInstrumentSelect(this._instruments);
          }
        } break;
        case 'instrument-destroy-success': {
          delete this._instruments[event.data.id];
          for (const id in this._performers) {
            this._performers[id].updateInstrumentSelect(this._instruments);
          }
        } break;
        case 'instrument-on-note-on': {
          this._instruments[event.data.id]?.noteOnCallback(event.data.pitch);
        } break;
        case 'instrument-on-note-off': {
          this._instruments[event.data.id]?.noteOffCallback(event.data.pitch);
        } break;
        case 'performer-create-success': {
          const {performer, resolveHandle} = this._pendingPerformers.shift();
          resolveHandle(event.data.id);
          this._performers[event.data.id] = performer;
          this._performers[event.data.id].updateInstrumentSelect(this._instruments);
        } break;
        case 'performer-destroy-success': {
          delete this._performers[event.data.id];
        } break;
        case 'performer-get-properties-response': {
          const performer = this._performers[event.data.id];
          if (performer) {
            performer._isPlaying = event.data.isPlaying;
            performer._position = event.data.position;
          }
        } break;
        case 'task-create-success': {
          const performer = this._performers[event.data.performerHandle];
          if (performer) {
            this._tasks[event.data.id] = performer.onTaskCreateSuccess(event.data.id);
          }
        } break;
        case 'task-destroy-success': {
          delete this._tasks[event.data.id];
        } break;
        case 'task-get-properties-response': {
          const task = this._tasks[event.data.id];
          if (task) {
            task._duration = event.data.duration;
            task._isActive = event.data.isActive;
            task._position = event.data.position;
          }
        } break;
        case 'task-on-event': {
          this._tasks[event.data.id]?.eventCallback(event.data.eventType);
        } break;
      }
    };
  }

  _attachEvents() {
    // Instruments.
    const instrumentsContainer = this.container.querySelector('.instruments');
    this.container.querySelector('#createInstrumentBtn').addEventListener('click', () => {
      const instrumentContainer = document.createElement('div');
      instrumentContainer.className = 'instrument';
      instrumentsContainer.appendChild(instrumentContainer);
      this.createInstrument(instrumentContainer);
    });

    // Performers.
    const performersContainer = this.container.querySelector('.performers');
    this.container.querySelector('#createPerformerBtn').addEventListener('click', () => {
      const performerContainer = document.createElement('div');
      performerContainer.className = 'performer';
      performersContainer.appendChild(performerContainer);

      const performer = this.createPerformer(performerContainer);
      performer.isLooping = true;
      performer.position = this._metronome.position;
      if (this._metronome.isPlaying) {
        performer.start();
      }
    });

    // Transport controls.
    const playPauseButton = this.container.querySelector('#playPauseBtn');
    playPauseButton.addEventListener('click', () => {
      if (this._metronome.isPlaying) {
        Object.values(this._performers).forEach(performer => performer.stop());
        playPauseButton.textContent = ' Play ';
      } else {
        Object.values(this._performers).forEach(performer => performer.start());
        playPauseButton.textContent = 'Pause';
      }
    });

    document.addEventListener('visibilitychange', () => {
      if (document.hidden && this._metronome.isPlaying) {
        Object.values(this._performers).forEach(performer => performer.stop());
        playPauseButton.textContent = 'Play';
      }
    });

    const stopButton = this.container.querySelector('#stopBtn');
    stopButton.addEventListener('click', () => {
      Object.values(this._performers).forEach(performer => {
        performer.stop();
        performer.position = 0.0;
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

    for (const id in this._instruments) {
      this._instruments[id].destroy();
    }
    for (const id in this._performers) {
      this._performers[id].destroy();
    }
    this._instruments = {};
    this._performers = {};
    this._tasks = {};

    this._metronome = this.createPerformer();
  }

  _startUpdateLoop() {
    const update = () => {
      this._updateStatus();
      requestAnimationFrame(update);
    };
    update();
  }

  _updateStatus() {
    this._audioNode.port.postMessage({type: 'engine-update'});
    for (const performerHandle in this._performers) {
      this._audioNode.port.postMessage({type: 'performer-get-properties', id: performerHandle});
    }
    for (const taskHandle in this._tasks) {
      this._audioNode.port.postMessage({type: 'task-get-properties', id: taskHandle});
    }
    const instrumentCount = Object.keys(this._instruments).length;
    const performerCount = Math.max(Object.keys(this._performers).length - 1);
    const status = `
      Instruments: ${instrumentCount} |
      Performers: ${performerCount} |
      Position: ${this._metronome.position.toFixed(1)}
    `;
    this.container.querySelector('#createInstrumentBtn').disabled =
        (instrumentCount >= MAX_INSTRUMENT_COUNT);
    this.container.querySelector('#createPerformerBtn').disabled =
        (performerCount >= MAX_PERFORMER_COUNT);
    this.container.querySelector('.engine-status').textContent = status;
  }
}
