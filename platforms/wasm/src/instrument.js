import {CommandType} from './context.js'

/**
 * A representation of a musical instrument that can be played in real-time.
 */
export class Instrument {
  /**
   * @param {!Engine} engine
   * @param {number} handle
   * @param {function(number):void} noteOnCallback
   * @param {function(number):void} noteOffCallback
   */
  constructor(engine, handle, noteOnCallback, noteOffCallback) {
    /** @private @const {!Engine} */
    this._engine = engine;

    /** @private @const {number} */
    this._handle = handle;

    /** @public */
    this.noteOnCallback = noteOnCallback;

    /** @public */
    this.noteOffCallback = noteOffCallback;
  }

  /**
   * Destroys the instrument.
   */
  destroy() {
    this._engine._instruments.delete(this._handle);
    this._engine._pushCommand({type: CommandType.INSTRUMENT_DESTROY, handle: this._handle});
  }

  /**
   * Sets all notes off.
   */
  setAllNotesOff() {
    this._engine._pushCommand(
        {type: CommandType.INSTRUMENT_SET_ALL_NOTES_OFF, handle: this._handle});
  }

  /**
   * Sets a control value.
   * @param {number} typeIndex
   * @param {number} value
   */
  setControl(typeIndex, value) {
    this._engine._pushCommand(
        {type: CommandType.INSTRUMENT_SET_CONTROL, handle: this._handle, typeIndex, value});
  }

  /**
   * Sets a note control value.
   * @param {number} pitch
   * @param {number} typeIndex
   * @param {number} value
   */
  setNoteControl(pitch, typeIndex, value) {
    this._engine._pushCommand({
      type: CommandType.INSTRUMENT_SET_NOTE_CONTROL,
      handle: this._handle,
      pitch,
      typeIndex,
      value
    });
  }

  /**
   * Sets a note off.
   * @param {number} pitch
   */
  setNoteOff(pitch) {
    this._engine._pushCommand(
        {type: CommandType.INSTRUMENT_SET_NOTE_OFF, handle: this._handle, pitch});
  }

  /**
   * Sets a note on.
   * @param {number} pitch
   * @param {number=} gain
   * @param {number=} pitchShift
   */
  setNoteOn(pitch, gain = 1.0, pitchShift = 0.0) {
    this._engine._pushCommand(
        {type: CommandType.INSTRUMENT_SET_NOTE_ON, handle: this._handle, pitch, gain, pitchShift});
  }

  /**
   * Loads and assigns sample data to the instrument.
   * @param {!Array<{pitch: number, url: string}>} sampleData
   * @return {!Promise<void>}
   */
  async setSampleData(sampleData) {
    const slices = [];

    for (const {pitch, url} of sampleData) {
      const response = await fetch(url);
      if (!response.ok) {
        console.warn(`Invalid sample data for pitch ${pitch}`);
        continue;
      }

      const arrayBuffer = await response.arrayBuffer();
      const audioBuffer = await this._engine._audioContext.decodeAudioData(arrayBuffer);

      slices.push({
        rootPitch: pitch,
        sampleRate: audioBuffer.sampleRate,
        samples: audioBuffer.getChannelData(0),  // mono only
      });
    }

    this._engine._pushCommand(
        {type: CommandType.INSTRUMENT_SET_SAMPLE_DATA, handle: this._handle, slices});
  }

  /** @return {number} */
  get handle() {
    return this._handle;
  }
}
