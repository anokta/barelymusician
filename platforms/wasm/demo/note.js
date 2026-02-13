import {Command} from '../src/command.js';

export class Note {
  constructor(engine, performerui, position, duration, pitch, gain) {
    this._performerui = performerui;
    this._pitch = pitch;
    this.gain = gain;

    /** @private {number} */
    this._position = position;

    /** @private {number} */
    this._duration = duration;

    this._task = engine.createTask(performerui.performer, position, duration);
    this.updateInstrument(this._performerui.selectedInstrument?.instrument);

    this.noteDiv = null;
  }

  /**
   * Destroys the note.
   */
  destroy() {
    this._performerui.selectedInstrument?.instrument.setNoteOff(this._semitoneToPitch(this._pitch));
    this._task.destroy();
  }

  /**
   *
   * @param {!Instrument} instrument
   */
  updateInstrument(instrument) {
    if (instrument) {
      this._task.setCommands({
        beginCommands: [
          Command.instrument(instrument.handle)
              .setNoteOn(this._semitoneToPitch(this._pitch), this.gain),
        ],
        endCommands: [
          Command.instrument(instrument.handle).setNoteOff(this._semitoneToPitch(this._pitch)),
        ],
      });
    } else {
      this._task.setCommands();
    }
  }

  set duration(newDuration) {
    if (this._duration === newDuration) return;
    this._duration = newDuration;
    this._task.setDuration(newDuration);
  }

  set pitch(newPitch) {
    if (this._pitch === newPitch) return;

    this._performerui.selectedInstrument?.instrument.setNoteOff(this._semitoneToPitch(this._pitch));
    this._pitch = newPitch;
    if (this.isActive) {
      this._performerui.selectedInstrument?.instrument.setNoteOn(
          this._semitoneToPitch(this._pitch), this.gain);
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

  /**
   * Returns the corresponding note pitch for a given note in semitones.
   * @param {number} semitone
   * @return {number}
   * @private
   */
  _semitoneToPitch(semitone) {
    return semitone / 12.0;
  }
}
