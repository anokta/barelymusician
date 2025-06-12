import {Instrument} from './instrument.js';
import {Performer} from './performer.js';

export class Engine {
  constructor({container, audioContext}) {
    this.container = container;

    this.seed = 0.0;
    this.tempo = 120.0;
    this.timestamp = 0.0;

    this._render();

    this._instruments = {};
    this._performers = {};
    this._tasks = {};
    this._triggers = {};

    this._pendingInstruments = [];
    this._pendingPerformers = [];

    this._metronome = null;
    this._beat = 0;

    this._initAudioNode(audioContext);
  }

  _render() {
    this.container.innerHTML = `
    <div class="engine-controls">
      <button id="playPauseBtn">Play</button>
      <button id="stopBtn">Stop</button>
      <div class="engine-tempo">
        <label for="tempoSlider">Tempo</label>
        <input type="range" min="30" max="480" value="${this.tempo}" step="1" id="tempoSlider">
        <span id="tempoValue">${this.tempo}</span>
      </div>
    </div>
    <div class="engine-actions">
      <button id="createInstrumentBtn">Create Instrument</button>
      <button id="createPerformerBtn">Create Performer</button>
      <button id="resetBtn">Reset</button>
    </div>
    <div id="engineStatus"></div>
    <div class="engine-components">
      <div class="instruments"></div>
      <div class="performers"></div>
    </div>
     <p></p>
    `;
  }

  createInstrument(instrumentContainer) {
    let resolveHandle;
    const handlePromise = new Promise(resolve => {
      resolveHandle = resolve;
    });

    const instrument = new Instrument({
      container: instrumentContainer,
      audioNode: this._audioNode,
      handlePromise: handlePromise,
      noteOnCallback: (pitch) => {
        console.log(`[${instrumentContainer.id}] NoteOn(${pitch.toFixed(1)})`);
      },
      noteOffCallback: (pitch) => {
        console.log(`[${instrumentContainer.id}] NoteOff(${pitch.toFixed(1)})`);
      },
    });

    this._pendingInstruments.push({instrument, resolveHandle});
  }

  createPerformer(performerContainer) {
    let resolveHandle;
    const handlePromise = new Promise(resolve => {
      resolveHandle = resolve;
    });
    const performer = new Performer({
      container: performerContainer,
      audioNode: this._audioNode,
      handlePromise: handlePromise,
    });

    this._pendingPerformers.push({performer, resolveHandle});

    return performer;
  }

  _initAudioNode(audioContext) {
    this._audioNode = new AudioWorkletNode(audioContext, 'barelymusician-processor');
    this._audioNode.connect(audioContext.destination);
    this._audioNode.port.onmessage = (event) => {
      if (!event.data) return;

      switch (event.data.type) {
        case 'init-success': {
          this._attachEvents();
          this._startUpdateLoop();
        } break;
        case 'engine-get-timestamp-response': {
          this.timestamp = event.data.timestamp;
        } break;
        case 'instrument-create-success': {
          const {instrument, resolveHandle} = this._pendingInstruments.shift();
          resolveHandle(event.data.handle);
          this._instruments[event.data.handle] = instrument;
        } break;
        case 'instrument-destroy-success': {
          delete this._instruments[event.data.handle];
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
        } break;
        case 'performer-get-properties-response': {
          const performer = this._performers[event.data.handle];
          if (performer) {
            performer.isLooping = event.data.isLooping;
            performer.loopBeginPosition = event.data.loopBeginPosition;
            performer.loopLength = event.data.loopLength;
            performer.position = event.data.position;
          }
        } break;
        case 'task-create-success': {
          const performer = this._performers[event.data.performerHandle];
          if (performer) {
            this._tasks[event.data.handle] = performer.onTaskCreateSuccess(event.data.handle);
          }
        } break;
        case 'task-get-properties-response': {
          const task = this._tasks[event.data.handle];
          if (task) {
            task.duration = event.data.duration;
            task.position = event.data.position;
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
        case 'trigger-get-properties-response': {
          const trigger = this._triggers[event.data.handle];
          if (trigger) {
            trigger.position = event.data.position;
          }
        } break;
        case 'trigger-on-process': {
          this._triggers[event.data.handle]?.processCallback();
        } break;
      }
    };
  }

  _createMetronome() {
    this._metronome = this.createPerformer();
    this._metronome.isLooping = true;
    this._metronome.createTrigger(0.0, () => {
      console.log('Tick: ' + this._beat);
      ++this._beat;
    });
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
      this.createPerformer(performerContainer);
    });

    // Transport controls.
    this._createMetronome();

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
      this.tempoValue.textContent = this.tempo;
      this._audioNode.port.postMessage({type: 'engine-set-tempo', tempo: this.tempo});
    });

    // reset
    this.container.querySelector('#resetBtn').addEventListener('click', () => {
      stopButton.click();
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
      this._createMetronome();
    });
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
    for (const performerHandle in this._performers) {
      this._audioNode.port.postMessage({type: 'performer-get-properties', handle: performerHandle});
    }
    for (const taskHandle in this._tasks) {
      this._audioNode.port.postMessage({type: 'task-get-properties', handle: taskHandle});
    }
    for (const triggerHandle in this._triggers) {
      this._audioNode.port.postMessage({type: 'trigger-get-properties', handle: triggerHandle});
    }
    const status = `
      Instruments: ${Object.keys(this._instruments).length} |
      Performers: ${Math.max(Object.keys(this._performers).length - 1, 0)} |
      Position: ${(this._beat + this._metronome.position).toFixed(1)}
    `;
    this.container.querySelector('#engineStatus').textContent = status;
  }
}
