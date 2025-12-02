import {CONTROLS} from './control.js';

/**
 * Implements UI and logic of a musical instrument.
 */
export class Instrument {
  /**
   * @param {!Object} params
   * @param {!Element} params.container
   * @param {!AudioContext} params.audioContext
   * @param {!AudioWorkletNode} params.audioNode
   * @param {!Promise<number>} params.handlePromise
   * @param {function(number):void} params.noteOnCallback
   * @param {function(number):void} params.noteOffCallback
   */
  constructor(
      {container, audioContext, audioNode, handlePromise, noteOnCallback, noteOffCallback}) {
    /** @private @const {!Element} */
    this._container = container;
    /** @private @const {!AudioContext} */
    this._audioContext = audioContext;
    /** @private @const {!AudioWorkletNode} */
    this._audioNode = audioNode;
    /** @private @const {!Promise<number>} */
    this._handlePromise = handlePromise;

    /** @public */
    this.noteOnCallback = noteOnCallback;
    /** @public */
    this.noteOffCallback = noteOffCallback;

    if (this._container) {
      this._initContainer();
    }

    this._audioNode.port.postMessage({type: 'instrument-create'});
    this.setSampleData([{pitch: 0.0, url: 'data/sample.wav'}]);
  }

  /**
   * Destroys the instrument and removes its UI.
   * @return {!Promise<void>}
   */
  async destroy() {
    await this._withHandle(handle => {
      this._audioNode.port.postMessage({type: 'instrument-destroy', handle});
    });
    if (this._container) {
      this._container.remove();
    }
  }

  /**
   * Sets all notes off.
   */
  setAllNotesOff() {
    this._withHandle(handle => {
      this._audioNode.port.postMessage({
        type: 'instrument-set-all-notes-off',
        handle,
      });
    });
  }

  /**
   * Sets a control value.
   * @param {number} typeIndex
   * @param {number} value
   */
  setControl(typeIndex, value) {
    this._withHandle(handle => {
      this._audioNode.port.postMessage({
        type: 'instrument-set-control',
        handle,
        typeIndex,
        value,
      });
    });

    if (this._container) {
      const controlContainer =
          this._container.querySelector('#controls').querySelector(`#control-${typeIndex}`);
      if (controlContainer.querySelector('input')) {
        controlContainer.querySelector('input').value = value;
        controlContainer.querySelector(`#input-number-${typeIndex}`).value = value;
      } else {
        controlContainer.querySelector('select').value = value;
      }
    }
  }

  /**
   * Sets a note off.
   * @param {number} note
   */
  setNoteOff(note) {
    this._withHandle(handle => {
      this._audioNode.port.postMessage({
        type: 'instrument-set-note-off',
        handle,
        pitch: this._noteToPitch(note),
      });

      if (this._container) {
        this._container.querySelector(`[data-note="${note}"]`)?.classList.remove('active');
      }
    });
  }

  /**
   * Sets a note on.
   * @param {number} note
   * @param {number=} gain
   * @param {number=} pitchShift
   */
  setNoteOn(note, gain = 1.0, pitchShift = 0.0) {
    this._withHandle(handle => {
      this._audioNode.port.postMessage({
        type: 'instrument-set-note-on',
        handle,
        pitch: this._noteToPitch(note),
        gain,
        pitchShift,
      });

      if (this._container) {
        this._container.querySelector(`[data-note="${note}"]`)?.classList.add('active');
      }
    });
  }

  /**
   * Set instrument sample data.
   *
   * @param {!Array<{pitch: number, url: string}>} sampleData
   * @return {!Promise<void>}
   */
  async setSampleData(sampleData) {
    const slices = [];
    for (const {pitch, url} of sampleData) {
      const response = await fetch(url);
      if (!response.ok) {
        console.warn(`Invalid sample data for pitch ${pitch}`);
        continue;
      }
      const arrayBuffer = await response.arrayBuffer();
      const audioBuffer = await this._audioContext.decodeAudioData(arrayBuffer);
      slices.push({
        root_pitch: pitch,
        sample_rate: audioBuffer.sampleRate,
        samples: audioBuffer.getChannelData(0),  // mono assumed for now.
      });
    }
    await this._withHandle(async handle => {
      this._audioNode.port.postMessage({
        type: 'instrument-set-sample-data',
        handle,
        slices,
      });
    });
  }

  /**
   * Creates a control UI for a given control type.
   * @param {number} controlTypeIndex
   * @param {!Element} parentContainer
   * @private
   */
  _createControlContainer(controlTypeIndex, parentContainer) {
    if (!CONTROLS[controlTypeIndex]) return;
    const control = CONTROLS[controlTypeIndex];

    // Container
    const controlContainer = document.createElement('div');
    controlContainer.id = `control-${controlTypeIndex}`;
    controlContainer.classList.add('control-container');
    parentContainer.appendChild(controlContainer);

    // Label
    const controlLabel = document.createElement('label');
    controlLabel.textContent = control.name;
    controlLabel.htmlFor = `input-${controlTypeIndex}`;
    controlContainer.appendChild(controlLabel);

    // Input
    if (control.valueType !== 'bool' && control.valueType !== 'float' &&
        control.valueType !== 'int') {  // enum type
      const select = document.createElement('select');
      select.id = `input-${controlTypeIndex}`;

      for (const [key, value] of Object.entries(control.valueType)) {
        if (typeof value !== 'number' || key === 'COUNT') continue;
        const option = document.createElement('option');
        option.value = value;
        option.textContent =
            key.replace(/_/g, ' ').toLowerCase().replace(/\b\w/g, c => c.toUpperCase());
        if (value === control.defaultValue) option.selected = true;
        select.appendChild(option);
      }

      select.addEventListener('change', e => {
        const value = parseInt(e.target.value, 10);
        this.setControl(controlTypeIndex, value);
      });

      controlContainer.appendChild(select);
      return;
    }

    const controlInput = document.createElement('input');
    controlInput.id = `input-${controlTypeIndex}`;
    switch (control.valueType) {
      case 'bool':
        controlInput.type = 'checkbox';
        controlInput.checked = control.defaultValue;
        break;
      case 'int':
        controlInput.type = 'range';
        controlInput.min = control.minValue;
        controlInput.max = control.maxValue;
        controlInput.step = 1;
        controlInput.value = control.defaultValue;
        break;
      case 'float':
        controlInput.type = 'range';
        controlInput.min = control.minValue;
        controlInput.max = control.maxValue;
        controlInput.step = 0.01 * (control.maxValue - control.minValue);
        controlInput.value = control.defaultValue;
        break;
      default:
        console.error(`Invalid control value type: ${control.valueType}`);
        controlContainer.remove();
        return;
    }
    controlContainer.appendChild(controlInput);

    if (control.valueType === 'bool') {
      controlInput.addEventListener(
          'change', e => this.setControl(controlTypeIndex, e.target.checked ? 1.0 : 0.0));
    } else {
      // Value input (number box)
      const controlValueInput = document.createElement('input');
      controlValueInput.type = 'number';
      controlValueInput.id = `input-number-${controlTypeIndex}`;
      controlValueInput.value = controlInput.value;
      controlValueInput.min = controlInput.min;
      controlValueInput.max = controlInput.max;
      controlValueInput.step = controlInput.step;
      controlValueInput.classList.add('value-input');
      controlContainer.appendChild(controlValueInput);

      // Sync slider and number input
      controlInput.addEventListener('input', e => {
        let value =
            control.valueType === 'int' ? parseInt(e.target.value, 10) : parseFloat(e.target.value);
        if (!Number.isNaN(value)) {
          value = Math.min(Math.max(value, controlValueInput.min), controlValueInput.max);
          controlValueInput.value = value;
          this.setControl(controlTypeIndex, value);
        }
      });
      controlValueInput.addEventListener('change', e => {
        let value =
            control.valueType === 'int' ? parseInt(e.target.value, 10) : parseFloat(e.target.value);
        if (!Number.isNaN(value)) {
          value = Math.min(Math.max(value, controlInput.min), controlInput.max);
          controlInput.value = Math.min(Math.max(value, controlInput.min), controlInput.max);
          this.setControl(controlTypeIndex, value);
        }
      });
    }
  }

  /**
   * Initializes the container for the instrument UI.
   * @private
   */
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

    // Controls
    const controlsContainer = this._container.querySelector('#controls');
    for (const controlTypeIndex in CONTROLS) {
      this._createControlContainer(controlTypeIndex, controlsContainer);
    }

    // Piano keys
    const piano = this._container.querySelector('#piano');
    let pressedNote = null;
    piano.addEventListener('mousedown', e => {
      if (e.target.classList.contains('key')) {
        const note = Number(e.target.dataset.note);
        this.setNoteOn(note);
        pressedNote = note;
      }
    });
    document.addEventListener('mouseup', () => {
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
    piano.addEventListener('mouseleave', () => {
      if (pressedNote !== null) {
        this.setNoteOff(pressedNote);
        pressedNote = null;
      }
    });

    piano.addEventListener('touchstart', e => {
      const touch = e.touches[0];
      const target = document.elementFromPoint(touch.clientX, touch.clientY);
      if (target && target.classList.contains('key')) {
        const note = Number(target.dataset.note);
        this.setNoteOn(note);
        pressedNote = note;
        e.preventDefault();
      }
    }, {passive: false});
    piano.addEventListener('touchend', e => {
      if (pressedNote !== null) {
        this.setNoteOff(pressedNote);
        pressedNote = null;
        e.preventDefault();
      }
    }, {passive: false});
    piano.addEventListener('touchmove', e => {
      if (pressedNote !== null) {
        const touch = e.touches[0];
        const target = document.elementFromPoint(touch.clientX, touch.clientY);
        if (target && target.classList.contains('key')) {
          const note = Number(target.dataset.note);
          if (note !== pressedNote) {
            this.setNoteOff(pressedNote);
            this.setNoteOn(note);
            pressedNote = note;
          }
        }
        e.preventDefault();
      }
    }, {passive: false});

    // Delete button
    this._container.querySelector('#deleteBtn').addEventListener('click', () => this.destroy());

    // Set id and label
    this._withHandle(handle => {
      this._container.id = `instrument#${handle}`;
      const label = this._container.querySelector('label');
      label.textContent = this._container.id;
    });
  }

  /**
   * Returns the corresponding note pitch for a given note in semitones.
   * @param {number} note
   * @return {number}
   * @private
   */
  _noteToPitch(note) {
    return note / 12.0;
  }

  /**
   * Helper to run a function with the resolved handle.
   * @param {function(number):void} fn
   * @return {!Promise}
   * @private
   */
  async _withHandle(fn) {
    const handle = await this._handlePromise;
    return fn(handle);
  }
}
