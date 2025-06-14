import {Note} from './note.js'
import {Task, TaskState} from './task.js'
import {Trigger} from './trigger.js'

const PITCHES = 13;  // e.g. 16 semitones (C4–C5)
const CLIP_HEIGHT = 240;
const CLIP_WIDTH = 440;
const ROW_HEIGHT = CLIP_HEIGHT / PITCHES;
const GRID_DIVISIONS = 16;  // e.g. 16 for 1/16th notes

const NOTE_NAMES = ['C', 'C#', 'D', 'D#', 'E', 'F', 'F#', 'G', 'G#', 'A', 'A#', 'B'];
const BASE_OCTAVE = 4;  // e.g. C4 is the bottom

export class Performer {
  constructor({container, audioNode, handlePromise, instruments}) {
    this._container = container;
    this._audioNode = audioNode;
    this._handlePromise = handlePromise;

    this._isLooping = false;
    this._isPlaying = false;
    this._loopBeginPosition = 0.0;
    this._loopLength = 1.0;
    this._position = 0.0;

    this._pendingTasks = [];
    this._pendingTriggers = [];

    this._notes = [];
    this._selectedInstrument = null;

    if (this._container) {
      this._initContainer(instruments);
    }

    this._audioNode.port.postMessage({type: 'performer-create'});
  }

  async _withHandle(fn) {
    const handle = await this._handlePromise;
    return fn(handle);
  }

  _renderClip() {
    const clip = this._container.querySelector('.clip');
    clip.innerHTML = '';

    // // Draw rows
    // for (let i = 0; i < PITCHES; ++i) {
    //   const row = document.createElement('div');
    //   row.className = 'clip-row';
    //   row.style.top = `${i * ROW_HEIGHT}px`;
    //   row.style.height = `${ROW_HEIGHT}px`;
    //   clip.appendChild(row);
    // }


    const getNoteName = (pitch) => {
      const note = NOTE_NAMES[pitch % 12];
      const octave = BASE_OCTAVE + Math.floor(pitch / 12);
      return note + octave;
    };

    // Draw horizontal rows (pitches)
    for (let i = 0; i < PITCHES; ++i) {
      const row = document.createElement('div');
      row.className = 'clip-row';
      row.style.top = `${i * ROW_HEIGHT}px`;
      row.style.height = `${ROW_HEIGHT}px`;
      clip.appendChild(row);

      const noteNameDiv = document.createElement('div');
      noteNameDiv.className = 'clip-note-name';
      noteNameDiv.style.top = `${i * ROW_HEIGHT}px`;
      noteNameDiv.textContent = getNoteName(PITCHES - 1 - i);
      clip.appendChild(noteNameDiv);
    }

    // // Draw vertical grid lines (beats or subdivisions)
    // const gridDivisions = 16;  // e.g., 16 for 1/16th notes in a 1-bar loop
    // for (let i = 0; i <= gridDivisions; ++i) {
    //   const gridLine = document.createElement('div');
    //   gridLine.className = 'clip-gridline';
    //   gridLine.style.left = `${(i / gridDivisions) * 100}%`;
    //   gridLine.style.top = '0';
    //   gridLine.style.height = '100%';
    //   clip.appendChild(gridLine);
    // }

    // Draw vertical grid lines and position markers
    for (let i = 0; i <= GRID_DIVISIONS; ++i) {
      const x = (i / GRID_DIVISIONS) * CLIP_WIDTH;

      // Grid line
      const gridLine = document.createElement('div');
      gridLine.className = 'clip-gridline';
      gridLine.style.left = `${x}px`;
      clip.appendChild(gridLine);

      // Position marker (every 4th grid, e.g. quarter note)
      if (i % 4 === 0) {
        const marker = document.createElement('div');
        marker.className = 'clip-marker';
        marker.style.left = `${x}px`;
        marker.textContent = (this.loopLength * i / GRID_DIVISIONS).toFixed(2);
        clip.appendChild(marker);
      }
    }

    // Draw notes
    for (const note of this._notes) {
      if (note._position >= this._loopLength) continue;

      const clampedDuration =
          Math.min(note._position + note._duration, this._loopLength) - note._position;

      const noteDiv = document.createElement('div');
      noteDiv.className = 'clip-note';
      noteDiv.style.left = `${note._position * CLIP_WIDTH / this._loopLength}px`;
      noteDiv.style.width = `${clampedDuration * CLIP_WIDTH / this._loopLength}px`;
      noteDiv.style.top = `${(PITCHES - 1 - note.pitch) * ROW_HEIGHT}px`;
      noteDiv.style.height = `${ROW_HEIGHT - 2}px`;

      // Drag/move/remove logic
      noteDiv.onmousedown = (e) => this._startNoteDrag(e, note);
      noteDiv.onclick = (e) => {
        if (e.detail === 2) this._removeNote(note);
      };  // double-click to remove

      clip.appendChild(noteDiv);
    }

    // Add note creation logic
    clip.onmousedown = (e) => this._startNoteCreate(e);
  }

  _startNoteCreate(e) {
    const snapToGrid = (x) => {
      const gridSize = CLIP_WIDTH / GRID_DIVISIONS;
      return Math.round(x / gridSize) * gridSize;
    };

    const clip = e.currentTarget;
    const rect = clip.getBoundingClientRect();
    let x = e.clientX - rect.left;
    const y = e.clientY - rect.top;

    x = snapToGrid(x);

    const pitch = PITCHES - 1 - Math.floor(y / ROW_HEIGHT);
    const start = x / CLIP_WIDTH * this._loopLength;

    let noteDiv = document.createElement('div');
    noteDiv.className = 'clip-note';
    noteDiv.style.top = `${(PITCHES - 1 - pitch) * ROW_HEIGHT}px`;
    noteDiv.style.left = `${x}px`;
    noteDiv.style.height = `${ROW_HEIGHT - 2}px`;
    clip.appendChild(noteDiv);

    const onMouseMove = (moveEvent) => {
      let moveX = moveEvent.clientX - rect.left;

      moveX = snapToGrid(moveX);

      const width = Math.max(8, moveX - x);
      noteDiv.style.width = `${width}px`;
    };

    const onMouseUp = (upEvent) => {
      document.removeEventListener('mousemove', onMouseMove);
      document.removeEventListener('mouseup', onMouseUp);
      const endX = upEvent.clientX - rect.left;
      const duration = Math.max(0.05, (endX - x) / CLIP_WIDTH * this._loopLength);
      this._addNote(start, duration, pitch, 1.0);
      clip.removeChild(noteDiv);
      this._renderClip();
    };

    document.addEventListener('mousemove', onMouseMove);
    document.addEventListener('mouseup', onMouseUp);
  }

  _addNote(start, duration, pitch, gain) {
    const note = new Note(this, start, duration, pitch, gain);
    this._notes.push(note);
  }

  _removeNote(note) {
    this._notes = this._notes.filter(n => n !== note);
    this._renderClip();
  }

  _startNoteDrag(e, note) {
    // Implement drag logic to move or resize note
    // (left as an exercise, but similar to _startNoteCreate)
  }

  _initContainer(instruments) {
    this._container.innerHTML = `
      <label id="name"></label>
      <button id="clearNotesBtn">Clear</button>
      <button id="loopDecBtn">-</button>
      <span id="loopLengthLabel">1</span>
      <button id="loopIncBtn">+</button>
      <select id="instrumentSelect"></select>
      <div class="clip"></div>
      <button id="deleteBtn" title="Delete Instrument">
        <i class="material-icons">delete</i>
      </button>
    `;

    // instrument select
    const instrumentSelect = this._container.querySelector('#instrumentSelect');
    instrumentSelect.addEventListener('change', () => {
      if (this._selectedInstrument) {
        this._selectedInstrument.setAllNotesOff();
      }
      this._selectedInstrument = instruments[instrumentSelect.value];
    });

    this.updateInstrumentSelect(instruments);

    // controls
    this._container.querySelector('#clearNotesBtn').onclick = () => {
      this._notes = [];
      this._renderClip();
    };

    const loopLengthLabel = this._container.querySelector('#loopLengthLabel');
    this._container.querySelector('#loopDecBtn').onclick = () => {
      this.loopLength = Math.max(this.loopLength - 1, 1);
      loopLengthLabel.textContent = this.loopLength;
      this._renderClip();
    };
    this._container.querySelector('#loopIncBtn').onclick = () => {
      this.loopLength = Math.min(this.loopLength + 1, 4);
      loopLengthLabel.textContent = this.loopLength;
      this._renderClip();
    };

    // delete
    this._container.querySelector('#deleteBtn').addEventListener('click', () => this.destroy());

    // id
    this._withHandle((handle) => {
      this._container.id = `performer#${handle}`;

      // label
      const label = this._container.querySelector('label');
      label.textContent = this._container.id;

      // TODO(#164): testonly
      this.isLooping = true;
      this._renderClip();
    });
  }

  get selectedInstrument() {
    return this._selectedInstrument;
  }

  get isLooping() {
    return this._isLooping;
  }

  get isPlaying() {
    return this._isPlaying;
  }

  get loopBeginPosition() {
    return this._loopBeginPosition;
  }

  get loopLength() {
    return this._loopLength;
  }

  get position() {
    return this._position;
  }

  /**
   * @param {bool} newIsLooping
   */
  set isLooping(newIsLooping) {
    if (this._isLooping == newIsLooping) return;

    // TODO(#164): Revisit how the properties are set when they are set (immediate vs deferred).
    this._isLooping = newIsLooping;
    this._withHandle((handle) => {
      this._audioNode.port.postMessage({
        type: 'performer-set-looping',
        handle: handle,
        isLooping: newIsLooping,
      });
    });
  }

  /**
   * @param {float} newLoopBeginPosition
   */
  set loopBeginPosition(newLoopBeginPosition) {
    if (this._loopBeginPosition == newLoopBeginPosition) return;

    this._loopBeginPosition = newLoopBeginPosition;
    this._withHandle((handle) => {
      this._audioNode.port.postMessage({
        type: 'performer-set-loop-begin-position',
        handle: handle,
        loopBeginPosition: newLoopBeginPosition,
      });
    });
  }

  /**
   * @param {float} newLoopLength
   */
  set loopLength(newLoopLength) {
    if (this._loopLength == newLoopLength) return;

    this._loopLength = Math.max(newLoopLength, 0.0);
    this._withHandle((handle) => {
      this._audioNode.port.postMessage({
        type: 'performer-set-loop-begin-position',
        handle: handle,
        loopLength: newLoopLength,
      });
    });
  }

  /**
   * @param {float} newPosition
   */
  set position(newPosition) {
    if (this._position == newPosition) return;

    this._position = newPosition;
    this._withHandle((handle) => {
      this._audioNode.port.postMessage({
        type: 'performer-set-position',
        handle: handle,
        position: newPosition,
      });
    });
  }

  createTask(position, duration, processCallback) {
    this._withHandle((handle) => {
      let resolveHandle;
      const handlePromise = new Promise(resolve => {
        resolveHandle = resolve;
      });
      const task = new Task({
        audioNode: this._audioNode,
        handlePromise: handlePromise,
        performerHandle: handle,
        position,
        duration,
        processCallback,
      });

      this._pendingTasks.push({task, resolveHandle});
    });
  }

  createTrigger(position, processCallback) {
    this._withHandle((handle) => {
      let resolveHandle;
      const handlePromise = new Promise(resolve => {
        resolveHandle = resolve;
      });
      const trigger = new Trigger({
        audioNode: this._audioNode,
        handlePromise: handlePromise,
        performerHandle: handle,
        position,
        processCallback,
      });

      this._pendingTriggers.push({trigger, resolveHandle});
    });
  }

  destroy() {
    this._withHandle((handle) => {
      this._audioNode.port.postMessage({type: 'performer-destroy', handle: handle});
    });
    if (this._container) {
      this._container.remove();
    }
  }

  onTaskCreateSuccess(handle) {
    const {task, resolveHandle} = this._pendingTasks.shift();
    resolveHandle(handle);
    return task;
  }

  onTriggerCreateSuccess(handle) {
    const {trigger, resolveHandle} = this._pendingTriggers.shift();
    resolveHandle(handle);
    return trigger;
  }

  start() {
    if (this._isPlaying) return;

    this._isPlaying = true;
    this._withHandle((handle) => {
      this._audioNode.port.postMessage({type: 'performer-start', handle: handle});
    });
  }

  stop() {
    if (!this._isPlaying) return;

    this._isPlaying = false;
    this._withHandle((handle) => {
      this._audioNode.port.postMessage({type: 'performer-stop', handle: handle});
    });
  }

  updateInstrumentSelect(instruments) {
    if (!this._container) return;

    const instrumentSelect = this._container.querySelector('#instrumentSelect');
    const currentInstrumentHandle = instrumentSelect.value;
    instrumentSelect.innerHTML = '';

    const noneOption = document.createElement('option');
    noneOption.value = 0;
    noneOption.textContent = 'none';
    instrumentSelect.appendChild(noneOption);

    Object.keys(instruments).forEach((handle) => {
      const option = document.createElement('option');
      option.value = handle;
      option.textContent = `instrument#${handle}`;
      instrumentSelect.appendChild(option);
    });

    if (instruments[currentInstrumentHandle]) {
      instrumentSelect.value = currentInstrumentHandle;
    }
    this._selectedInstrument = instruments[currentInstrumentHandle];
  }
}
