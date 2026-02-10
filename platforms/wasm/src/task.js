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
   * @param {number} handle
   * @param {function(number):void} eventCallback
   */
  constructor(engine, handle, eventCallback) {
    /** @private @const {!Engine} */
    this._engine = engine;

    /** @private @const {number} */
    this._handle = handle;

    /** @private {boolean} */
    this._isActive = false;

    /** @public */
    this.eventCallback = eventCallback;
  }

  /** Destroys the task. */
  destroy() {
    this._engine._tasks.delete(this._handle);
    this._engine._pushCommand({type: CommandType.TASK_DESTROY, handle: this._handle});
  }

  /** @param {number} duration */
  setDuration(duration) {
    this._engine._pushCommand(
        {type: CommandType.TASK_SET_DURATION, handle: this._handle, duration});
  }

  /** @param {number} position */
  setPosition(position) {
    this._engine._pushCommand(
        {type: CommandType.TASK_SET_POSITION, handle: this._handle, position});
  }

  /** @param {number} priority */
  setPriority(priority) {
    this._engine._pushCommand(
        {type: CommandType.TASK_SET_PRIORITY, handle: this._handle, priority});
  }

  /** @return {number} */
  get handle() {
    return this._handle;
  }

  /** @return {boolean} */
  get isActive() {
    return this._isActive;
  }
}
