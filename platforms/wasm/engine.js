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

    this.instruments = {};
    this.performers = {};

    // TODO(#164): Testonly fields for now, to be cleaned up later.
    this.metronome = null;
    this.position = 0.0;

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

    this.instruments[handle] = new Instrument({
      container: instrumentContainer,
      audioNode: this.audioNode,
      handle: handle,
      noteOnCallback: (pitch) => {
        console.log(`[Instrument#${handle}] NoteOn(${pitch.toFixed(1)})`);
      },
      noteOffCallback: (pitch) => {
        console.log(`[Instrument#${handle}] NoteOff(${pitch.toFixed(1)})`);
      },
    });
  }

  _createPerformer(handle) {
    if (!this.metronome) {
      this.metronome = new Performer({audioNode: this.audioNode, handle: handle});
      // TODO(#164): Make this work.
      // this.metronomeTask = this.metronome.createTrigger(0.0, () => {
      //   console.log('TICK');
      // });
      return;
    }
    this.performers[handle] = new Performer({audioNode: this.audioNode, handle: handle});
  }

  _destroyInstrument(handle) {
    document.getElementById('instrument-' + handle).remove();
    this.audioNode.port.postMessage({type: 'instrument-destroy', handle: handle});
    delete this.instruments[handle];
  }

  _destroyPerformer(handle) {
    this.audioNode.port.postMessage({type: 'performer-destroy', handle: handle});
    delete this.performers[handle];
  }

  _initAudioNode(audioContext) {
    this.audioNode = new AudioWorkletNode(audioContext, 'barelymusician-processor');
    this.audioNode.connect(audioContext.destination);
    this.audioNode.port.onmessage = (event) => {
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
          if (event.data.handle in this.instruments) {
            this.instruments[event.data.handle].noteOnCallback(event.data.pitch);
          }
        } break;
        case 'instrument-on-note-off': {
          if (event.data.handle in this.instruments) {
            this.instruments[event.data.handle].noteOffCallback(event.data.pitch);
          }
        } break;
        case 'performer-create-success': {
          this._createPerformer(event.data.handle);
        } break;
        case 'performer-get-position-response': {
          this.position = event.data.position;
        } break;
      }
    };
  }

  _attachEvents() {
    // Instruments.
    this.container.querySelector('#createInstrumentBtn').addEventListener('click', () => {
      this.audioNode.port.postMessage({type: 'instrument-create'});
    });

    // // Performers.
    // this.container.querySelector('#createPerformerBtn').addEventListener('click', () => {
    //   this.audioNode.port.postMessage({type: 'performer-create'});
    // });

    // this.container.querySelector('#deletePerformerBtn').addEventListener('click', () => {
    //   if (this.performers.length > 0) {
    //     const performer = this.performers.pop();
    //     // TODO: Need to destroy explicitly?
    //     this._updateStatus();
    //   }
    // });

    // Transport controls.
    this.audioNode.port.postMessage({type: 'performer-create'});

    const playPauseButton = this.container.querySelector('#playPauseBtn');
    playPauseButton.addEventListener('click', () => {
      if (this.metronome.isPlaying) {
        this.metronome.stop();
        Object.values(this.performers).forEach(performer => performer.stop());
        playPauseButton.textContent = ' Play ';
      } else {
        this.metronome.start();
        Object.values(this.performers).forEach(performer => performer.start());
        playPauseButton.textContent = 'Pause';
      }
    });

    this.container.querySelector('#stopBtn').addEventListener('click', () => {
      this.metronome.stop();
      this.metronome.position = 0.0;
      Object.values(this.performers).forEach(performer => {
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
      this.audioNode.port.postMessage({type: 'engine-set-tempo', tempo: this.tempo});
    });

    this.container.querySelector('#resetBtn').addEventListener('click', () => {
      for (const handle in this.instruments) {
        this._destroyInstrument(handle);
      }
      this.instruments = {};
      this.performers = {};
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
    this.audioNode.port.postMessage({type: 'engine-get-timestamp'});
    if (this.metronome) {
      this.audioNode.port.postMessage(
          {type: 'performer-get-position', handle: this.metronome.handle});
    }
    const status = `
      Instruments: ${Object.keys(this.instruments).length} |
      Performers: ${Object.keys(this.performers).length} |
      Position: ${this.position.toFixed(1)}
    `;
    this.container.querySelector('#engineStatus').textContent = status;
  }
}
