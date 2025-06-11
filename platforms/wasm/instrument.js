import {CONTROLS} from './control.js'

export class Instrument {
  constructor({container, audioNode, handlePromise, noteOnCallback, noteOffCallback}) {
    this._container = container;
    this._audioNode = audioNode;
    this._handlePromise = handlePromise;

    this.noteOnCallback = noteOnCallback;
    this.noteOffCallback = noteOffCallback;

    if (this._container) {
      this._initContainer();
    }

    this._audioNode.port.postMessage({type: 'instrument-create'});
  }

  async _withHandle(fn) {
    const handle = await this._handlePromise;
    return fn(handle);
  }

  _createControlContainer(controlTypeIndex, parentContainer) {
    if (!CONTROLS[controlTypeIndex]) return;
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

  _initContainer() {
    this._container.innerHTML = `
      <label id="name"></label>
      <div id="controls"></div>
      <div id="piano">
        <div id="keys">
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
      <button id="deleteBtn" title="Delete Instrument">
        <i class="material-icons">delete</i>
      </button>
    `;

    // controls
    const controlsContainer = this._container.querySelector('#controls');
    for (const controlTypeIndex in CONTROLS) {
      this._createControlContainer(controlTypeIndex, controlsContainer);
    }

    // piano keys
    const piano = this._container.querySelector('#piano');
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

    // delete
    this._container.querySelector('#deleteBtn').addEventListener('click', () => this.destroy());

    // id
    this._withHandle((handle) => {
      this._container.id = `instrument#${handle}`;

      // label
      const label = this._container.querySelector('label');
      label.textContent = this._container.id;
    });
  }

  _noteToPitch(note) {
    return note / 12.0;
  }

  destroy() {
    this._withHandle((handle) => {
      this._audioNode.port.postMessage({type: 'instrument-destroy', handle: handle});
    });
    this._container.remove();
  }

  setControl(typeIndex, value) {
    this._withHandle((handle) => {
      this._audioNode.port.postMessage({
        type: 'instrument-set-control',
        handle: handle,
        typeIndex: typeIndex,
        value: value,
      });
    });
  }

  setNoteOff(note) {
    this._withHandle((handle) => {
      this._audioNode.port.postMessage({
        type: 'instrument-set-note-off',
        handle: handle,
        pitch: this._noteToPitch(note),
      });

      if (this._container) {
        this._container.querySelector(`[data-note="${note}"]`)?.classList.remove('active');
      }
    });
  }

  setNoteOn(note, gain = 1.0, pitchShift = 0.0) {
    this._withHandle((handle) => {
      this._audioNode.port.postMessage({
        type: 'instrument-set-note-on',
        handle: handle,
        pitch: this._noteToPitch(note),
        gain: gain,
        pitchShift: pitchShift,
      });

      if (this._container) {
        this._container.querySelector(`[data-note="${note}"]`)?.classList.add('active');
      }
    });
  }
}
