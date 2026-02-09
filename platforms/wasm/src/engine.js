import {CommandType, MessageType} from './context.js';
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
    this._nextId = 1;

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
    const id = this._nextId++;
    const instrument = new Instrument(this, id, noteOnCallback, noteOffCallback);
    this._instruments.set(id, instrument);
    this._pushCommand({type: CommandType.INSTRUMENT_CREATE, id});
    return instrument;
  }

  /**
   * Creates a new performer.
   * @return {!Performer}
   */
  createPerformer() {
    const id = this._nextId++;
    const performer = new Performer(this, id);
    this._performers.set(id, performer);
    this._pushCommand({type: CommandType.PERFORMER_CREATE, id});
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
    const id = this._nextId++;
    const task = new Task(this, id, eventCallback);
    this._tasks.set(id, task);
    this._pushCommand({
      type: CommandType.TASK_CREATE,
      id,
      performerId: performer.id,
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
        this._instruments.get(command.id)?.noteOnCallback(command.pitch);
        break;
      case CommandType.INSTRUMENT_ON_NOTE_OFF:
        this._instruments.get(command.id)?.noteOffCallback(command.pitch);
        break;
      case CommandType.PERFORMER_GET_PROPERTIES_SUCCESS: {
        const performer = this._performers.get(command.id);
        if (performer) {
          performer._position = command.position;
        }
        break;
      }
      case CommandType.TASK_GET_PROPERTIES_SUCCESS: {
        const task = this._tasks.get(command.id);
        if (task) {
          task._isActive = command.isActive;
        }
        break;
      }
      case CommandType.TASK_ON_EVENT:
        this._tasks.get(command.id)?.eventCallback(command.eventType);
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
