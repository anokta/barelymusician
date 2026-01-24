import {TaskEventType} from '../src/task.js';

export class Note {
  constructor(engine, performerui, position, duration, pitch, gain) {
    this._performerui = performerui;
    this._pitch = pitch;
    this.gain = gain;

    this._task = engine.createTask(performerui.performer, position, duration, type => {
      if (type === TaskEventType.BEGIN) {
        this._performerui.selectedInstrument?.setNoteOn(this._pitch, this.gain);
      } else if (type === TaskEventType.END) {
        this._performerui.selectedInstrument?.setNoteOff(this._pitch);
      }
    });

    this.noteDiv = null;
  }

  /**
   * Destroys the note.
   */
  destroy() {
    this._performerui.selectedInstrument?.setNoteOff(this._pitch);
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
    if (this._pitch === newPitch) return;

    this._performerui.selectedInstrument?.setNoteOff(this._pitch);
    this._pitch = newPitch;
    if (this.isActive) {
      this._performerui.selectedInstrument?.setNoteOn(this._pitch, this.gain);
    }
  }

  set position(newPosition) {
    this._task.position = newPosition;
  }
}
