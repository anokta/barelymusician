import {CommandType, MessageType} from './command.js';
import {Instrument} from './instrument.js';
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
    /** @private {!AudioContext} */
    this._audioContext = audioContext;

    /** @private {!Map<number, !Instrument>} */
    this._instruments = new Map();

    /** @private {!Map<number, !Performer>} */
    this._performers = new Map();

    /** @private {!Map<number, !Task>} */
    this._tasks = new Map();

    /** @private {number} */
    this._nextHandle = 1;

    /** @private {!Array<!Object>} */
    this._pendingCommands = [];

    /** @private {!AudioWorkletNode} */
    this._audioNode = this._createAudioNode(audioContext);

    this._audioNode.port.onmessage = event => {
      if (!event.data) return;
      switch (event.data.type) {
        case MessageType.INIT_SUCCESS:
          initCallback();
          break;
        case MessageType.UPDATE_SUCCESS:
          for (const {handle, position} of event.data.performer_properties) {
            const performer = this._performers.get(handle);
            if (performer) {
              performer._position = position;
            }
          }
          for (const {handle, isActive} of event.data.task_properties) {
            const task = this._tasks.get(handle);
            if (task) {
              task._isActive = isActive;
            }
          }
          for (const command of event.data.commands) {
            this._processCommand(command);
          }
          break;
        default:
          console.error(`Invalid message: ${event.data.type}`);
      };
    }
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
    const handle = this._nextHandle++;
    const instrument = new Instrument(this, handle, noteOnCallback, noteOffCallback);
    this._instruments.set(handle, instrument);
    this._pushCommand({type: CommandType.INSTRUMENT_CREATE, handle});
    return instrument;
  }

  /**
   * Creates a new performer.
   * @return {!Performer}
   */
  createPerformer() {
    const handle = this._nextHandle++;
    const performer = new Performer(this, handle);
    this._performers.set(handle, performer);
    this._pushCommand({type: CommandType.PERFORMER_CREATE, handle});
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
    const handle = this._nextHandle++;
    const task = new Task(this, handle, eventCallback);
    this._tasks.set(handle, task);
    this._pushCommand({
      type: CommandType.TASK_CREATE,
      handle,
      performerHandle: performer.handle,
      position,
      duration,
      priority,
    });
    return task;
  }

  /**
   * Sets a control value.
   * @param {number} typeIndex
   * @param {number} value
   */
  setControl(typeIndex, value) {
    this._pushCommand({type: CommandType.ENGINE_SET_CONTROL, typeIndex, value});
  }

  /**
   * Updates the internal state.
   */
  update() {
    this._audioNode.port.postMessage({type: MessageType.UPDATE, commands: this._pendingCommands});
    this._pendingCommands = [];
  }

  /** @param {number} tempo */
  setTempo(tempo) {
    this._pushCommand({type: CommandType.ENGINE_SET_TEMPO, tempo});
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
   * Processes a command.
   * @param {!Object} command
   * @private
   */
  _processCommand(command) {
    switch (command.type) {
      case CommandType.INSTRUMENT_ON_NOTE_ON:
        this._instruments.get(command.handle)?.noteOnCallback(command.pitch);
        break;
      case CommandType.INSTRUMENT_ON_NOTE_OFF:
        this._instruments.get(command.handle)?.noteOffCallback(command.pitch);
        break;
      default:
        console.error(`Invalid command: ${command.type}`);
    }
  }

  /**
   * @param {!Object} command
   * @private
   */
  _pushCommand(command) {
    this._pendingCommands.push(command);
  }
}
