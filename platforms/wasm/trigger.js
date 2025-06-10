
export class Trigger {
  constructor({audioNode, handlePromise, performerHandle, position, callback}) {
    this._audioNode = audioNode;
    this._handlePromise = handlePromise;

    this._isActive = false;
    this._position = position;

    this.callback = callback;

    this._audioNode.port.postMessage({type: 'trigger-create', performerHandle, position});
  }

  async _withHandle(fn) {
    const handle = await this._handlePromise;
    return fn(handle);
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

  onProcess(position) {
    this._position = position;
    this.callback();
  }
}
