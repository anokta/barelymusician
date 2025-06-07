import {CONTROLS} from './control.js'

export class Instrument {
  constructor({container, audioNode, handle, noteOnCallback, noteOffCallback}) {
    this.container = container;
    this.audioNode = audioNode;
    this.handle = handle;
    this.noteOnCallback = noteOnCallback;
    this.noteOffCallback = noteOffCallback;

    if (this.container) {
      this._render();
      this._attachEvents();
    }
  }

  _render() {
    this.container.innerHTML = `
      <div id="controls"></div>
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

  _createControlContainer(controlTypeIndex, parentContainer) {
    if (!(controlTypeIndex in CONTROLS)) return;
    const control = CONTROLS[controlTypeIndex];

    // container
    const controlContainer = document.createElement('div');
    controlContainer.id = `control-${controlTypeIndex}`;
    controlContainer.classList.add('control-container');
    parentContainer.appendChild(controlContainer);

    // label
    const controlLabel = document.createElement('label');
    controlLabel.textContent = control.name;
    controlLabel.htmlFor = `input-${controlTypeIndex}`;
    controlContainer.appendChild(controlLabel);

    // input
    const controlInput = document.createElement('input');
    controlInput.id = `input-${controlTypeIndex}`;
    switch (control.valueType) {
      case 'bool': {
        controlInput.type = 'checkbox';
        controlInput.checked = control.defaultValue;
      } break;
      case 'int': {
        controlInput.type = 'range';
        controlInput.min = control.minValue;
        controlInput.max = control.maxValue;
        controlInput.step = 1;
        controlInput.value = control.defaultValue;
      } break;
      case 'float': {
        controlInput.type = 'range';
        controlInput.min = control.minValue;
        controlInput.max = control.maxValue;
        controlInput.step = 0.01 * (control.maxValue - control.minValue);
        controlInput.value = control.defaultValue;
      } break;
      default:
        console.error('Invalid control value type: ' + control.valueType);
        controlContainer.remove();
        return;
    }
    controlContainer.appendChild(controlInput);

    if (control.valueType == 'bool') {
      controlInput.addEventListener(
          'change', (e) => this.setControl(controlTypeIndex, e.target.checked ? 1.0 : 0.0));
    } else {
      const controlSpan = document.createElement('span');
      controlSpan.id = `span-${controlTypeIndex}`;
      controlSpan.textContent = control.defaultValue.toPrecision(2);
      controlContainer.appendChild(controlSpan);

      controlInput.addEventListener('input', (e) => {
        controlSpan.textContent = controlInput.value;
        this.setControl(
            controlTypeIndex,
            (control.valueType == 'int') ? parseInt(e.target.value) : parseFloat(e.target.value));
      });
    }
  }

  _attachEvents() {
    // controls
    const controlsContainer = this.container.querySelector('#controls');
    for (const controlTypeIndex in CONTROLS) {
      this._createControlContainer(controlTypeIndex, controlsContainer);
    }

    // piano keys
    const piano = this.container.querySelector('#piano');
    let pressedNote = null;
    piano.addEventListener('mousedown', e => {
      if (e.target.classList.contains('key')) {
        const note = Number(e.target.dataset.note);
        this.setNoteOn(note);
        pressedNote = note;
      }
    });
    document.addEventListener('mouseup', e => {
      if (pressedNote !== null) {
        this.setNoteOff(pressedNote);
        pressedNote = null;
      }
    });
    piano.addEventListener('mouseover', e => {
      if (pressedNote !== null && e.target.classList.contains('key')) {
        const note = Number(e.target.dataset.note);
        this.setNoteOff(pressedNote);
        this.setNoteOn(note);
        pressedNote = note;
      }
    });
    piano.addEventListener('mouseleave', e => {
      if (pressedNote !== null) {
        this.setNoteOff(pressedNote);
        pressedNote = null;
      }
    });
  }

  _noteToPitch(note) {
    return note / 12.0;
  }

  setControl(typeIndex, value) {
    if (!this.handle) return;

    this.audioNode.port.postMessage({
      type: 'instrument-set-control',
      handle: this.handle,
      typeIndex: typeIndex,
      value: value,
    });
  }

  setNoteOff(note) {
    if (!this.handle) return;

    this.audioNode.port.postMessage({
      type: 'instrument-set-note-off',
      handle: this.handle,
      pitch: this._noteToPitch(note),
    });

    if (this.container) {
      this.container.querySelector(`[data-note="${note}"]`)?.classList.remove('active');
    }
  }

  setNoteOn(note, gain = 1.0, pitchShift = 0.0) {
    if (!this.handle) return;

    this.audioNode.port.postMessage({
      type: 'instrument-set-note-on',
      handle: this.handle,
      pitch: this._noteToPitch(note),
      gain: gain,
      pitchShift: pitchShift,
    });

    if (this.container) {
      this.container.querySelector(`[data-note="${note}"]`)?.classList.add('active');
    }
  }
}
