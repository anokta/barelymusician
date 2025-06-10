export const TaskState = {
  BEGIN: 0,
  END: 1,
  UPDATE: 2,
  COUNT: 3,
};

export class Task {
  constructor({audioNode, handlePromise, performerHandle, position, duration, callback}) {
    this._audioNode = audioNode;
    this._handlePromise = handlePromise;

    this._position = position;
    this._duration = duration;
    this._isActive = false;

    this.callback = callback;

    this._audioNode.port.postMessage({type: 'task-create', performerHandle, position, duration});
  }

  async _withHandle(fn) {
    const handle = await this._handlePromise;
    return fn(handle);
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
   * @param {float} newDuration
   */
  set duration(newDuration) {
    if (this._duration == newDuration) return;

    this._duration = newDuration;
    this._withHandle((handle) => {
      this._audioNode.port.postMessage({
        type: 'task-set-duration',
        handle: handle,
        duration: newDuration,
      });
    });
  }

  /**
   * @param {float} newPosition
   */
  set position(newPosition) {
    if (this._position == newPosition) return;

    this._position = newPosition;
    this._withHandle((handle) => {
      this._audioNode.port.postMessage({
        type: 'task-set-position',
        handle: handle,
        position: newPosition,
      });
    });
  }

  onProcess(position, state) {
    this._position = position;
    this.callback(state);
  }
}
