import {TaskState} from './task.js'

export class Note {
  constructor(performer, position, duration, pitch, gain) {
    this._performer = performer;
    this._pitch = pitch;

    this.gain = gain;

    this._task = performer.createTask(position, duration, (state) => {
      if (state == TaskState.BEGIN) {
        this._performer.selectedInstrument?.setNoteOn(this._pitch, this.gain);
      } else if (state == TaskState.END) {
        this._performer.selectedInstrument?.setNoteOff(this._pitch);
      }
    });

    this.noteDiv = null;
  }

  /**
   * Destroys the note.
   */
  destroy() {
    this._performer.selectedInstrument?.setNoteOff(this._pitch);
    this._task.destroy();
  }

  get duration() {
    return this._task.duration;
  }

  get isActive() {
    return this._task.isActive;
  }

  get pitch() {
    return this._pitch;
  }

  get position() {
    return this._task.position;
  }

  set duration(newDuration) {
    this._task.duration = newDuration;
  }

  set pitch(newPitch) {
    if (this._pitch == newPitch) return;

    if (this.isActive) {
      this._performer.selectedInstrument?.setNoteOff(this._pitch);
    }
    this._pitch = newPitch;
    if (this.isActive) {
      this._performer.selectedInstrument?.setNoteOn(this._pitch, this.gain);
    }
  }

  set position(newPosition) {
    this._task.position = newPosition;
  }
}
