import {Task} from './task.js'
import {Trigger} from './trigger.js'

export class Performer {
  constructor({audioNode, handlePromise}) {
    this._audioNode = audioNode;
    this._handlePromise = handlePromise;

    this._isLooping = false;
    this._isPlaying = false;
    this._loopBeginPosition = 0.0;
    this._loopLength = 1.0;
    this._position = 0.0;

    this._pendingTasks = [];
    this._pendingTriggers = [];

    this._audioNode.port.postMessage({type: 'performer-create'});
  }

  async _withHandle(fn) {
    const handle = await this._handlePromise;
    return fn(handle);
  }

  get isLooping() {
    return this._isLooping;
  }

  get isPlaying() {
    return this._isPlaying;
  }

  get loopBeginPosition() {
    return this._loopBeginPosition;
  }

  get loopLength() {
    return this._loopLength;
  }

  get position() {
    return this._position;
  }

  /**
   * @param {bool} newIsLooping
   */
  set isLooping(newIsLooping) {
    if (this._isLooping == newIsLooping) return;

    // TODO(#164): Revisit how the properties are set when they are set (immediate vs deferred).
    this._isLooping = newIsLooping;
    this._withHandle((handle) => {
      this._audioNode.port.postMessage({
        type: 'performer-set-looping',
        handle: handle,
        isLooping: newIsLooping,
      });
    });
  }

  /**
   * @param {float} newLoopBeginPosition
   */
  set loopBeginPosition(newLoopBeginPosition) {
    if (this._loopBeginPosition == newLoopBeginPosition) return;

    this._loopBeginPosition = newLoopBeginPosition;
    this._withHandle((handle) => {
      this._audioNode.port.postMessage({
        type: 'performer-set-loop-begin-position',
        handle: handle,
        loopBeginPosition: newLoopBeginPosition,
      });
    });
  }

  /**
   * @param {float} newLoopLength
   */
  set loopLength(newLoopLength) {
    if (this._loopLength == newLoopLength) return;

    this._loopLength = newLoopLength;
    this._withHandle((handle) => {
      this._audioNode.port.postMessage({
        type: 'performer-set-loop-begin-position',
        handle: handle,
        loopLength: newLoopLength,
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
        type: 'performer-set-position',
        handle: handle,
        position: newPosition,
      });
    });
  }

  createTask(position, duration, processCallback) {
    this._withHandle((handle) => {
      let resolveHandle;
      const handlePromise = new Promise(resolve => {
        resolveHandle = resolve;
      });
      const task = new Task({
        audioNode: this._audioNode,
        handlePromise: handlePromise,
        performerHandle: handle,
        position,
        duration,
        processCallback,
      });

      this._pendingTasks.push({task, resolveHandle});
    });
  }

  createTrigger(position, processCallback) {
    this._withHandle((handle) => {
      let resolveHandle;
      const handlePromise = new Promise(resolve => {
        resolveHandle = resolve;
      });
      const trigger = new Trigger({
        audioNode: this._audioNode,
        handlePromise: handlePromise,
        performerHandle: handle,
        position,
        processCallback,
      });

      this._pendingTriggers.push({trigger, resolveHandle});
    });
  }

  onTaskCreateSuccess(handle) {
    const {task, resolveHandle} = this._pendingTasks.shift();
    resolveHandle(handle);
    return task;
  }

  onTriggerCreateSuccess(handle) {
    const {trigger, resolveHandle} = this._pendingTriggers.shift();
    resolveHandle(handle);
    return trigger;
  }

  start() {
    if (this._isPlaying) return;

    this._isPlaying = true;
    this._withHandle((handle) => {
      this._audioNode.port.postMessage({type: 'performer-start', handle: handle});
    });
  }

  stop() {
    if (!this._isPlaying) return;

    this._isPlaying = false;
    this._withHandle((handle) => {
      this._audioNode.port.postMessage({type: 'performer-stop', handle: handle});
    });
  }
}
