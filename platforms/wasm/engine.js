export class Engine {
  /**
   * @param {Object} options
   * @param {HTMLElement} options.container - Where to render the UI
   * @param {object} options.engine - The BarelyEngine instance
   * @param {number} [options.latency=0.2] - Latency in seconds
   */
  constructor({container, engine, latency = 0.2}) {
    this.container = container;
    this.engine = engine;
    this.latency = latency;

    this.instruments = [];
    this.performers = [];

    this.metronome = null;

    this._render();
    this._attachEvents();
    this._startUpdateLoop();
  }

  _render() {
    this.container.innerHTML = `
      <div>
        <button id="playPauseBtn">Play</button>
        <button id="stopBtn">Stop</button>
        <label>Tempo: <input type="range" min="30" max="480" value="${
        this.engine.tempo}" step="1" id="tempoSlider"></label>
        <span id="tempoValue">${this.engine.tempo}</span>
      </div>
      <div style="margin-top:1em;">
        <button id="createInstrumentBtn">Create Instrument</button>
        <button id="deleteInstrumentBtn">Delete Last Instrument</button>
        <button id="createPerformerBtn">Create Performer</button>
        <button id="deletePerformerBtn">Delete Last Performer</button>
      </div>
      <div id="engineStatus" style="margin-top:1em;font-size:0.9em;color:#666;"></div>
    `;
  }

  _attachEvents() {
    // Instruments.
    this.container.querySelector('#createInstrumentBtn').addEventListener('click', () => {
      const instrument = this.engine.createInstrument();
      this.instruments.push(instrument);
      this._updateStatus();
    });

    this.container.querySelector('#deleteInstrumentBtn').addEventListener('click', () => {
      if (this.instruments.length > 0) {
        const instrument = this.instruments.pop();
        // TODO: Need to destroy explicitly?
        this._updateStatus();
      }
    });

    // Performers.
    this.container.querySelector('#createPerformerBtn').addEventListener('click', () => {
      const performer = this.engine.createPerformer();
      this.performers.push(performer);
      this._updateStatus();
    });

    this.container.querySelector('#deletePerformerBtn').addEventListener('click', () => {
      if (this.performers.length > 0) {
        const performer = this.performers.pop();
        // TODO: Need to destroy explicitly?
        this._updateStatus();
      }
    });

    // Transport controls.
    this.metronome = this.engine.createPerformer();

    this.container.querySelector('#playPauseBtn').addEventListener('click', () => {
      if (this.metronome.isPlaying) {
        this.metronome.stop();
        this.performers.forEach(performer => performer.stop());
        this.container.querySelector('#playPauseBtn').textContent = 'Play';
      } else {
        this.metronome.start();
        this.performers.forEach(performer => performer.start());
        this.container.querySelector('#playPauseBtn').textContent = 'Pause';
      }
    });

    this.container.querySelector('#stopBtn').addEventListener('click', () => {
      this.metronome.stop();
      this.metronome.position = 0.0;
      this.performers.forEach(performer => {
        performer.stop();
        performer.position = 0.0;
      });
      this.container.querySelector('#playPauseBtn').textContent = 'Play';
    });

    this.tempoSlider = this.container.querySelector('#tempoSlider');
    this.tempoValue = this.container.querySelector('#tempoValue');
    this.tempoSlider.addEventListener('input', () => {
      this.engine.tempo = Number(this.tempoSlider.value);
      this.tempoValue.textContent = this.engine.tempo;
    });
  }

  _startUpdateLoop() {
    const update = () => {
      const toSeconds = 0.001;
      const timestamp = performance.now() * toSeconds + this.latency;
      this.engine.update(timestamp);
      this._updateStatus();
      requestAnimationFrame(update);
    };
    update();
  }

  _updateStatus() {
    const status = `
      Instruments: ${this.instruments.length} |
      Performers: ${this.performers.length} |
      Position: ${this.metronome.position.toFixed(1)} |
      Timestamp: ${this.engine.timestamp.toFixed(1)}
    `;
    this.container.querySelector('#engineStatus').textContent = status;
  }
}
