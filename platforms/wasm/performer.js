import {Note} from './note.js';
import {Task} from './task.js';

const PITCHES = 14;
const CLIP_HEIGHT = 240;
const CLIP_WIDTH = 440;
const ROW_HEIGHT = CLIP_HEIGHT / PITCHES;
const GRID_DIVISIONS = 16;
const GRID_SIZE = CLIP_WIDTH / GRID_DIVISIONS;
const MAX_LOOP_LENGTH = 16;

const NOTE_NAMES = ['C', 'C#', 'D', 'D#', 'E', 'F', 'F#', 'G', 'G#', 'A', 'A#', 'B'];
const BASE_OCTAVE = 4;

export class Performer {
  /**
   * @param {!Object} options
   */
  constructor({container, audioNode, idPromise, instruments}) {
    this._container = container;
    this._audioNode = audioNode;
    this._idPromise = idPromise;

    this._isLooping = false;
    this._isPlaying = false;
    this._loopBeginPosition = 0.0;
    this._loopLength = 1.0;
    this._position = 0.0;

    this._pendingTasks = [];

    this._notes = [];
    this._selectedInstrument = null;

    if (this._container) {
      this._initContainer(instruments);
    }

    this._audioNode.port.postMessage({type: 'performer-create'});
  }

  /**
   * @param {number} position
   * @param {number} duration
   * @param {function} eventCallback
   * @return {!Task}
   */
  createTask(position, duration, eventCallback) {
    let resolveId;
    const idPromise = new Promise(resolve => {
      resolveId = resolve;
    });
    const task = new Task({
      audioNode: this._audioNode,
      idPromise,
      position,
      duration,
      eventCallback,
    });

    this._pendingTasks.push({task, resolveId});

    this._withId(id => {
      this._audioNode.port.postMessage({
        type: 'task-create',
        performerHandle: id,
        position,
        duration,
      });
    });

    return task;
  }

  /**
   * Destroys the performer and cleans up resources.
   * @return {!Promise<void>}
   */
  async destroy() {
    this._destroyAllNotes();
    await this._withId(id => {
      this._audioNode.port.postMessage({type: 'performer-destroy', id});
    });
    if (this._container) {
      this._container.remove();
    }
  }

  /**
   * @return {number}
   */
  getSelectedInstrumentHandle() {
    if (!this._container) return 0;
    return this._container.querySelector('#instrumentSelect').value;
  }

  /**
   * @param {number} id
   * @return {!Task}
   */
  onTaskCreateSuccess(id) {
    const {task, resolveId} = this._pendingTasks.shift();
    resolveId(id);
    return task;
  }

  /**
   * Starts playback.
   */
  start() {
    this._isPlaying = true;
    this._withId(id => {
      this._audioNode.port.postMessage({type: 'performer-start', id});
    });
  }

  /**
   * Stops playback.
   */
  stop() {
    this._isPlaying = false;
    this._withId(id => {
      this._audioNode.port.postMessage({type: 'performer-stop', id});
    });
  }

  /**
   * Updates the instrument select dropdown.
   * @param {!Object} instruments
   */
  updateInstrumentSelect(instruments) {
    if (!this._container) return;

    const instrumentSelect = this._container.querySelector('#instrumentSelect');
    const currentInstrumentHandle = instrumentSelect.value;
    instrumentSelect.innerHTML = '';

    const noneOption = document.createElement('option');
    noneOption.value = 0;
    noneOption.textContent = 'none';
    instrumentSelect.appendChild(noneOption);

    Object.keys(instruments).forEach(id => {
      const option = document.createElement('option');
      option.value = id;
      option.textContent = `instrument#${id}`;
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

    const getNoteName = pitch => {
      const note = NOTE_NAMES[pitch % 12];
      const octave = BASE_OCTAVE + Math.floor(pitch / 12);
      return `${note}${octave}`;
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

      if (i === 0) continue;

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

      note.noteDiv.onmousedown = e => this._startNoteDrag(e, note, note.noteDiv);
      note.noteDiv.ontouchstart = e => {
        if (e.touches.length > 1) return;
        const touch = e.touches[0];
        // Synthesize a mouse-like event object.
        const fakeEvent = {
          ...e,
          clientX: touch.clientX,
          clientY: touch.clientY,
          currentTarget: clip,
          preventDefault: () => e.preventDefault(),
          stopPropagation: () => e.stopPropagation(),
          detail: e.detail || 1,
        };
        this._startNoteDrag(fakeEvent, note, note.noteDiv);
        e.preventDefault();
      };

      clip.appendChild(note.noteDiv);
    }

    // Add note creation logic
    clip.onmousedown = e => this._startNoteCreate(e);
    clip.ontouchstart = e => {
      if (e.touches.length > 1) return;
      const touch = e.touches[0];
      // Synthesize a mouse-like event object.
      const fakeEvent = {
        ...e,
        clientX: touch.clientX,
        clientY: touch.clientY,
        currentTarget: clip,
        preventDefault: () => e.preventDefault(),
        stopPropagation: () => e.stopPropagation(),
        detail: 1,
      };
      this._startNoteCreate(fakeEvent);
      e.preventDefault();
    };
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

    const noteDiv = document.createElement('div');
    noteDiv.className = 'clip-note';
    noteDiv.style.top = `${(PITCHES - 1 - pitch) * ROW_HEIGHT}px`;
    noteDiv.style.left = `${x}px`;
    noteDiv.style.height = `${ROW_HEIGHT - 2}px`;
    clip.appendChild(noteDiv);

    const onMouseMove = moveEvent => {
      let moveX = moveEvent.clientX - rect.left;
      moveX = this._snapToGrid(moveX);
      const width = Math.max(moveX - x, 0.0);
      noteDiv.style.width = `${width}px`;
    };

    const onTouchMove = touchEvent => {
      if (touchEvent.touches.length > 1) return;
      const touch = touchEvent.touches[0];
      onMouseMove({clientX: touch.clientX, clientY: touch.clientY});
      touchEvent.preventDefault();
    };

    const onTouchEnd = touchEvent => {
      const touch = (touchEvent.changedTouches && touchEvent.changedTouches[0]) || {};
      onMouseUp({clientX: touch.clientX || 0, clientY: touch.clientY || 0});
      touchEvent.preventDefault();
    };

    const onMouseUp = upEvent => {
      document.removeEventListener('mousemove', onMouseMove);
      document.removeEventListener('mouseup', onMouseUp);
      document.removeEventListener('touchmove', onTouchMove);
      document.removeEventListener('touchend', onTouchEnd);
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
    document.addEventListener('touchmove', onTouchMove, {passive: false});
    document.addEventListener('touchend', onTouchEnd, {passive: false});
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

    if (e.detail === 2) {
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

    const onTouchMove = touchEvent => {
      if (touchEvent.touches.length > 1) return;
      const touch = touchEvent.touches[0];
      onMouseMove({clientX: touch.clientX, clientY: touch.clientY});
      touchEvent.preventDefault();
    };

    const onTouchEnd = touchEvent => {
      const touch = (touchEvent.changedTouches && touchEvent.changedTouches[0]) || {};
      onMouseUp({clientX: touch.clientX || 0, clientY: touch.clientY || 0});
      touchEvent.preventDefault();
    };

    const onMouseMove = moveEvent => {
      let moveX = moveEvent.clientX - clipRect.left;
      let moveY = moveEvent.clientY - clipRect.top;

      if (dragMode === 'move') {
        let newLeft = this._snapToGrid(moveX - (startX - noteLeft));
        const newPitch =
            Math.min(Math.max(PITCHES - 1 - Math.floor(moveY / ROW_HEIGHT), 0), PITCHES - 2);
        note.pitch = newPitch;
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

    const onMouseUp = upEvent => {
      document.removeEventListener('mousemove', onMouseMove);
      document.removeEventListener('mouseup', onMouseUp);
      document.removeEventListener('touchmove', onTouchMove);
      document.removeEventListener('touchend', onTouchEnd);
      noteDiv.style.cursor = '';

      const finalLeft = parseFloat(noteDiv.style.left);
      const finalWidth = parseFloat(noteDiv.style.width);
      const finalTop = parseFloat(noteDiv.style.top);

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
    document.addEventListener('touchmove', onTouchMove, {passive: false});
    document.addEventListener('touchend', onTouchEnd, {passive: false});
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
    this._container.querySelector('#clearNotesBtn').addEventListener('click', () => {
      this._destroyAllNotes();
      this._renderClip();
    });

    const loopLengthLabel = this._container.querySelector('#loopLengthLabel');
    this._container.querySelector('#loopDecBtn').addEventListener('click', () => {
      this.loopLength = Math.max(this.loopLength - 1, 1);
      loopLengthLabel.textContent = this.loopLength;
      this._renderClip();
    });
    this._container.querySelector('#loopIncBtn').addEventListener('click', () => {
      this.loopLength = Math.min(this.loopLength + 1, MAX_LOOP_LENGTH);
      loopLengthLabel.textContent = this.loopLength;
      this._renderClip();
    });

    // delete
    this._container.querySelector('#deleteBtn').addEventListener('click', () => this.destroy());

    // id
    this._withId(id => {
      this._container.id = `performer#${id}`;

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
   * @param {boolean} newIsLooping
   */
  set isLooping(newIsLooping) {
    if (this._isLooping === newIsLooping) return;

    this._isLooping = newIsLooping;
    this._withId(id => {
      this._audioNode.port.postMessage({
        type: 'performer-set-looping',
        id,
        isLooping: newIsLooping,
      });
    });
  }

  /**
   * @param {number} newLoopBeginPosition
   */
  set loopBeginPosition(newLoopBeginPosition) {
    if (this._loopBeginPosition === newLoopBeginPosition) return;

    this._loopBeginPosition = newLoopBeginPosition;
    this._withId(id => {
      this._audioNode.port.postMessage({
        type: 'performer-set-loop-begin-position',
        id,
        loopBeginPosition: newLoopBeginPosition,
      });
    });
  }

  /**
   * @param {number} newLoopLength
   */
  set loopLength(newLoopLength) {
    if (this._loopLength === newLoopLength) return;

    this._loopLength = Math.max(newLoopLength, 0.0);
    this._withId(id => {
      this._audioNode.port.postMessage({
        type: 'performer-set-loop-length',
        id,
        loopLength: newLoopLength,
      });
    });

    if (this._container) {
      this._container.querySelector('#loopLengthLabel').textContent = this._loopLength;
    }
  }

  /**
   * @param {number} newPosition
   */
  set position(newPosition) {
    if (this._position === newPosition) return;

    this._position = newPosition;
    this._withId(id => {
      this._audioNode.port.postMessage({
        type: 'performer-set-position',
        id,
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

  async _withId(fn) {
    const id = await this._idPromise;
    return fn(id);
  }
}
