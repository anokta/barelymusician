export const ControlType = {
  GAIN: 0,
  PITCH_SHIFT: 1,
  RETRIGGER: 2,
  VOICE_COUNT: 3,
  ATTACK: 4,
  DECAY: 5,
  SUSTAIN: 6,
  RELEASE: 7,
  OSC_MIX: 8,
  OSC_MODE: 9,
  OSC_NOISE_MIX: 10,
  OSC_PITCH_SHIFT: 11,
  OSC_SHAPE: 12,
  OSC_SKEW: 13,
  SLICE_MODE: 14,
  FILTER_TYPE: 15,
  FILTER_FREQUENCY: 16,
  FILTER_Q: 17,
  BIT_CRUSHER_DEPTH: 18,
  BIT_CRUSHER_RATE: 19,
};

export class Instrument {
  constructor({container, audioNode, handle, noteOnCallback, noteOffCallback}) {
    this.container = container;
    this.audioNode = audioNode;
    this.handle = handle;
    this.noteOnCallback = noteOnCallback;
    this.noteOffCallback = noteOffCallback;
    this.activeNotes = new Set();

    this._init();
    this._render();
    this._attachEvents();
  }

  _init() {
    this.audioNode.port.onmessage = async (event) => {
      if (!event.data) {
        return;
      }
      switch (event.data.type) {
        case 'instrument-on-note-on': {
          if (event.data.handle == this.handle) {
            this.noteOnCallback(event.data.pitch);
          }
        } break;
        case 'instrument-on-note-off': {
          if (event.data.handle == this.handle) {
            this.noteOffCallback(event.data.pitch);
          }
        } break;
      }
    };
  }

  _render() {
    this.container.innerHTML = `
      <div>
        <label>Osc Shape <input type="range" min="0" max="1" step="0.01" value="0.0" id="oscShape"></label>
        <span id="oscShapeValue">0.0</span>
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
    // Oscillator sliders.
    const oscShape = this.container.querySelector('#oscShape');
    const oscShapeValue = this.container.querySelector('#oscShapeValue');
    oscShape.addEventListener('input', () => {
      oscShapeValue.textContent = oscShape.value;
      this.audioNode.port.postMessage({
        type: 'instrument-set-control',
        handle: this.handle,
        typeIndex: 12,
        value: parseFloat(oscShape.value)
      });
    });

    // Piano keys.
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
    piano.addEventListener('mouseover', e => {
      if (pressedNote !== null && e.target.classList.contains('key')) {
        const note = Number(e.target.dataset.note);
        this.stopNote(pressedNote);
        this.playNote(note);
        pressedNote = note;
      }
    });
    piano.addEventListener('mouseleave', e => {
      if (pressedNote !== null) {
        this.stopNote(pressedNote);
        pressedNote = null;
      }
    });
  }

  _noteToPitch(note) {
    return note / 12;
  }

  playNote(note) {
    if (!this.handle) {
      return;
    }
    if (!this.activeNotes.has(note)) {
      this.activeNotes.add(note);
      const pitch = this._noteToPitch(note);
      this.audioNode.port.postMessage({
        type: 'instrument-set-note-on',
        handle: this.handle,
        pitch: pitch,
        gain: 1.0
      });
      const el = this.container.querySelector(`[data-note="${note}"]`);
      if (el) {
        el.classList.add('active');
      }
    }
  }

  stopNote(note) {
    if (!this.handle) {
      return;
    }
    if (this.activeNotes.has(note)) {
      this.activeNotes.delete(note);
      const pitch = this._noteToPitch(note);
      // this.instrument.setNoteOff(pitch);
      this.audioNode.port.postMessage(
          {type: 'instrument-set-note-off', handle: this.handle, pitch: pitch});
      const el = this.container.querySelector(`[data-note="${note}"]`);
      if (el) {
        el.classList.remove('active');
      }
    }
  }
}
