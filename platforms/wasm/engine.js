import {CONTROLS} from './control.js';
import {Instrument} from './instrument.js';
import {Performer} from './performer.js';

export class Engine {
  /**
   * @param {!Object} options
   */
  constructor({container, audioContext, state}) {
    this.container = container;

    this.seed = 0.0;
    this._tempo = 120.0;
    this.timestamp = 0.0;

    this._render();

    this._instruments = {};
    this._performers = {};
    this._tasks = {};
    this._triggers = {};

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
    const {tempoJson, instrumentsJson, performersJson} =
        JSON.parse(decodeURIComponent(atob(stateStr)));

    this.tempo = tempoJson;

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

      tempInstruments[instrumentJson.handle] = instrument;
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
        tempInstruments[performerJson.instrumentHandle]._withHandle(handle => {
          performer.updateInstrumentSelect(this._instruments);
          performer._container.querySelector('#instrumentSelect').value = handle;
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
    const instrumentsJson = Object.keys(this._instruments).map(handle => {
      const instrument = this._instruments[handle];
      const controlValues = {};
      const controlsContainer = instrument._container.querySelector('#controls');
      for (const controlTypeIndex in CONTROLS) {
        const controlInput =
            controlsContainer.querySelector(`#control-${controlTypeIndex}`).querySelector('input');
        controlValues[controlTypeIndex] = controlInput.value;
      }
      return {handle, controlValues};
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
    return btoa(encodeURIComponent(JSON.stringify({tempoJson, instrumentsJson, performersJson})));
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

  set tempo(newTempo) {
    if (this._tempo === newTempo) return;

    this._tempo = newTempo;
    this.tempoSlider.value = this._tempo;
    this.tempoValue.textContent = this._tempo;
    this._audioNode.port.postMessage({type: 'engine-set-tempo', tempo: this._tempo});
  }

  _initAudioNode(audioContext, state) {
    this._audioNode = new AudioWorkletNode(audioContext, 'barelymusician-processor');
    this._audioNode.connect(audioContext.destination);
    this._audioNode.port.onmessage = event => {
      if (!event.data) return;

      switch (event.data.type) {
        case 'init-success': {
          this._reset();
          this._attachEvents();
          if (state) {
            this.loadState(state);
          }
          this._startUpdateLoop();
        } break;
        case 'engine-get-timestamp-response': {
          this.timestamp = event.data.timestamp;
        } break;
        case 'instrument-create-success': {
          const {instrument, resolveHandle} = this._pendingInstruments.shift();
          resolveHandle(event.data.handle);
          this._instruments[event.data.handle] = instrument;
          for (const handle in this._performers) {
            this._performers[handle].updateInstrumentSelect(this._instruments);
          }
        } break;
        case 'instrument-destroy-success': {
          delete this._instruments[event.data.handle];
          for (const handle in this._performers) {
            this._performers[handle].updateInstrumentSelect(this._instruments);
          }
        } break;
        case 'instrument-on-note-on': {
          this._instruments[event.data.handle]?.noteOnCallback(event.data.pitch);
        } break;
        case 'instrument-on-note-off': {
          this._instruments[event.data.handle]?.noteOffCallback(event.data.pitch);
        } break;
        case 'performer-create-success': {
          const {performer, resolveHandle} = this._pendingPerformers.shift();
          resolveHandle(event.data.handle);
          this._performers[event.data.handle] = performer;
          this._performers[event.data.handle].updateInstrumentSelect(this._instruments);
        } break;
        case 'performer-destroy-success': {
          delete this._performers[event.data.handle];
        } break;
        // TODO(#164): Is this needed?
        case 'performer-get-properties-response': {
          const performer = this._performers[event.data.handle];
          if (performer && performer === this._metronome) {
            performer._isPlaying = event.data.isPlaying;
            performer._position = event.data.position;
          }
        } break;
        case 'task-create-success': {
          const performer = this._performers[event.data.performerHandle];
          if (performer) {
            this._tasks[event.data.handle] = performer.onTaskCreateSuccess(event.data.handle);
          }
        } break;
        case 'task-destroy-success': {
          delete this._tasks[event.data.handle];
        } break;
        case 'task-get-properties-response': {
          const task = this._tasks[event.data.handle];
          if (task) {
            task._duration = event.data.duration;
            task._isActive = event.data.isActive;
            task._position = event.data.position;
          }
        } break;
        case 'task-on-process': {
          this._tasks[event.data.handle]?.processCallback(event.data.state);
        } break;
        case 'trigger-create-success': {
          const performer = this._performers[event.data.performerHandle];
          if (performer) {
            this._triggers[event.data.handle] = performer.onTriggerCreateSuccess(event.data.handle);
          }
        } break;
        case 'trigger-destroy-success': {
          delete this._triggers[event.data.handle];
        } break;
        // TODO(#164): Is this needed?
        // case 'trigger-get-properties-response': {
        //   const trigger = this._triggers[event.data.handle];
        //   if (trigger) {
        //     trigger.position = event.data.position;
        //   }
        // } break;
        case 'trigger-on-process': {
          this._triggers[event.data.handle]?.processCallback();
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
    });

    // Transport controls.
    const playPauseButton = this.container.querySelector('#playPauseBtn');
    playPauseButton.addEventListener('click', () => {
      if (this._metronome.isPlaying) {
        this._metronome.stop();
        Object.values(this._performers).forEach(performer => performer.stop());
        playPauseButton.textContent = ' Play ';
      } else {
        this._metronome.start();
        Object.values(this._performers).forEach(performer => performer.start());
        playPauseButton.textContent = 'Pause';
      }
    });

    const stopButton = this.container.querySelector('#stopBtn');
    stopButton.addEventListener('click', () => {
      this._metronome.stop();
      this._metronome.position = 0.0;
      this._beat = 0;
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
    for (const handle in this._instruments) {
      this._instruments[handle].destroy();
    }
    for (const handle in this._performers) {
      this._performers[handle].destroy();
    }
    this._instruments = {};
    this._performers = {};
    this._tasks = {};
    this._triggers = {};

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
    this._audioNode.port.postMessage({type: 'engine-get-timestamp'});
    // TODO(#164): Is this needed?
    for (const performerHandle in this._performers) {
      this._audioNode.port.postMessage({type: 'performer-get-properties', handle: performerHandle});
    }
    for (const taskHandle in this._tasks) {
      this._audioNode.port.postMessage({type: 'task-get-properties', handle: taskHandle});
    }
    // for (const triggerHandle in this._triggers) {
    //   this._audioNode.port.postMessage({type: 'trigger-get-properties', handle: triggerHandle});
    // }
    const status = `
      Instruments: ${Object.keys(this._instruments).length} |
      Performers: ${Math.max(Object.keys(this._performers).length - 1, 0)} |
      Position: ${this._metronome.position.toFixed(1)}
    `;
    this.container.querySelector('.engine-status').textContent = status;
  }
}
