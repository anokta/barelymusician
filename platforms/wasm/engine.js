import {Instrument} from './instrument.js';

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
    // this.metronome = null;

    this._initAudioNode(audioContext);
  }

  _render() {
    this.container.innerHTML = `
      <div>
        <button id="playPauseBtn">Play</button>
        <button id="stopBtn">Stop</button>
        <label>Tempo: <input type="range" min="30" max="480" value="${
        this.tempo}" step="1" id="tempoSlider"></label>
        <span id="tempoValue">${this.tempo}</span>
      </div>
      <div style="margin-top:1em;">
        <button id="createInstrumentBtn">Create Instrument</button>
        <button id="createPerformerBtn">Create Performer</button>
        <button id="resetBtn">Reset</button>
      </div>
      <div id="engineStatus" style="margin-top:1em;font-size:0.9em;color:#666;"></div>
      <p></p>
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
        console.log('NoteOn: ' + pitch);
      },
      noteOffCallback: (pitch) => {
        console.log('NoteOff: ' + pitch);
      },
    });
  }

  _destroyInstrument(handle) {
    document.getElementById('instrument-' + handle).remove();
    this.audioNode.port.postMessage({type: 'instrument-destroy', handle: handle});

    delete this.instruments[handle];
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
      }
    };
  }

  _attachEvents() {
    // Instruments.
    this.container.querySelector('#createInstrumentBtn').addEventListener('click', () => {
      this.audioNode.port.postMessage({type: 'instrument-create'});
    });

    // this.container.querySelector('#deleteInstrumentBtn').addEventListener('click', () => {
    //   const lastInstrumentIndex = Object.keys(this.instruments).length - 1;
    //   this.audioNode.port.postMessage(
    //       {type: 'instrument-destroy', handle: this.instruments[lastInstrumentIndex].handle});
    //   this._destroyInstrumentContainer(lastInstrumentIndex);
    // });

    // // Performers.
    // this.container.querySelector('#createPerformerBtn').addEventListener('click', () => {
    //   const performer = this.engine.createPerformer();
    //   this.performers.push(performer);
    //   this._updateStatus();
    // });

    // this.container.querySelector('#deletePerformerBtn').addEventListener('click', () => {
    //   if (this.performers.length > 0) {
    //     const performer = this.performers.pop();
    //     // TODO: Need to destroy explicitly?
    //     this._updateStatus();
    //   }
    // });

    // // Transport controls.
    // this.metronome = this.engine.createPerformer();

    // this.container.querySelector('#playPauseBtn').addEventListener('click', () => {
    //   if (this.metronome.isPlaying) {
    //     this.metronome.stop();
    //     this.performers.forEach(performer => performer.stop());
    //     this.container.querySelector('#playPauseBtn').textContent = 'Play';
    //   } else {
    //     this.metronome.start();
    //     this.performers.forEach(performer => performer.start());
    //     this.container.querySelector('#playPauseBtn').textContent = 'Pause';
    //   }
    // });

    // this.container.querySelector('#stopBtn').addEventListener('click', () => {
    //   this.metronome.stop();
    //   this.metronome.position = 0.0;
    //   this.performers.forEach(performer => {
    //     performer.stop();
    //     performer.position = 0.0;
    //   });
    //   this.container.querySelector('#playPauseBtn').textContent = 'Play';
    // });

    this.tempoSlider = this.container.querySelector('#tempoSlider');
    this.tempoValue = this.container.querySelector('#tempoValue');
    this.tempoSlider.addEventListener('input', () => {
      this.tempo = Number(this.tempoSlider.value);
      this.tempoValue.textContent = this.tempo;
      this.audioNode.port.postMessage({type: 'engine-set-tempo', tempo: this.tempo});
    });

    this.container.querySelector('#resetBtn').addEventListener('click', () => {
      for (let handle in this.instruments) {
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
    const status = `
      Instruments: ${Object.keys(this.instruments).length} |
      Performers: ${Object.keys(this.performers).length} |
      Timestamp: ${this.timestamp.toFixed(1)}
    `;
    this.container.querySelector('#engineStatus').textContent = status;
  }
}
