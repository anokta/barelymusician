import {CommandType} from './context.js'

/**
 * Task event types.
 * @enum {number}
 */
export const TaskEventType = {
  BEGIN: 0,
  END: 1,
  COUNT: 2,
};

/**
 * A representation of a recurring task that can be performed by a musical performer in real-time.
 */
export class Task {
  /**
   * @param {!Engine} engine
   * @param {number} id
   * @param {function(number):void} eventCallback
   */
  constructor(engine, id, eventCallback) {
    /** @private @const {!Engine} */
    this._engine = engine;

    /** @private @const {number} */
    this._id = id;

    /** @private {boolean} */
    this._isActive = false;

    /** @public */
    this.eventCallback = eventCallback;
  }

  /**
   * Destroys the task.
   */
  destroy() {
    this._engine._tasks.delete(this._id);
    this._engine._pushCommand({type: CommandType.TASK_DESTROY, id: this._id});
  }

  /** @param {number} duration */
  setDuration(duration) {
    this._engine._pushCommand({type: CommandType.TASK_SET_DURATION, id: this._id, duration});
  }

  /** @param {number} position */
  setPosition(position) {
    this._engine._pushCommand({type: CommandType.TASK_SET_POSITION, id: this._id, position});
  }

  /** @param {number} priority */
  setPriority(priority) {
    this._engine._pushCommand({type: CommandType.TASK_SET_PRIORITY, id: this._id, priority});
  }

  /** @return {number} */
  get id() {
    return this._id;
  }

  /** @return {boolean} */
  get isActive() {
    return this._isActive;
  }
}
