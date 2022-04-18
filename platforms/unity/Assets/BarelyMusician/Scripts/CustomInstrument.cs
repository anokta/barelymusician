using System;
using System.Runtime.InteropServices;
using UnityEngine;

namespace Barely {
  /// Custom instrument interface.
  public abstract class CustomInstrument : Instrument {
    public InstrumentDefinition Definition { get; private set; }

    protected abstract ParameterDefinition[] GetParameterDefinitions();

    protected abstract void ProcessCallback(double[] output, int numOutputChannels);

    protected abstract void SetDataCallback(byte[] data);

    protected abstract void SetNoteOffCallback(double pitch);

    protected abstract void SetNoteOnCallback(double pitch, double intensity);

    protected abstract void SetParameterCallback(int index, double value);

    protected override void Awake() {
      base.Awake();
      _processCallback =
          delegate(IntPtr state, double[] output, Int32 numOutputChannels, Int32 numOutputFrames) {
        ProcessCallback(output, numOutputChannels);
      };
      _setDataCallback = delegate(IntPtr state, byte[] data, Int32 size) {
        SetDataCallback(data);
      };
      _setNoteOffCallback = delegate(IntPtr state, double pitch) {
        SetNoteOffCallback(pitch);
      };
      _setNoteOnCallback = delegate(IntPtr state, double pitch, double intensity) {
        SetNoteOnCallback(pitch, intensity);
      };
      _setParameterCallback = delegate(IntPtr state, Int32 index, double value) {
        SetParameter(index, value);
      };
      Definition = new InstrumentDefinition(_processCallback, _setDataCallback, _setNoteOffCallback,
                                            _setNoteOnCallback, _setParameterCallback,
                                            GetParameterDefinitions());
    }

    protected override void OnDestroy() {
      base.OnDestroy();
      _processCallback = null;
      _setDataCallback = null;
      _setNoteOffCallback = null;
      _setNoteOnCallback = null;
      _setParameterCallback = null;
    }

    private InstrumentDefinition.ProcessCallback _processCallback = null;
    private InstrumentDefinition.SetDataCallback _setDataCallback = null;
    private InstrumentDefinition.SetNoteOffCallback _setNoteOffCallback = null;
    private InstrumentDefinition.SetNoteOnCallback _setNoteOnCallback = null;
    private InstrumentDefinition.SetParameterCallback _setParameterCallback = null;
  }
}
