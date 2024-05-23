using System;
using System.Runtime.InteropServices;
using UnityEngine;

namespace Barely {
  /// Custom effect definition that can be used to define new effects.
  public interface CustomEffectDefinition {
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
    /// @param id Control identifier.
    /// @param value Control value.
    public void OnSetControl(int id, double value);

    /// Set data callback.
    ///
    /// @param dataPtr Pointer to data.
    /// @param size Data size in bytes.
    public void OnSetData(IntPtr data, int size);
  }

  /// Custom effect template that implements a custom effect.
  ///
  /// This template can be used to implement new effects using a `CustomEffectDefinition`.
  public abstract class CustomEffect<DefinitionType> : Effect,
                                                       Musician.Internal.CustomEffectInterface
      where DefinitionType : class, CustomEffectDefinition, new() {
    /// Returns the internal definition.
    ///
    /// @return Internal effect definition.
    public Musician.Internal.EffectDefinition GetDefinition() {
      var controlDefinitions = GetControlDefinitions();
      return new Musician.Internal.EffectDefinition() {
        createCallback = OnCreate,
        destroyCallback = OnDestroy,
        processCallback = OnProcess,
        setControlCallback = OnSetControl,
        setDataCallback = OnSetData,
        controlDefinitions = GetControlDefinitionsPtr(controlDefinitions),
        controlDefinitionCount = controlDefinitions != null ? controlDefinitions.Length : 0,
      };
    }

    /// Returns an array of control definitions.
    ///
    /// @return Array of control definitions.
    protected abstract ControlDefinition[] GetControlDefinitions();

    // Create callback.
    [AOT.MonoPInvokeCallback(typeof(Musician.Internal.EffectDefinition_CreateCallback))]
    private static void OnCreate(ref IntPtr state, Int32 frameRate) {
      GCHandle handle = GCHandle.Alloc(new DefinitionType());
      state = GCHandle.ToIntPtr(handle);
      (handle.Target as DefinitionType).OnCreate(frameRate);
    }

    // Destroy callback.
    [AOT.MonoPInvokeCallback(typeof(Musician.Internal.EffectDefinition_DestroyCallback))]
    private static void OnDestroy([In] ref IntPtr state) {
      var handle = GCHandle.FromIntPtr(state);
      (handle.Target as DefinitionType).OnDestroy();
      handle.Free();
    }

    // Process callback.
    [AOT.MonoPInvokeCallback(typeof(Musician.Internal.EffectDefinition_ProcessCallback))]
    private static void OnProcess(ref IntPtr state, IntPtr outputSamples, Int32 outputChannelCount,
                                  Int32 outputFrameCount) {
      (GCHandle.FromIntPtr(state).Target as DefinitionType)
          .OnProcess(Musician.Internal.OutputSamples, outputChannelCount, outputFrameCount);
      Marshal.Copy(Musician.Internal.OutputSamples, 0, outputSamples,
                   outputChannelCount * outputFrameCount);
    }

    // Set control callback.
    [AOT.MonoPInvokeCallback(typeof(Musician.Internal.EffectDefinition_SetControlCallback))]
    private static void OnSetControl(ref IntPtr state, Int32 id, double value) {
      (GCHandle.FromIntPtr(state).Target as DefinitionType).OnSetControl(id, value);
    }

    // Set data callback.
    [AOT.MonoPInvokeCallback(typeof(Musician.Internal.EffectDefinition_SetDataCallback))]
    private static void OnSetData(ref IntPtr state, IntPtr dataPtr, Int32 size) {
      (GCHandle.FromIntPtr(state).Target as DefinitionType).OnSetData(dataPtr, size);
    }

    // Allocates and returns a pointer to an array of control definitions.
    private IntPtr GetControlDefinitionsPtr(ControlDefinition[] definitions) {
      if (definitions == null || definitions.Length == 0) {
        return IntPtr.Zero;
      }
      IntPtr definitionsPtr =
          Marshal.AllocHGlobal(definitions.Length * Marshal.SizeOf<ControlDefinition>());
      for (int i = 0; i < definitions.Length; ++i) {
        IntPtr definitionPtr =
            new IntPtr(definitionsPtr.ToInt64() + i * Marshal.SizeOf<ControlDefinition>());
        Marshal.StructureToPtr(definitions[i], definitionPtr, false);
      }
      return definitionsPtr;
    }
  }
}  // namespace Barely
