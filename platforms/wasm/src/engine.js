import {Instrument} from './instrument.js';
import {MessageType} from './message.js';
import {Performer} from './performer.js';
import {Task} from './task.js';

/**
 * A representation of an engine that governs all musical components.
 */
export class Engine {
  /**
   * @param {!AudioContext} audioContext
   * @param {function():void} initCallback
   */
  constructor(audioContext, initCallback) {
    /** @private {!Map<number, !Instrument>} */
    this._instruments = new Map();

    /** @private {!Map<number, !Performer>} */
    this._performers = new Map();

    /** @private {!Map<number, !Task>} */
    this._tasks = new Map();

    /** @private {number} */
    this._nextRequestId = 0;

    /**
     * @private {!Map<number, {resolveId:function(number):void, instrument?:!Instrument,
     *     performer?:!Performer, task?:!Task}>}
     */
    this._pendingRequests = new Map();

    /** @private {!AudioContext} */
    this._audioContext = audioContext;

    /** @private {!AudioWorkletNode} */
    this._audioNode = this._createAudioNode(audioContext);

    this._bindMessageHandlers(initCallback);
  }

  /**
   * Creates a new instrument.
   * @param {{
   *   noteOnCallback: (function(number):void),
   *   noteOffCallback: (function(number):void)
   * }=} params
   * @return {!Instrument}
   */
  createInstrument({noteOnCallback = () => {}, noteOffCallback = () => {}} = {}) {
    let resolveId;
    const idPromise = new Promise(resolve => (resolveId = resolve));

    const instrument = new Instrument({
      audioContext: this._audioContext,
      audioNode: this._audioNode,
      idPromise,
      noteOnCallback,
      noteOffCallback,
    });

    const requestId = this._nextRequestId++;
    this._pendingRequests.set(requestId, {resolveId, instrument});
    this._audioNode.port.postMessage({type: MessageType.INSTRUMENT_CREATE, requestId});

    return instrument;
  }

  /**
   * Creates a new performer.
   * @return {!Performer}
   */
  createPerformer() {
    let resolveId;
    const idPromise = new Promise(resolve => (resolveId = resolve));

    const performer = new Performer({audioNode: this._audioNode, idPromise});

    const requestId = this._nextRequestId++;
    this._pendingRequests.set(requestId, {resolveId, performer});
    this._audioNode.port.postMessage({type: MessageType.PERFORMER_CREATE, requestId});

    return performer;
  }

  /**
   * Creates a new task.
   * @param {!Performer} performer
   * @param {number} position
   * @param {number} duration
   * @param {function(number):void} eventCallback
   * @param {number=} priority
   * @return {!Task}
   */
  createTask(performer, position, duration, eventCallback, priority = 0) {
    let resolveId;
    const idPromise = new Promise(resolve => (resolveId = resolve));

    const task = new Task({
      audioNode: this._audioNode,
      idPromise,
      position,
      duration,
      priority,
      eventCallback,
    });

    const requestId = this._nextRequestId++;
    this._pendingRequests.set(requestId, {resolveId, task});
    performer.id.then(performerId => {
      this._audioNode.port.postMessage({
        type: MessageType.TASK_CREATE,
        requestId,
        performerId,
        position,
        duration,
        priority,
      });
    });

    return task;
  }

  /**
   * Sets a control value.
   * @param {number} typeIndex
   * @param {number} value
   */
  setControl(typeIndex, value) {
    this._audioNode.port.postMessage({
      type: MessageType.ENGINE_SET_CONTROL,
      typeIndex,
      value,
    });
  }

  /**
   * Updates the internal state.
   */
  update() {
    this._audioNode.port.postMessage({type: MessageType.ENGINE_UPDATE});
  }

  /** @param {number} tempo */
  setTempo(tempo) {
    this._audioNode.port.postMessage({
      type: MessageType.ENGINE_SET_TEMPO,
      tempo,
    });
  }

  /** @return {!AudioWorkletNode} */
  get audioNode() {
    return this._audioNode;
  }

  /**
   * @param {!AudioContext} audioContext
   * @return {!AudioWorkletNode}
   * @private
   */
  _createAudioNode(audioContext) {
    const STEREO_CHANNEL_COUNT = 2;

    const node = new AudioWorkletNode(audioContext, 'barelymusician-processor', {
      numberOfInputs: 0,
      numberOfOutputs: 1,
      outputChannelCount: [STEREO_CHANNEL_COUNT],
      channelCount: STEREO_CHANNEL_COUNT,
      channelCountMode: 'explicit',
    });

    node.connect(audioContext.destination);
    return node;
  }

  /**
   * @param {function():void} initCallback
   * @private
   */
  _bindMessageHandlers(initCallback) {
    this._audioNode.port.onmessage = event => {
      const data = event.data;
      if (!data?.type) return;

      switch (data.type) {
        case MessageType.INIT_SUCCESS:
          initCallback();
          break;
        case MessageType.INSTRUMENT_CREATE_SUCCESS: {
          const {resolveId, instrument} = this._pendingRequests.get(data.requestId);
          resolveId(data.id);
          this._pendingRequests.delete(data.requestId);
          this._instruments.set(data.id, instrument);
          break;
        }
        case MessageType.INSTRUMENT_DESTROY_SUCCESS:
          this._instruments.delete(data.id);
          break;
        case MessageType.INSTRUMENT_ON_NOTE_ON:
          this._instruments.get(data.id)?.noteOnCallback(data.pitch);
          break;
        case MessageType.INSTRUMENT_ON_NOTE_OFF:
          this._instruments.get(data.id)?.noteOffCallback(data.pitch);
          break;
        case MessageType.PERFORMER_CREATE_SUCCESS: {
          const {resolveId, performer} = this._pendingRequests.get(data.requestId);
          this._pendingRequests.delete(data.requestId);
          resolveId(data.id);
          this._performers.set(data.id, performer);
          break;
        }
        case MessageType.PERFORMER_DESTROY_SUCCESS:
          this._performers.delete(data.id);
          break;
        case MessageType.PERFORMER_GET_PROPERTIES_SUCCESS: {
          const performer = this._performers.get(data.id);
          if (performer) {
            performer._position = data.position;
          }
          break;
        }
        case MessageType.TASK_CREATE_SUCCESS: {
          const {resolveId, task} = this._pendingRequests.get(data.requestId);
          this._pendingRequests.delete(data.requestId);
          resolveId(data.id);
          this._tasks.set(data.id, task);
          break;
        }
        case MessageType.TASK_DESTROY_SUCCESS:
          this._tasks.delete(data.id);
          break;
        case MessageType.TASK_GET_PROPERTIES_SUCCESS: {
          const task = this._tasks.get(data.id);
          if (task) {
            task._isActive = data.isActive;
          }
          break;
        }
        case MessageType.TASK_ON_EVENT:
          this._tasks.get(data.id)?.eventCallback(data.eventType);
          break;
      }
    };
  }
}
