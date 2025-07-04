
export class Trigger {
  constructor({audioNode, handlePromise, position, processCallback}) {
    this._audioNode = audioNode;
    this._handlePromise = handlePromise;

    this._isActive = false;
    this._position = position;

    this.processCallback = processCallback;
  }

  /**
   * Destroys the task.
   */
  async destroy() {
    await this._withHandle((handle) => {
      this._audioNode.port.postMessage({type: 'trigger-destroy', handle});
    });
  }

  get position() {
    return this._position;
  }

  /**
   * @param {float} newPosition
   */
  set position(newPosition) {
    if (this._position == newPosition) return;

    this._position = newPosition;
    this._withHandle((handle) => {
      this._audioNode.port.postMessage({
        type: 'trigger-set-position',
        handle: handle,
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
