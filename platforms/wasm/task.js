export const TaskState = {
  BEGIN: 0,
  END: 1,
  UPDATE: 2,
  COUNT: 3,
};

export class Task {
  constructor({audioNode, handlePromise, position, duration, processCallback}) {
    this._audioNode = audioNode;
    this._handlePromise = handlePromise;

    this._position = position;
    this._duration = duration;
    this._isActive = false;

    this.processCallback = processCallback;
  }

  /**
   * Destroys the task.
   * @return {!Promise<void>}
   */
  async destroy() {
    await this._withHandle(handle => {
      this._audioNode.port.postMessage({type: 'task-destroy', handle});
    });
  }

  get duration() {
    return this._duration;
  }

  get isActive() {
    return this._isActive;
  }

  get position() {
    return this._position;
  }

  /**
   * @param {number} newDuration
   */
  set duration(newDuration) {
    if (this._duration === newDuration) return;

    this._duration = newDuration;
    this._withHandle(handle => {
      this._audioNode.port.postMessage({
        type: 'task-set-duration',
        handle,
        duration: newDuration,
      });
    });
  }

  /**
   * @param {number} newPosition
   */
  set position(newPosition) {
    if (this._position === newPosition) return;

    this._position = newPosition;
    this._withHandle(handle => {
      this._audioNode.port.postMessage({
        type: 'task-set-position',
        handle,
        position: newPosition,
      });
    });
  }

  /**
   * Helper to run a function with the resolved handle.
   * @param {function(number):void} fn
   * @return {!Promise}
   * @private
   */
  async _withHandle(fn) {
    const handle = await this._handlePromise;
    return fn(handle);
  }
}
