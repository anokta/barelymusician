import {MessageType} from './message.js'

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
 * A representation of a recurring task that can be performed by a musical
 * performer in real-time.
 */
export class Task {
  /**
   * @param {{
   *   audioNode: !AudioWorkletNode,
   *   idPromise: !Promise<number>,
   *   position: number,
   *   duration: number,
   *   priority: number,
   *   eventCallback: function(number):void
   * }} params
   */
  constructor({audioNode, idPromise, position, duration, priority, eventCallback}) {
    /** @private @const {!AudioWorkletNode} */
    this._audioNode = audioNode;

    /** @private @const {!Promise<number>} */
    this._idPromise = idPromise;

    /** @private {boolean} */
    this._isDestroyed = false;

    /** @private {number} */
    this._position = position;

    /** @private {number} */
    this._duration = duration;

    /** @private {number} */
    this._priority = priority;

    /** @private {boolean} */
    this._isActive = false;

    /** @public */
    this.eventCallback = eventCallback;
  }

  /**
   * Destroys the task.
   * @return {!Promise<void>}
   */
  async destroy() {
    if (this._isDestroyed) return;

    this._isDestroyed = true;
    await this._withId(id => {
      this._audioNode.port.postMessage({
        type: MessageType.TASK_DESTROY,
        id,
      });
    });
  }

  /** @param {number} newDuration */
  set duration(newDuration) {
    if (this._duration === newDuration) return;

    this._duration = newDuration;
    this._withId(id => {
      this._audioNode.port.postMessage({
        type: MessageType.TASK_SET_DURATION,
        id,
        duration: newDuration,
      });
    });
  }

  /** @param {number} newPosition */
  set position(newPosition) {
    if (this._position === newPosition) return;

    this._position = newPosition;
    this._withId(id => {
      this._audioNode.port.postMessage({
        type: MessageType.TASK_SET_POSITION,
        id,
        position: newPosition,
      });
    });
  }

  /** @param {number} newPriority */
  set priority(newPriority) {
    if (this._priority === newPriority) return;

    this._priority = newPriority;
    this._withId(id => {
      this._audioNode.port.postMessage({
        type: MessageType.TASK_SET_PRIORITY,
        id,
        priority: newPriority,
      });
    });
  }

  /** @return {number} */
  get duration() {
    return this._duration;
  }

  /** @return {!Promise<void>} */
  get id() {
    return this._idPromise;
  }

  /** @return {boolean} */
  get isActive() {
    return this._isActive;
  }

  /** @return {number} */
  get position() {
    return this._position;
  }

  /** @return {number} */
  get priority() {
    return this._priority;
  }

  /**
   * @param {function(number):(void|!Promise<void>)} fn
   * @return {!Promise<void>}
   * @private
   */
  async _withId(fn) {
    if (this._isDestroyed) return;
    const id = await this._idPromise;
    await fn(id);
  }
}
