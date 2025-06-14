import {Task, TaskState} from './task.js'

export class Note {
  constructor(performer, position, duration, pitch, gain) {
    this._performer = performer;
    this._position = position;
    this._duration = duration;

    this.pitch = pitch;
    this.gain = gain;

    this._task = this._performer.createTask(position, duration, (state) => {
      if (state == TaskState.BEGIN) {
        this._performer.selectedInstrument?.setNoteOn(this.pitch, this.gain);
      } else if (state == TaskState.END) {
        this._performer.selectedInstrument?.setNoteOff(this.pitch);
      }
    });
  }

  get duration() {
    return this._task.duration;
  }

  get position() {
    return this._task.position;
  }

  set duration(newDuration) {
    this._task.duration = newDuration;
  }

  set position(newPosition) {
    this._task.position = newPosition;
  }
}
