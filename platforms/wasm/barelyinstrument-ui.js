class BarelyInstrumentUI {
  constructor({container, instrument}) {
    this.container = container;
    this.instrument = instrument;
    this.activeNotes = new Set();
    this.baseMidi = 60;  // C4

    this._render();
    this._attachEvents();
  }

  noteToPitch(midi) {
    return (midi - 60) / 12;
  }

  _render() {
    this.container.innerHTML = `
      <div>
        <label>Osc Mix: <input type="range" min="0" max="1" step="0.01" value="0.5" id="oscMix"></label>
        <span id="oscMixValue">0.5</span>
      </div>
      <div>
        <label>Osc Freq: <input type="range" min="20" max="2000" step="1" value="440" id="oscFreq"></label>
        <span id="oscFreqValue">440</span>
      </div>
      <div id="piano" style="margin:2em 0;user-select:none;">
        <div style="position:relative;width:336px;height:120px;">
          <div class="white key" data-note="0" style="left:0px;"></div>
          <div class="white key" data-note="2" style="left:48px;"></div>
          <div class="white key" data-note="4" style="left:96px;"></div>
          <div class="white key" data-note="5" style="left:144px;"></div>
          <div class="white key" data-note="7" style="left:192px;"></div>
          <div class="white key" data-note="9" style="left:240px;"></div>
          <div class="white key" data-note="11" style="left:288px;"></div>
          <div class="black key" data-note="1" style="left:36px;"></div>
          <div class="black key" data-note="3" style="left:84px;"></div>
          <div class="black key" data-note="6" style="left:180px;"></div>
          <div class="black key" data-note="8" style="left:228px;"></div>
          <div class="black key" data-note="10" style="left:276px;"></div>
        </div>
      </div>
    `;
  }

  _attachEvents() {
    // Oscillator sliders
    // const oscMix = this.container.querySelector('#oscMix');
    // const oscMixValue = this.container.querySelector('#oscMixValue');
    // oscMix.addEventListener('input', () => {
    //   oscMixValue.textContent = oscMix.value;
    //   this.instrument.setControl(
    //       0, parseFloat(oscMix.value));  // 0 = OscMix, adjust as needed
    // });

    // const oscFreq = this.container.querySelector('#oscFreq');
    // const oscFreqValue = this.container.querySelector('#oscFreqValue');
    // oscFreq.addEventListener('input', () => {
    //   oscFreqValue.textContent = oscFreq.value;
    //   this.instrument.setControl(
    //       1, parseFloat(oscFreq.value));  // 1 = OscFreq, adjust as needed
    // });

    // Piano keys
    const piano = this.container.querySelector('#piano');
    let pressedNote = null;
    piano.addEventListener('mousedown', e => {
      if (e.target.classList.contains('key')) {
        const note = Number(e.target.dataset.note);
        this.playNote(note);
        pressedNote = note;
      }
    });
    document.addEventListener('mouseup', e => {
      if (pressedNote !== null) {
        this.stopNote(pressedNote);
        pressedNote = null;
      }
    });
    piano.addEventListener('mouseleave', e => {
      if (pressedNote !== null) {
        this.stopNote(pressedNote);
        pressedNote = null;
      }
    });

    // Touch support
    let activeTouchNotes = new Map();
    piano.addEventListener('touchstart', e => {
      for (const t of e.changedTouches) {
        const el = document.elementFromPoint(t.clientX, t.clientY);
        if (el && el.classList.contains('key')) {
          const note = Number(el.dataset.note);
          this.playNote(note);
          activeTouchNotes.set(t.identifier, note);
        }
      }
      e.preventDefault();
    }, {passive: false});
    piano.addEventListener('touchend', e => {
      for (const t of e.changedTouches) {
        const note = activeTouchNotes.get(t.identifier);
        if (note !== undefined) {
          this.stopNote(note);
          activeTouchNotes.delete(t.identifier);
        }
      }
      e.preventDefault();
    }, {passive: false});
  }

  playNote(note) {
    if (!this.activeNotes.has(note)) {
      this.activeNotes.add(note);
      const pitch = this.noteToPitch(this.baseMidi + note);
      this.instrument.setNoteOn(pitch);
      const el = this.container.querySelector(`[data-note="${note}"]`);
      if (el) el.classList.add('active');
    }
  }

  stopNote(note) {
    if (this.activeNotes.has(note)) {
      this.activeNotes.delete(note);
      const pitch = this.noteToPitch(this.baseMidi + note);
      this.instrument.setNoteOff(pitch);
      const el = this.container.querySelector(`[data-note="${note}"]`);
      if (el) el.classList.remove('active');
    }
  }
}

export default BarelyInstrumentUI;
