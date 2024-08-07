using System;
using System.Runtime.InteropServices;
using UnityEngine;

namespace Barely {
  /// Custom instrument definition that can be used to define new instruments.
  public interface CustomInstrumentDefinition {
    /// Create callback.
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
    /// @param controlId Control identifier.
    /// @param value Control value.
    public void OnSetControl(int controlId, double value);

    /// Set data callback.
    ///
    /// @param dataPtr Pointer to data.
    /// @param size Data size in bytes.
    public void OnSetData(IntPtr data, int size);

    /// Set note control callback.
    ///
    /// @param pitch Note pitch.
    /// @param controlId Note control identifier.
    /// @param value Note control value.
    public void OnSetNoteControl(double pitch, int controlId, double value);

    /// Set note off callback.
    ///
    /// @param pitch Note pitch.
    public void OnSetNoteOff(double pitch);

    /// Set note on callback.
    ///
    /// @param noteId Note identifier.
    /// @param pitch Note pitch.
    /// @param intensity Note intensity.
    public void OnSetNoteOn(int noteId, double pitch, double intensity);
  }

  /// Custom instrument template that implements a custom instrument.
  ///
  /// This template can be used to implement new instruments using a `CustomInstrumentDefinition`.
  public abstract class CustomInstrument<DefinitionType>
      : Instrument, Musician.Internal.CustomInstrumentInterface
      where DefinitionType : class, CustomInstrumentDefinition, new() {
    /// Returns the internal definition.
    ///
    /// @return Internal instrument definition.
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
    protected abstract Control.Definition[] GetControlDefinitions();

    /// Returns an array of note control definitions.
    ///
    /// @return Array of note control definitions.
    protected abstract Control.Definition[] GetNoteControlDefinitions();

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
          .OnProcess(Musician.Internal.OutputSamples, outputChannelCount, outputFrameCount);
      Marshal.Copy(Musician.Internal.OutputSamples, 0, outputSamples,
                   outputChannelCount * outputFrameCount);
    }

    // Set control callback.
    [AOT.MonoPInvokeCallback(typeof(Musician.Internal.InstrumentDefinition_SetControlCallback))]
    private static void OnSetControl(ref IntPtr state, Int32 controlId, double value) {
      (GCHandle.FromIntPtr(state).Target as DefinitionType).OnSetControl(controlId, value);
    }

    // Set data callback.
    [AOT.MonoPInvokeCallback(typeof(Musician.Internal.InstrumentDefinition_SetDataCallback))]
    private static void OnSetData(ref IntPtr state, IntPtr dataPtr, Int32 size) {
      (GCHandle.FromIntPtr(state).Target as DefinitionType).OnSetData(dataPtr, size);
    }

    // Set note control callback.
    [AOT.MonoPInvokeCallback(typeof(Musician.Internal.InstrumentDefinition_SetNoteControlCallback))]
    private static void OnSetNoteControl(ref IntPtr state, double pitch, Int32 controlId,
                                         double value) {
      (GCHandle.FromIntPtr(state).Target as DefinitionType)
          .OnSetNoteControl(pitch, controlId, value);
    }

    // Set note off callback.
    [AOT.MonoPInvokeCallback(typeof(Musician.Internal.InstrumentDefinition_SetNoteOffCallback))]
    private static void OnSetNoteOff(ref IntPtr state, double pitch) {
      (GCHandle.FromIntPtr(state).Target as DefinitionType).OnSetNoteOff(pitch);
    }

    // Set note on callback.
    [AOT.MonoPInvokeCallback(typeof(Musician.Internal.InstrumentDefinition_SetNoteOnCallback))]
    private static void OnSetNoteOn(ref IntPtr state, Int32 noteId, double pitch,
                                    double intensity) {
      (GCHandle.FromIntPtr(state).Target as DefinitionType).OnSetNoteOn(noteId, pitch, intensity);
    }

    // Allocates and returns a pointer to an array of control definitions.
    private IntPtr GetControlDefinitionsPtr(Control.Definition[] definitions) {
      if (definitions == null || definitions.Length == 0) {
        return IntPtr.Zero;
      }
      IntPtr definitionsPtr =
          Marshal.AllocHGlobal(definitions.Length * Marshal.SizeOf<Control.Definition>());
      for (int i = 0; i < definitions.Length; ++i) {
        IntPtr definitionPtr =
            new IntPtr(definitionsPtr.ToInt64() + i * Marshal.SizeOf<Control.Definition>());
        Marshal.StructureToPtr(definitions[i], definitionPtr, false);
      }
      return definitionsPtr;
    }
  }
}  // namespace Barely
