import {Instrument} from './instrument.js';
import {Performer} from './performer.js';

export class Engine {
  constructor({container, audioContext}) {
    this.container = container;

    this.seed = 0.0;
    this.tempo = 120.0;
    this.timestamp = 0.0;

    this._render();
    this.instrumentsContainer = document.createElement('div');
    this.instrumentsContainer.className = 'instruments';
    this.container.appendChild(this.instrumentsContainer);

    this._instruments = {};
    this._performers = {};

    this._pendingPerformers = [];

    this._metronome = null;

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
     <p></p>
    <div class="instruments"></div>
    `;
  }

  _createInstrument(handle) {
    const instrumentContainer = document.createElement('div');
    instrumentContainer.id = 'instrument-' + handle;
    this.instrumentsContainer.appendChild(instrumentContainer);

    this._instruments[handle] = new Instrument({
      container: instrumentContainer,
      audioNode: this._audioNode,
      handle: handle,
      noteOnCallback: (pitch) => {
        console.log(`[Instrument#${handle}] NoteOn(${pitch.toFixed(1)})`);
      },
      noteOffCallback: (pitch) => {
        console.log(`[Instrument#${handle}] NoteOff(${pitch.toFixed(1)})`);
      },
    });
  }

  createPerformer() {
    let resolveHandle;
    const handlePromise = new Promise(resolve => {
      resolveHandle = resolve;
    });
    const performer = new Performer({audioNode: this._audioNode, handlePromise: handlePromise});

    this._audioNode.port.postMessage({type: 'performer-create'});
    this._pendingPerformers.push({performer, resolveHandle});

    return performer;
  }

  _destroyInstrument(handle) {
    document.getElementById('instrument-' + handle).remove();
    this._audioNode.port.postMessage({type: 'instrument-destroy', handle: handle});
    delete this._instruments[handle];
  }

  _destroyPerformer(handle) {
    this._audioNode.port.postMessage({type: 'performer-destroy', handle: handle});
    delete this._performers[handle];
  }

  _initAudioNode(audioContext) {
    this._audioNode = new AudioWorkletNode(audioContext, 'barelymusician-processor');
    this._audioNode.connect(audioContext.destination);
    this._audioNode.port.onmessage = (event) => {
      if (!event.data) {
        return;
      }

      switch (event.data.type) {
        case 'init-success': {
          this._attachEvents();
          this._startUpdateLoop();
        } break;
        case 'engine-get-timestamp-response': {
          this.timestamp = event.data.timestamp;
        } break;
        case 'instrument-create-success': {
          this._createInstrument(event.data.handle);
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
        case 'task-on-process': {
          this.tasks[event.data.handle]?.processCallback(event.data.state);
        } break;
        case 'trigger-on-process': {
          this.triggers[event.data.handle]?.processCallback();
        } break;
      }
    };
  }

  _attachEvents() {
    // Instruments.
    this.container.querySelector('#createInstrumentBtn').addEventListener('click', () => {
      this._audioNode.port.postMessage({type: 'instrument-create'});
    });

    // // Performers.
    // this.container.querySelector('#createPerformerBtn').addEventListener('click', () => {
    //   this._audioNode.port.postMessage({type: 'performer-create'});
    // });

    // this.container.querySelector('#deletePerformerBtn').addEventListener('click', () => {
    //   if (this._performers.length > 0) {
    //     const performer = this._performers.pop();
    //     // TODO: Need to destroy explicitly?
    //     this._updateStatus();
    //   }
    // });

    // Transport controls.
    this._metronome = this.createPerformer();

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

    this.container.querySelector('#stopBtn').addEventListener('click', () => {
      this._metronome.stop();
      this._metronome.position = 0.0;
      Object.values(this._performers).forEach(performer => {
        performer.stop();
        performer.position = 0.0;
      });
      this.container.querySelector('#playPauseBtn').textContent = 'Play';
    });

    this.tempoSlider = this.container.querySelector('#tempoSlider');
    this.tempoValue = this.container.querySelector('#tempoValue');
    this.tempoSlider.addEventListener('input', () => {
      this.tempo = Number(this.tempoSlider.value);
      this.tempoValue.textContent = this.tempo;
      this._audioNode.port.postMessage({type: 'engine-set-tempo', tempo: this.tempo});
    });

    this.container.querySelector('#resetBtn').addEventListener('click', () => {
      for (const handle in this._instruments) {
        this._destroyInstrument(handle);
      }
      this._instruments = {};
      this._performers = {};
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
    const status = `
      Instruments: ${Object.keys(this._instruments).length} |
      Performers: ${Object.keys(this._performers).length} |
      Position: ${this._metronome.position.toFixed(1)}
    `;
    this.container.querySelector('#engineStatus').textContent = status;
  }
}
