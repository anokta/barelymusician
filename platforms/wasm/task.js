export const TaskEventType = {
  BEGIN: 0,
  END: 1,
  COUNT: 2,
};

export class Task {
  constructor({audioNode, idPromise, position, duration, eventCallback}) {
    this._audioNode = audioNode;
    this._idPromise = idPromise;

    this._position = position;
    this._duration = duration;
    this._isActive = false;

    this.eventCallback = eventCallback;
  }

  /**
   * Destroys the task.
   * @return {!Promise<void>}
   */
  async destroy() {
    await this._withId(id => {
      this._audioNode.port.postMessage({type: 'task-destroy', id});
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
    this._withId(id => {
      this._audioNode.port.postMessage({
        type: 'task-set-duration',
        id,
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
    this._withId(id => {
      this._audioNode.port.postMessage({
        type: 'task-set-position',
        id,
        position: newPosition,
      });
    });
  }

  /**
   * Helper to run a function with the resolved id.
   * @param {function(number):void} fn
   * @return {!Promise}
   * @private
   */
  async _withId(fn) {
    const id = await this._idPromise;
    return fn(id);
  }
}
