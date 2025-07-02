import {Note} from './note.js'
import {Task, TaskState} from './task.js'
import {Trigger} from './trigger.js'

const PITCHES = 14;  // e.g. 16 semitones (C4–C5)
const CLIP_HEIGHT = 240;
const CLIP_WIDTH = 440;
const ROW_HEIGHT = CLIP_HEIGHT / PITCHES;
const GRID_DIVISIONS = 16;  // e.g. 16 for 1/16th notes
const GRID_SIZE = CLIP_WIDTH / GRID_DIVISIONS;

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

  createTask(position, duration, processCallback) {
    let resolveHandle;
    const handlePromise = new Promise(resolve => {
      resolveHandle = resolve;
    });
    const task = new Task({
      audioNode: this._audioNode,
      handlePromise,
      position,
      duration,
      processCallback,
    });

    this._pendingTasks.push({task, resolveHandle});

    this._withHandle((handle) => {
      this._audioNode.port.postMessage(
          {type: 'task-create', performerHandle: handle, position, duration});
    });

    return task;
  }

  createTrigger(position, processCallback) {
    let resolveHandle;
    const handlePromise = new Promise(resolve => {
      resolveHandle = resolve;
    });
    const trigger = new Trigger({
      audioNode: this._audioNode,
      handlePromise,
      position,
      processCallback,
    });

    this._pendingTriggers.push({trigger, resolveHandle});

    this._withHandle((handle) => {
      this._audioNode.port.postMessage({type: 'trigger-create', performerHandle: handle, position});
    });

    return trigger;
  }

  destroy() {
    this._destroyAllNotes();
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

  _renderClip() {
    const clip = this._container.querySelector('.clip');
    clip.innerHTML = '';

    const getNoteName = (pitch) => {
      const note = NOTE_NAMES[pitch % 12];
      const octave = BASE_OCTAVE + Math.floor(pitch / 12);
      return note + octave;
    };

    // Draw horizontal rows (pitches)
    for (let i = 0; i < PITCHES; ++i) {
      const y = i * ROW_HEIGHT;

      const row = document.createElement('div');
      row.className = 'clip-row';
      row.style.top = `${y}px`;
      row.style.left = `${GRID_SIZE}px`;
      row.style.width = `${CLIP_WIDTH}px`;
      row.style.height = `${ROW_HEIGHT}px`;
      clip.appendChild(row);

      if (i == 0) continue;

      const noteNameDiv = document.createElement('div');
      noteNameDiv.className = 'clip-note-name';
      noteNameDiv.style.top = `${y}px`;
      noteNameDiv.textContent = getNoteName(PITCHES - 1 - i);
      clip.appendChild(noteNameDiv);
    }

    // Draw vertical grid lines and position markers
    for (let i = 0; i <= GRID_DIVISIONS; ++i) {
      const x = (i + 1) * GRID_SIZE;

      // Grid line
      const gridLine = document.createElement('div');
      gridLine.className = 'clip-gridline';
      gridLine.style.top = `${ROW_HEIGHT}px`;
      gridLine.style.left = `${x}px`;
      gridLine.style.height = `${CLIP_HEIGHT - ROW_HEIGHT}px`;
      clip.appendChild(gridLine);

      // Position marker (every 4th grid, e.g. quarter note)
      if (i % 2 === 0) {
        const marker = document.createElement('div');
        marker.className = 'clip-marker';
        marker.style.left = `${x}px`;
        marker.textContent = (this._loopLength * i / GRID_DIVISIONS).toFixed(3);
        clip.appendChild(marker);
      }
    }

    // Draw notes
    for (const note of this._notes) {
      if (note.position >= this._loopLength) continue;

      const clampedDuration =
          Math.min(note.position + note.duration, this._loopLength) - note.position;

      note.noteDiv = document.createElement('div');
      note.noteDiv.className = 'clip-note';
      note.noteDiv.style.left = `${
          (note.position + this._loopLength / GRID_DIVISIONS) * CLIP_WIDTH / this._loopLength}px`;
      note.noteDiv.style.width = `${clampedDuration * CLIP_WIDTH / this._loopLength}px`;
      note.noteDiv.style.top = `${(PITCHES - 1 - note.pitch) * ROW_HEIGHT}px`;
      note.noteDiv.style.height = `${ROW_HEIGHT - 2}px`;

      note.noteDiv.onmousedown = (e) => this._startNoteDrag(e, note, note.noteDiv);

      clip.appendChild(note.noteDiv);
    }

    // Add note creation logic
    clip.onmousedown = (e) => this._startNoteCreate(e);
  }

  _startNoteCreate(e) {
    const clip = e.currentTarget;
    const rect = clip.getBoundingClientRect();
    const x = this._snapToGrid(e.clientX - rect.left);
    const y = e.clientY - rect.top;

    if (x < GRID_SIZE || x > CLIP_WIDTH || y < ROW_HEIGHT || y > CLIP_HEIGHT) {
      return;
    }

    const pitch = PITCHES - 1 - Math.floor(y / ROW_HEIGHT);
    const start = (this._loopLength * x) / CLIP_WIDTH;

    let noteDiv = document.createElement('div');
    noteDiv.className = 'clip-note';
    noteDiv.style.top = `${(PITCHES - 1 - pitch) * ROW_HEIGHT}px`;
    noteDiv.style.left = `${x}px`;
    noteDiv.style.height = `${ROW_HEIGHT - 2}px`;
    clip.appendChild(noteDiv);

    const onMouseMove = (moveEvent) => {
      let moveX = moveEvent.clientX - rect.left;

      moveX = this._snapToGrid(moveX);

      const width = Math.max(moveX - x, 0.0);
      noteDiv.style.width = `${width}px`;
    };

    const onMouseUp = (upEvent) => {
      document.removeEventListener('mousemove', onMouseMove);
      document.removeEventListener('mouseup', onMouseUp);
      const endX = Math.min(upEvent.clientX - rect.left, CLIP_WIDTH + GRID_SIZE);
      const duration = this._snapToGrid(endX - x) / CLIP_WIDTH * this._loopLength;
      if (duration > 0) {
        this._addNote(start - this._loopLength / GRID_DIVISIONS, duration, pitch, 1.0);
      }
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
    this._notes = this._notes.filter(n => {
      if (n === note) {
        n.destroy();
        return false;
      }
      return true;
    });
    this._renderClip();
  }

  _startNoteDrag(e, note, noteDiv) {
    e.preventDefault();
    e.stopPropagation();

    if (e.detail == 2) {  // double click to remove
      this._removeNote(note);
      return;
    }

    const clipRect = noteDiv.parentElement.getBoundingClientRect();
    const startX = e.clientX - clipRect.left;
    const noteLeft = parseFloat(noteDiv.style.left);
    const noteWidth = parseFloat(noteDiv.style.width);

    const edgeThreshold = 6;  // px
    let dragMode = 'move';
    if (Math.abs(startX - noteLeft) < edgeThreshold) {
      dragMode = 'resize-left';
      noteDiv.style.cursor = 'ew-resize';
    } else if (Math.abs(startX - (noteLeft + noteWidth)) < edgeThreshold) {
      dragMode = 'resize-right';
      noteDiv.style.cursor = 'ew-resize';
    } else {
      noteDiv.style.cursor = 'move';
    }

    const origPosition = note.position;
    const origDuration = note.duration;

    const onMouseMove = (moveEvent) => {
      let moveX = moveEvent.clientX - clipRect.left;
      let moveY = moveEvent.clientY - clipRect.top;

      if (dragMode === 'move') {
        // Snap left edge to grid, keep duration
        let newLeft = this._snapToGrid(moveX - (startX - noteLeft));
        const newPitch =
            Math.min(Math.max(PITCHES - 1 - Math.floor(moveY / ROW_HEIGHT), 0), PITCHES - 2);
        note.pitch = newPitch;
        // Clamp so note stays in bounds
        newLeft = Math.min(Math.max(newLeft, GRID_SIZE), CLIP_WIDTH - noteWidth + GRID_SIZE);
        noteDiv.style.left = `${newLeft}px`;
        noteDiv.style.top = `${(PITCHES - 1 - newPitch) * ROW_HEIGHT}px`;
      } else if (dragMode === 'resize-left') {
        let newLeft = Math.max(GRID_SIZE, this._snapToGrid(moveX));
        let newWidth = noteWidth + (noteLeft - newLeft);
        if (newWidth < GRID_SIZE) {
          newWidth = GRID_SIZE;
          newLeft = noteLeft + noteWidth - GRID_SIZE;
        }
        noteDiv.style.left = `${newLeft}px`;
        noteDiv.style.width = `${newWidth}px`;
      } else if (dragMode === 'resize-right') {
        let newRight = Math.min(this._snapToGrid(moveX), CLIP_WIDTH + GRID_SIZE);
        let newWidth = Math.max(GRID_SIZE, newRight - noteLeft);
        noteDiv.style.width = `${newWidth}px`;
      }
    };

    const onMouseUp = (upEvent) => {
      document.removeEventListener('mousemove', onMouseMove);
      document.removeEventListener('mouseup', onMouseUp);
      noteDiv.style.cursor = '';

      let finalLeft = parseFloat(noteDiv.style.left);
      let finalWidth = parseFloat(noteDiv.style.width);
      let finalTop = parseFloat(noteDiv.style.top);

      if (dragMode === 'move') {
        note.position =
            (finalLeft * this._loopLength) / CLIP_WIDTH - this._loopLength / GRID_DIVISIONS;
        note.pitch = PITCHES - 1 - Math.round(finalTop / ROW_HEIGHT);
      } else if (dragMode === 'resize-left') {
        const newStart =
            (finalLeft * this._loopLength) / CLIP_WIDTH - this._loopLength / GRID_DIVISIONS;
        const newDuration = origPosition + origDuration - newStart;
        if (newDuration > 0) {
          note.position = newStart;
          note.duration = newDuration;
        }
      } else if (dragMode === 'resize-right') {
        const newDuration = (finalWidth * this._loopLength) / CLIP_WIDTH;
        if (newDuration > 0) {
          note.duration = newDuration;
        }
      }
      this._renderClip();
    };

    document.addEventListener('mousemove', onMouseMove);
    document.addEventListener('mouseup', onMouseUp);
  }

  _initContainer(instruments) {
    this._container.innerHTML = `
      <label id="name"></label>
      <button id="loopDecBtn">-</button>
      <span id="loopLengthLabel">${this._loopLength}</span>
      <button id="loopIncBtn">+</button>
      <select id="instrumentSelect"></select>
      <button id="clearNotesBtn">Clear</button>
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
      this._destroyAllNotes();
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

      this._renderClip();
    });

    this._startRenderLoop();
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
        type: 'performer-set-loop-length',
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

  _destroyAllNotes() {
    for (const note of this._notes) {
      note.destroy();
    }
    this._notes = [];
  }

  _snapToGrid(x) {
    return Math.min(Math.round(x / GRID_SIZE) * GRID_SIZE, CLIP_WIDTH + GRID_SIZE);
  }

  _startRenderLoop() {
    const render = () => {
      for (const note of this._notes) {
        if (note.noteDiv) {
          note.noteDiv.style.background = (note.isActive ? '#8ff' : '#4af');
        }
      }
      requestAnimationFrame(render);
    };
    render();
  }

  async _withHandle(fn) {
    const handle = await this._handlePromise;
    return fn(handle);
  }
}
