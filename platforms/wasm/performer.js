export class Performer {
  constructor({audioNode, handle}) {
    this.audioNode = audioNode;
    this.handle = handle;
    this.isPlaying = false;
  }

  /**
   * @param {bool} newIsLooping
   */
  set isLooping(newIsLooping) {
    if (!this.handle) return;

    this.audioNode.port.postMessage({
      type: 'performer-set-looping',
      handle: this.handle,
      isLooping: newIsLooping,
    });
  }

  /**
   * @param {float} newLoopBeginPosition
   */
  set loopBeginPosition(newLoopBeginPosition) {
    if (!this.handle) return;

    this.audioNode.port.postMessage({
      type: 'performer-set-loop-begin-position',
      handle: this.handle,
      loopBeginPosition: newLoopBeginPosition,
    });
  }

  /**
   * @param {float} newLoopLength
   */
  set loopLength(newLoopLength) {
    if (!this.handle) return;

    this.audioNode.port.postMessage({
      type: 'performer-set-loop-begin-position',
      handle: this.handle,
      loopLength: newLoopLength,
    });
  }

  /**
   * @param {float} newPosition
   */
  set position(newPosition) {
    if (!this.handle) return;

    this.audioNode.port.postMessage({
      type: 'performer-set-position',
      handle: this.handle,
      position: newPosition,
    });
  }

  createTask(position, duration, callback) {
    if (!this.handle) return;

    // TODO(#164): need to respond to in `task-create-success`.
    this.audioNode.port.postMessage({
      type: 'task-create',
      performerHandle: this.handle,
      position: position,
      duration: duration,
      callback: callback,
    });
  }

  createTrigger(position, callback) {
    if (!this.handle) return;

    // TODO(#164): need to respond to in `trigger-create-success`.
    this.audioNode.port.postMessage({
      type: 'trigger-create',
      performerHandle: this.handle,
      position: position,
      callback: callback,
    });
  }

  start() {
    if (!this.handle) return;
    this.audioNode.port.postMessage({type: 'performer-start', handle: this.handle});
    this.isPlaying = true;
  }

  stop() {
    if (!this.handle) return;
    this.audioNode.port.postMessage({type: 'performer-stop', handle: this.handle});
    this.isPlaying = false;
  }
}
