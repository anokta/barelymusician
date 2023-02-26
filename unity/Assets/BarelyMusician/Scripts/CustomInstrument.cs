using System;
using System.Runtime.InteropServices;
using UnityEngine;

namespace Barely {
  /// Control definition.
  public struct ControlDefinition {
    /// Default value.
    public double defaultValue;

    /// Minimum value.
    public double minValue;

    /// Maximum value.
    public double maxValue;
  }

  /// Custom instrument definition.
  public interface CustomInstrumentDefinition {
    /// Creates callback.
    ///
    /// @param frame_rate Frame rate in hertz.
    public void OnCreate(int frameRate);

    /// Destroy callback.
    public void OnDestroy();

    /// Process callback.
    ///
    /// @param outputSamples Array of interleaved output samples.
    /// @param outputChannelCount Number of output channels.
    /// @param outputFrameCount Number of output frames.
    public void OnProcess(double[] outputSamples, int outputChannelCount, int outputFrameCount);

    /// Set control callback.
    ///
    /// @param index Control index.
    /// @param value Control value.
    /// @param slopePerFrame Control slope in value change per frame.
    public void OnSetControl(int index, double value, double slopePerFrame);

    /// Set data callback.
    ///
    /// @param dataPtr Pointer to data.
    /// @param size Data size in bytes.
    public void OnSetData(IntPtr data, int size);

    /// Set note control callback.
    ///
    /// @param pitch Note pitch.
    /// @param index Note control index.
    /// @param value Note control value.
    /// @param slopePerFrame Note control slope in value change per frame.
    public void OnSetNoteControl(double pitch, int index, double value, double slopePerFrame);

    /// Set note off callback.
    ///
    /// @param pitch Note pitch.
    public void OnSetNoteOff(double pitch);

    /// Set note on callback.
    ///
    /// @param pitch Note pitch.
    /// @param intensity Note intensity.
    public void OnSetNoteOn(double pitch, double intensity);
  }

  /// Custom instrument template.
  public abstract class CustomInstrument<DefinitionType>
      : Instrument, Musician.Internal.CustomInstrumentInterface
      where DefinitionType : class, CustomInstrumentDefinition, new() {
    /// Returns the definition.
    ///
    /// @return Instrument definition.
    public Musician.Internal.InstrumentDefinition GetDefinition() {
      var controlDefinitions = GetControlDefinitions();
      var noteControlDefinitions = GetNoteControlDefinitions();
      return new Musician.Internal.InstrumentDefinition() {
        createCallback = OnCreate,
        destroyCallback = OnDestroy,
        processCallback = OnProcess,
        setControlCallback = OnSetControl,
        setDataCallback = OnSetData,
        setNoteControlCallback = OnSetNoteControl,
        setNoteOffCallback = OnSetNoteOff,
        setNoteOnCallback = OnSetNoteOn,
        controlDefinitions = GetControlDefinitionsPtr(controlDefinitions),
        controlDefinitionCount = controlDefinitions != null ? controlDefinitions.Length : 0,
        noteControlDefinitions = GetControlDefinitionsPtr(noteControlDefinitions),
        noteControlDefinitionCount =
            noteControlDefinitions != null ? noteControlDefinitions.Length : 0,
      };
    }

    /// Returns an array of control definitions.
    ///
    /// @return Array of control definitions.
    protected abstract ControlDefinition[] GetControlDefinitions();

    /// Returns an array of note control definitions.
    ///
    /// @return Array of note control definitions.
    protected abstract ControlDefinition[] GetNoteControlDefinitions();

    protected override void OnEnable() {
      var config = AudioSettings.GetConfiguration();
      _outputSamples = new double[config.dspBufferSize * (int)config.speakerMode];
      base.OnEnable();
    }

    protected override void OnDisable() {
      base.OnDisable();
      _outputSamples = null;
    }

    // Create callback.
    [AOT.MonoPInvokeCallback(typeof(Musician.Internal.InstrumentDefinition_CreateCallback))]
    private static void OnCreate(ref IntPtr state, Int32 frameRate) {
      GCHandle handle = GCHandle.Alloc(new DefinitionType());
      state = GCHandle.ToIntPtr(handle);
      (handle.Target as DefinitionType).OnCreate(frameRate);
    }

    // Destroy callback.
    [AOT.MonoPInvokeCallback(typeof(Musician.Internal.InstrumentDefinition_DestroyCallback))]
    private static void OnDestroy([In] ref IntPtr state) {
      var handle = GCHandle.FromIntPtr(state);
      (handle.Target as DefinitionType).OnDestroy();
      handle.Free();
    }

    // Process callback.
    [AOT.MonoPInvokeCallback(typeof(Musician.Internal.InstrumentDefinition_ProcessCallback))]
    private static void OnProcess(ref IntPtr state, IntPtr outputSamples, Int32 outputChannelCount,
                                  Int32 outputFrameCount) {
      (GCHandle.FromIntPtr(state).Target as DefinitionType)
          .OnProcess(_outputSamples, outputChannelCount, outputFrameCount);
      Marshal.Copy(_outputSamples, 0, outputSamples, outputChannelCount * outputFrameCount);
    }

    // Set control callback.
    [AOT.MonoPInvokeCallback(typeof(Musician.Internal.InstrumentDefinition_SetControlCallback))]
    private static void OnSetControl(ref IntPtr state, Int32 index, double value,
                                     double slopePerFrame) {
      (GCHandle.FromIntPtr(state).Target as DefinitionType)
          .OnSetControl(index, value, slopePerFrame);
    }

    // Set data callback.
    [AOT.MonoPInvokeCallback(typeof(Musician.Internal.InstrumentDefinition_SetDataCallback))]
    private static void OnSetData(ref IntPtr state, IntPtr dataPtr, Int32 size) {
      (GCHandle.FromIntPtr(state).Target as DefinitionType).OnSetData(dataPtr, size);
    }

    // Set note control callback.
    [AOT.MonoPInvokeCallback(typeof(Musician.Internal.InstrumentDefinition_SetNoteControlCallback))]
    private static void OnSetNoteControl(ref IntPtr state, double pitch, Int32 index, double value,
                                         double slopePerFrame) {
      (GCHandle.FromIntPtr(state).Target as DefinitionType)
          .OnSetNoteControl(pitch, index, value, slopePerFrame);
    }

    // Set note off callback.
    [AOT.MonoPInvokeCallback(typeof(Musician.Internal.InstrumentDefinition_SetNoteOffCallback))]
    private static void OnSetNoteOff(ref IntPtr state, double pitch) {
      (GCHandle.FromIntPtr(state).Target as DefinitionType).OnSetNoteOff(pitch);
    }

    // Set note on callback.
    [AOT.MonoPInvokeCallback(typeof(Musician.Internal.InstrumentDefinition_SetNoteOnCallback))]
    private static void OnSetNoteOn(ref IntPtr state, double pitch, double intensity) {
      (GCHandle.FromIntPtr(state).Target as DefinitionType).OnSetNoteOn(pitch, intensity);
    }

    // Allocates and returns a pointer to an array of control definitions.
    private IntPtr GetControlDefinitionsPtr(ControlDefinition[] definitions) {
      if (definitions == null || definitions.Length == 0) {
        return IntPtr.Zero;
      }
      IntPtr definitionsPtr =
          Marshal.AllocHGlobal(definitions.Length * Marshal.SizeOf<ControlDefinition>());
      int byteOffset = 0;
      for (int i = 0; i < definitions.Length; ++i) {
        var defaultValueBytes = BitConverter.GetBytes(definitions[i].defaultValue);
        for (int j = 0; j < defaultValueBytes.Length; ++j) {
          Marshal.WriteByte(definitionsPtr, byteOffset++, defaultValueBytes[j]);
        }
        var minValueBytes = BitConverter.GetBytes(definitions[i].minValue);
        for (int j = 0; j < minValueBytes.Length; ++j) {
          Marshal.WriteByte(definitionsPtr, byteOffset++, minValueBytes[j]);
        }
        var maxValueBytes = BitConverter.GetBytes(definitions[i].maxValue);
        for (int j = 0; j < maxValueBytes.Length; ++j) {
          Marshal.WriteByte(definitionsPtr, byteOffset++, maxValueBytes[j]);
        }
      }
      return definitionsPtr;
    }

    // Internal output samples.
    private static double[] _outputSamples = null;
  }
}  // namespace Barely
