import {TaskEventType} from '../src/task.js';

export class Note {
  constructor(engine, performerui, position, duration, pitch, gain) {
    this._performerui = performerui;
    this._pitch = pitch;
    this.gain = gain;

    /** @private {number} */
    this._position = position;

    /** @private {number} */
    this._duration = duration;

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

  set duration(newDuration) {
    if (this._duration === newDuration) return;
    this._duration = newDuration;
    this._task.setDuration(newDuration);
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
    if (this._position === newPosition) return;
    this._position = newPosition;
    this._task.setPosition(newPosition);
  }

  get duration() {
    return this._duration;
  }

  get isActive() {
    return this._task.isActive;
  }

  get pitch() {
    return this._pitch;
  }

  get position() {
    return this._position;
  }
}
