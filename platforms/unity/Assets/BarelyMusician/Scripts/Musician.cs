using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using UnityEngine;

namespace Barely {
  /// A representation of a musician that governs the tempo for all musical components.
  public static class Musician {
    /// Tempo in beats per minute.
    public static double Tempo {
      get { return _tempo; }
      set {
        if (_tempo != value) {
          Internal.Musician_SetTempo(value);
          _tempo = Internal.Musician_GetTempo();
        }
      }
    }
    private static double _tempo = 120.0;

    /// Timestamp in seconds.
    public static double Timestamp {
      get { return Internal.Musician_GetTimestamp(); }
    }

    /// Returns the corresponding number of beats for a given number of `seconds`.
    ///
    /// @param seconds Number of seconds.
    /// @return Number of beats.
    public static double GetBeatsFromSeconds(double seconds) {
      return Internal.Musician_GetBeatsFromSeconds(seconds);
    }

    /// Returns the corresponding number of seconds for a given number of `beats`.
    ///
    /// @param beats Number of beats.
    /// @return Number of seconds.
    public static double GetSecondsFromBeats(double beats) {
      return Internal.Musician_GetSecondsFromBeats(beats);
    }

    /// Schedules a task at a specific time.
    ///
    /// @param callback Task process callback.
    /// @param dspTime Time in seconds.
    public static void ScheduleTask(Action callback, double dspTime) {
      Internal.Musician_ScheduleTask(callback, dspTime);
    }

    /// Class that wraps the internal api.
    public static class Internal {
      /// Creates a new component.
      ///
      /// @param component Component.
      /// @param componentHandle Component handle.
      public static void Component_Create(Component component, ref IntPtr componentHandle) {
        if (Handle == IntPtr.Zero || componentHandle != IntPtr.Zero) {
          return;
        }
        switch (component) {
          case Arpeggiator arpeggiator:
            if (BarelyArpeggiator_Create(_handle, arpeggiator.ProcessOrder, ref componentHandle)) {
              return;
            }
            break;
          case Repeater repeater:
            if (BarelyRepeater_Create(_handle, repeater.ProcessOrder, ref componentHandle)) {
              return;
            }
            break;
          default:
            Debug.LogError("Unsupported component type: " + component.GetType());
            return;
        }
        Debug.LogError("Failed to create component '" + component.name + "'");
      }

      /// Destroys a component.
      ///
      /// @param component Component.
      /// @param componentHandle Component handle.
      public static void Component_Destroy(Component component, ref IntPtr componentHandle) {
        if (Handle == IntPtr.Zero || componentHandle == IntPtr.Zero) {
          return;
        }
        bool success = true;
        switch (component) {
          case Arpeggiator arpeggiator:
            success = BarelyArpeggiator_Destroy(componentHandle);
            break;
          case Repeater repeater:
            success = BarelyRepeater_Destroy(componentHandle);
            break;
          default:
            Debug.LogError("Unsupported component type: " + component.GetType());
            return;
        }
        if (!success) {
          Debug.LogError("Failed to destroy component '" + component.name + "'");
        }
        componentHandle = IntPtr.Zero;
      }

      /// Creates a new instrument.
      ///
      /// @param instrument Instrument.
      /// @param instrumentHandle Instrument handle.
      public static void Instrument_Create(Instrument instrument, ref IntPtr instrumentHandle) {
        if (Handle == IntPtr.Zero || instrumentHandle != IntPtr.Zero) {
          return;
        }
        bool success = BarelyMusician_AddInstrument(Handle, ref instrumentHandle);
        if (!success) {
          Debug.LogError("Failed to create instrument '" + instrument.name + "'");
          return;
        }
        GCHandle noteOffEventHandle = GCHandle.Alloc(instrument);
        BarelyInstrument_SetNoteOffEvent(instrumentHandle, _noteOffEventDefinition,
                                         GCHandle.ToIntPtr(noteOffEventHandle));
        GCHandle noteOnEventHandle = GCHandle.Alloc(instrument);
        BarelyInstrument_SetNoteOnEvent(instrumentHandle, _noteOnEventDefinition,
                                        GCHandle.ToIntPtr(noteOnEventHandle));
      }

      /// Destroys an instrument.
      ///
      /// @param instrumentHandle Instrument handle.
      public static void Instrument_Destroy(ref IntPtr instrumentHandle) {
        if (_handle == IntPtr.Zero || instrumentHandle == IntPtr.Zero) {
          instrumentHandle = IntPtr.Zero;
          return;
        }
        if (!BarelyMusician_RemoveInstrument(_handle, instrumentHandle)) {
          Debug.LogError("Failed to destroy instrument");
        }
        instrumentHandle = IntPtr.Zero;
      }

      /// Returns the value of an instrument control.
      ///
      /// @param instrumentHandle Instrument handle.
      /// @param index Control index.
      /// @return Control value.
      public static double Instrument_GetControl(IntPtr instrumentHandle, int index) {
        double value = 0.0;
        if (!BarelyInstrument_GetControl(instrumentHandle, index, ref value) &&
            instrumentHandle != IntPtr.Zero) {
          Debug.LogError("Failed to get instrument control " + index);
        }
        return value;
      }

      /// Returns the value of an instrument note control.
      ///
      /// @param instrumentHandle Instrument handle.
      /// @param pitch Note pitch.
      /// @param index Note control index.
      /// @return Note control value.
      public static double Instrument_GetNoteControl(IntPtr instrumentHandle, double pitch,
                                                     int index) {
        double value = 0.0;
        if (!BarelyInstrument_GetNoteControl(instrumentHandle, pitch, index, ref value) &&
            instrumentHandle != IntPtr.Zero) {
          Debug.LogError("Failed to get instrument note " + pitch + " control " + index + " value");
        }
        return value;
      }

      /// Returns whether an instrument note is on or not.
      ///
      /// @param instrumentHandle Instrument handle.
      /// @param pitch Note pitch.
      /// @return True if on, false otherwise.
      public static bool Instrument_IsNoteOn(IntPtr instrumentHandle, double pitch) {
        bool isNoteOn = false;
        if (!BarelyInstrument_IsNoteOn(instrumentHandle, pitch, ref isNoteOn) &&
            instrumentHandle != IntPtr.Zero) {
          Debug.LogError("Failed to get if instrument note " + pitch + " is on");
        }
        return isNoteOn;
      }

      /// Processes instrument output samples.
      ///
      /// @param instrumentHandle Instrument handle.
      /// @param outputSamples Output samples.
      /// @param outputChannelCount Number of output channels.
      public static void Instrument_Process(IntPtr instrumentHandle, float[] outputSamples,
                                            int outputChannelCount) {
        if (_handle == IntPtr.Zero) {
          for (int i = 0; i < outputSamples.Length; ++i) {
            outputSamples[i] = 0.0f;
          }
          return;
        }
        if (BarelyInstrument_Process(instrumentHandle, OutputSamples, outputChannelCount,
                                     outputSamples.Length / outputChannelCount,
                                     AudioSettings.dspTime)) {
          for (int i = 0; i < outputSamples.Length; ++i) {
            outputSamples[i] *= (float)OutputSamples[i];
          }
        } else {
          for (int i = 0; i < outputSamples.Length; ++i) {
            outputSamples[i] = 0.0f;
          }
        }
      }

      /// Resets an instrument control value.
      ///
      /// @param instrumentHandle Instrument handle.
      /// @param controlId Control index.
      public static void Instrument_ResetControl(IntPtr instrumentHandle, int controlId) {
        if (!BarelyInstrument_ResetControl(instrumentHandle, controlId) &&
            instrumentHandle != IntPtr.Zero) {
          Debug.LogError("Failed to reset instrument control " + controlId + " value");
        }
      }

      /// Resets an instrument note control value.
      ///
      /// @param instrumentHandle Instrument handle.
      /// @param pitch Note pitch.
      /// @param controlId Note control index.
      public static void Instrument_ResetNoteControl(IntPtr instrumentHandle, double pitch,
                                                     int controlId) {
        if (!BarelyInstrument_ResetNoteControl(instrumentHandle, pitch, controlId) &&
            instrumentHandle != IntPtr.Zero) {
          Debug.LogError("Failed to reset instrument note " + pitch + " control " + controlId +
                         " value");
        }
      }

      /// Sets all instrument notes off.
      ///
      /// @param instrumentHandle Instrument handle.
      public static void Instrument_SetAllNotesOff(IntPtr instrumentHandle) {
        if (!BarelyInstrument_SetAllNotesOff(instrumentHandle) && instrumentHandle != IntPtr.Zero) {
          Debug.LogError("Failed to stop all instrument notes");
        }
      }

      /// Sets an instrument control value.
      ///
      /// @param instrumentHandle Instrument handle.
      /// @param controlId Control index.
      /// @param value Control value.
      public static void Instrument_SetControl(IntPtr instrumentHandle, int controlId,
                                               double value) {
        if (!BarelyInstrument_SetControl(instrumentHandle, controlId, value) &&
            instrumentHandle != IntPtr.Zero) {
          Debug.LogError("Failed to set instrument control " + controlId + " value to " + value);
        }
      }

      /// Sets instrument data.
      ///
      /// @param instrumentHandle Instrument handle.
      /// @param samplers List of samplers.
      public static void Instrument_SetSampleData(IntPtr instrumentHandle,
                                                  List<Instrument.Sampler> samplers) {
        SampleDataDefinition[] definitions = null;
        if (samplers.Count > 0) {
          definitions = new SampleDataDefinition[samplers.Count];
          for (int i = 0; i < definitions.Length; ++i) {
            definitions[i] = new SampleDataDefinition() {
              rootPitch = samplers[i].RootPitch / 12.0,
              sampleRate = (samplers[i].Sample != null) ? samplers[i].Sample.frequency : 0,
              samples = samplers[i].Data,
              sampleCount = (samplers[i].Sample != null) ? samplers[i].Sample.samples : 0,
            };
          }
        }
        if (!BarelyInstrument_SetSampleData(instrumentHandle, definitions, samplers.Count) &&
            instrumentHandle != IntPtr.Zero) {
          Debug.LogError("Failed to set instrument sample data");
        }
      }

      /// Sets an instrument note control value.
      ///
      /// @param instrumentHandle Instrument handle.
      /// @param pitch Note pitch.
      /// @param controlId Note control index.
      /// @param value Note control value.
      public static void Instrument_SetNoteControl(IntPtr instrumentHandle, double pitch,
                                                   int controlId, double value) {
        if (!BarelyInstrument_SetNoteControl(instrumentHandle, pitch, controlId, value) &&
            instrumentHandle != IntPtr.Zero) {
          Debug.LogError("Failed to set instrument note " + pitch + " control " + controlId +
                         " value to " + value);
        }
      }

      /// Sets an instrument note off.
      ///
      /// @param instrumentHandle Instrument handle.
      /// @param pitch Note pitch.
      public static void Instrument_SetNoteOff(IntPtr instrumentHandle, double pitch) {
        if (!BarelyInstrument_SetNoteOff(instrumentHandle, pitch) &&
            instrumentHandle != IntPtr.Zero) {
          Debug.LogError("Failed to stop instrument note " + pitch + "");
        }
      }

      /// Sets an instrument note on.
      ///
      /// @param instrumentHandle Instrument handle.
      /// @param pitch Note pitch.
      public static void Instrument_SetNoteOn(IntPtr instrumentHandle, double pitch,
                                              double intensity) {
        if (!BarelyInstrument_SetNoteOn(instrumentHandle, pitch, intensity) &&
            instrumentHandle != IntPtr.Zero) {
          Debug.LogError("Failed to start instrument note " + pitch + " with " + intensity +
                         " intensity");
        }
      }

      /// Returns the corresponding number of seconds for a given number of musician beats.
      ///
      /// @param frames Number of beats.
      /// @return Number of seconds.
      public static double Musician_GetSecondsFromBeats(double beats) {
        double seconds = 0.0;
        if (!BarelyMusician_GetSecondsFromBeats(Handle, beats, ref seconds) &&
            _handle != IntPtr.Zero) {
          Debug.LogError("Failed to get seconds for " + beats + " musician beats");
        }
        return seconds;
      }

      /// Returns the corresponding number of musician beats for a given number of seconds.
      ///
      /// @param frames Number of seconds.
      /// @return Number of beats.
      public static double Musician_GetBeatsFromSeconds(double seconds) {
        double beats = 0.0;
        if (!BarelyMusician_GetBeatsFromSeconds(Handle, seconds, ref beats) &&
            _handle != IntPtr.Zero) {
          Debug.LogError("Failed to get musician beats for " + seconds + " seconds");
        }
        return beats;
      }

      /// Returns the tempo of a musician.
      ///
      /// @return Tempo in beats per minute.
      public static double Musician_GetTempo() {
        double tempo = 0.0;
        if (!BarelyMusician_GetTempo(Handle, ref tempo) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to get musician tempo");
        }
        return tempo;
      }

      /// Returns the timestamp of a musician.
      ///
      /// @return Timestamp in seconds.
      public static double Musician_GetTimestamp() {
        double timestamp = 0.0;
        if (!BarelyMusician_GetTimestamp(Handle, ref timestamp) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to get musician timestamp");
        }
        return timestamp;
      }

      /// Schedules a new musician task.
      ///
      /// @param callback Task process callback.
      /// @param timestamp Task timestamp in seconds.
      public static void Musician_ScheduleTask(Action callback, double timestamp) {
        if (timestamp < Timestamp) {
          Debug.LogError("Failed to create musician task at " + timestamp);
          return;
        }
        List<Action> callbacks = null;
        if (_scheduledTaskCallbacks != null &&
            !_scheduledTaskCallbacks.TryGetValue(timestamp, out callbacks)) {
          callbacks = new List<Action>();
          _scheduledTaskCallbacks.Add(timestamp, callbacks);
        }
        callbacks?.Add(callback);
      }

      /// Sets the tempo of a musician.
      ///
      /// @param tempo Tempo in beats per minute.
      public static void Musician_SetTempo(double tempo) {
        if (!BarelyMusician_SetTempo(Handle, tempo) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to set musician tempo");
        }
      }

      /// Cancels all one-off tasks.
      ///
      /// @param performerHandle Performer handle.
      public static void Performer_CancelAllOneOffTasks(IntPtr performerHandle) {
        if (!BarelyPerformer_CancelAllOneOffTasks(performerHandle) &&
            performerHandle != IntPtr.Zero) {
          Debug.LogError("Failed to cancel all one-off tasks");
        }
      }

      /// Creates a new performer.
      ///
      /// @param performer Performer.
      /// @param performerHandle Performer handle.
      public static void Performer_Create(Performer performer, ref IntPtr performerHandle) {
        if (Handle == IntPtr.Zero || performerHandle != IntPtr.Zero) {
          return;
        }
        if (!BarelyMusician_AddPerformer(Handle, /*processOrder=*/0, ref performerHandle)) {
          Debug.LogError("Failed to create performer '" + performer.name + "'");
        }
      }

      /// Destroys a performer.
      ///
      /// @param performerHandle Performer handle.
      public static void Performer_Destroy(ref IntPtr performerHandle) {
        if (_handle == IntPtr.Zero || performerHandle == IntPtr.Zero) {
          performerHandle = IntPtr.Zero;
          return;
        }
        if (!BarelyMusician_RemovePerformer(_handle, performerHandle)) {
          Debug.LogError("Failed to destroy performer");
        }
        performerHandle = IntPtr.Zero;
      }

      /// Returns the loop begin position of a performer.
      ///
      /// @param performerHandle Performer handle.
      /// @return Loop begin position in beats.
      public static double Performer_GetLoopBeginPosition(IntPtr performerHandle) {
        double loopBeginPosition = 0.0;
        if (!BarelyPerformer_GetLoopBeginPosition(performerHandle, ref loopBeginPosition) &&
            performerHandle != IntPtr.Zero) {
          Debug.LogError("Failed to get performer loop begin position");
        }
        return loopBeginPosition;
      }

      /// Returns the loop length of a performer.
      ///
      /// @param performerHandle Performer handle.
      /// @return Loop length in beats.
      public static double Performer_GetLoopLength(IntPtr performerHandle) {
        double loopLength = 0.0;
        if (!BarelyPerformer_GetLoopLength(performerHandle, ref loopLength) &&
            performerHandle != IntPtr.Zero) {
          Debug.LogError("Failed to get performer loop length");
        }
        return loopLength;
      }

      /// Returns the position of a performer.
      ///
      /// @param performerHandle Performer handle.
      /// @return Position in beats.
      public static double Performer_GetPosition(IntPtr performerHandle) {
        double position = 0.0;
        if (!BarelyPerformer_GetPosition(performerHandle, ref position) &&
            performerHandle != IntPtr.Zero) {
          Debug.LogError("Failed to get performer position");
        }
        return position;
      }

      /// Returns whether a performer is looping or not.
      ///
      /// @param performerHandle Performer handle.
      /// @return True if looping, false otherwise.
      public static bool Performer_IsLooping(IntPtr performerHandle) {
        bool isLooping = false;
        if (!BarelyPerformer_IsLooping(performerHandle, ref isLooping) &&
            performerHandle != IntPtr.Zero) {
          Debug.LogError("Failed to get if performer is looping");
        }
        return isLooping;
      }

      /// Returns whether a performer is playing or not.
      ///
      /// @param performerHandle Performer handle.
      /// @return True if playing, false otherwise.
      public static bool Performer_IsPlaying(IntPtr performerHandle) {
        bool isPlaying = false;
        if (!BarelyPerformer_IsPlaying(performerHandle, ref isPlaying) &&
            performerHandle != IntPtr.Zero) {
          Debug.LogError("Failed to get if performer is playing");
        }
        return isPlaying;
      }

      /// Schedules a one-off task.
      ///
      /// @param performerHandle Performer handle.
      /// @param callback Task callback.
      /// @param position Task position.
      public static void Performer_ScheduleOneOffTask(IntPtr performerHandle, Action callback,
                                                      double position) {
        if (Handle == null || callback == null) {
          return;
        }
        GCHandle ptr = GCHandle.Alloc(callback);
        if (!BarelyPerformer_ScheduleOneOffTask(performerHandle, _taskDefinition, position,
                                                GCHandle.ToIntPtr(ptr)) &&
            performerHandle != IntPtr.Zero) {
          ptr.Free();
          Debug.LogError("Failed to set performer loop begin position");
        }
      }

      /// Sets the loop begin position of a performer.
      ///
      /// @param performerHandle Performer handle.
      /// @param loopBeginPosition Loop begin position in beats.
      public static void Performer_SetLoopBeginPosition(IntPtr performerHandle,
                                                        double loopBeginPosition) {
        if (!BarelyPerformer_SetLoopBeginPosition(performerHandle, loopBeginPosition) &&
            performerHandle != IntPtr.Zero) {
          Debug.LogError("Failed to set performer loop begin position");
        }
      }

      /// Sets the loop length of a performer.
      ///
      /// @param performerHandle Performer handle.
      /// @param loopLength Loop length in beats.
      public static void Performer_SetLoopLength(IntPtr performerHandle, double loopLength) {
        if (!BarelyPerformer_SetLoopLength(performerHandle, loopLength) &&
            performerHandle != IntPtr.Zero) {
          Debug.LogError("Failed to set performer loop length");
        }
      }

      /// Sets whether a performer is looping or not.
      ///
      /// @param performerHandle Performer handle.
      /// @param isLooping True if looping, false otherwise.
      public static void Performer_SetLooping(IntPtr performerHandle, bool isLooping) {
        if (!BarelyPerformer_SetLooping(performerHandle, isLooping) &&
            performerHandle != IntPtr.Zero) {
          Debug.LogError("Failed to set performer looping");
        }
      }

      /// Sets the position of a performer.
      ///
      /// @param performerHandle Performer handle.
      /// @param position Position in beats.
      public static void Performer_SetPosition(IntPtr performerHandle, double position) {
        if (!BarelyPerformer_SetPosition(performerHandle, position) &&
            performerHandle != IntPtr.Zero) {
          Debug.LogError("Failed to set performer position");
        }
      }

      /// Starts a performer.
      ///
      /// @param performerHandle Performer handle.
      public static void Performer_Start(IntPtr performerHandle) {
        if (!BarelyPerformer_Start(performerHandle) && performerHandle != IntPtr.Zero) {
          Debug.LogError("Failed to start performer");
        }
      }

      /// Stops a performer.
      ///
      /// @param performerHandle Performer handle.
      public static void Performer_Stop(IntPtr performerHandle) {
        if (!BarelyPerformer_Stop(performerHandle) && performerHandle != IntPtr.Zero) {
          Debug.LogError("Failed to stop performer");
        }
      }

      /// Creates a new task.
      ///
      /// @param performerHandle Performer handle.
      /// @param callback Task callback.
      /// @param position Task position.
      /// @param taskHandle Task handle.
      public static void Task_Create(IntPtr performerHandle, Action callback, double position,
                                     ref IntPtr taskHandle) {
        if (Handle == IntPtr.Zero || taskHandle != IntPtr.Zero) {
          return;
        }
        GCHandle ptr = GCHandle.Alloc(callback);
        if (!BarelyPerformer_AddTask(performerHandle, _taskDefinition, position,
                                     GCHandle.ToIntPtr(ptr), ref taskHandle)) {
          ptr.Free();
        }
      }

      /// Destroys a task.
      ///
      /// @param taskHandle Task handle.
      public static void Task_Destroy(IntPtr performerHandle, ref IntPtr taskHandle) {
        if (_handle == IntPtr.Zero || performerHandle == IntPtr.Zero || taskHandle == IntPtr.Zero) {
          taskHandle = IntPtr.Zero;
          return;
        }
        if (!BarelyPerformer_RemoveTask(performerHandle, taskHandle)) {
          Debug.LogError("Failed to destroy performer task");
        }
        taskHandle = IntPtr.Zero;
      }

      /// Returns the position of a task.
      ///
      /// @param taskHandle Task handle.
      /// @return Position in beats.
      public static double Task_GetPosition(IntPtr taskHandle) {
        double position = 0.0;
        if (!BarelyTask_GetPosition(taskHandle, ref position) && taskHandle != IntPtr.Zero) {
          Debug.LogError("Failed to get performer task position");
        }
        return position;
      }

      /// Sets the position of a task.
      ///
      /// @param taskHandle Task handle.
      /// @param position Position in beats.
      public static void Task_SetPosition(IntPtr taskHandle, double position) {
        if (!BarelyTask_SetPosition(taskHandle, position) && taskHandle != IntPtr.Zero) {
          Debug.LogError("Failed to set performer task position");
        }
      }

      /// Returns whether an arpeggiator note is on or not.
      ///
      /// @param arpeggiatorHandle Arpeggiator handle.
      /// @param pitch Note pitch.
      /// @return True if on, false otherwise.
      public static bool Arpeggiator_IsNoteOn(IntPtr arpeggiatorHandle, double pitch) {
        bool isNoteOn = false;
        if (!BarelyArpeggiator_IsNoteOn(arpeggiatorHandle, pitch, ref isNoteOn) &&
            arpeggiatorHandle != IntPtr.Zero) {
          Debug.LogError("Failed to get if arpeggiator note " + pitch + " is on");
        }
        return isNoteOn;
      }

      /// Returns whether an arpeggiator is playing or not.
      ///
      /// @param arpeggiatorHandle Arpeggiator handle.
      /// @return True if playing, false otherwise.
      public static bool Arpeggiator_IsPlaying(IntPtr arpeggiatorHandle) {
        bool isPlaying = false;
        if (!BarelyArpeggiator_IsPlaying(arpeggiatorHandle, ref isPlaying) &&
            arpeggiatorHandle != IntPtr.Zero) {
          Debug.LogError("Failed to get if arpeggiator is playing");
        }
        return isPlaying;
      }

      /// Sets all arpeggiator notes off.
      ///
      /// @param arpeggiatorHandle Arpeggiator handle.
      public static void Arpeggiator_SetAllNotesOff(IntPtr arpeggiatorHandle) {
        if (!BarelyArpeggiator_SetAllNotesOff(arpeggiatorHandle) &&
            arpeggiatorHandle != IntPtr.Zero) {
          Debug.LogError("Failed to stop all arpeggiator notes");
        }
      }

      /// Sets an arpeggiator gate ratio.
      ///
      /// @param arpeggiatorHandle Arpeggiator handle.
      /// @param gateRatio Gate ratio.
      public static void Arpeggiator_SetGateRatio(IntPtr arpeggiatorHandle, double gateRatio) {
        if (!BarelyArpeggiator_SetGateRatio(arpeggiatorHandle, gateRatio) &&
            arpeggiatorHandle != IntPtr.Zero) {
          Debug.LogError("Failed to stop arpeggiator gate ratio " + gateRatio);
        }
      }

      /// Sets an arpeggiator instrument.
      ///
      /// @param arpeggiatorHandle Arpeggiator handle.
      /// @param instrument Instrument.
      public static void Arpeggiator_SetInstrument(IntPtr arpeggiatorHandle,
                                                   Instrument instrument) {
        if (!BarelyArpeggiator_SetInstrument(arpeggiatorHandle,
                                             Instrument.Internal.GetHandle(instrument)) &&
            arpeggiatorHandle != IntPtr.Zero) {
          Debug.LogError("Failed to set arpeggiator instrument '" + instrument.name + "'");
        }
      }

      /// Sets an arpeggiator note off.
      ///
      /// @param arpeggiatorHandle Arpeggiator handle.
      /// @param pitch Note pitch.
      public static void Arpeggiator_SetNoteOff(IntPtr arpeggiatorHandle, double pitch) {
        if (!BarelyArpeggiator_SetNoteOff(arpeggiatorHandle, pitch) &&
            arpeggiatorHandle != IntPtr.Zero) {
          Debug.LogError("Failed to stop arpeggiator note " + pitch);
        }
      }

      /// Sets an arpeggiator note on.
      ///
      /// @param arpeggiatorHandle Arpeggiator handle.
      /// @param pitch Note pitch.
      public static void Arpeggiator_SetNoteOn(IntPtr arpeggiatorHandle, double pitch) {
        if (!BarelyArpeggiator_SetNoteOn(arpeggiatorHandle, pitch) &&
            arpeggiatorHandle != IntPtr.Zero) {
          Debug.LogError("Failed to start arpeggiator note " + pitch);
        }
      }

      /// Sets an arpeggiator rate.
      ///
      /// @param arpeggiatorHandle Arpeggiator handle.
      /// @param rate Rate in notes per beat.
      public static void Arpeggiator_SetRate(IntPtr arpeggiatorHandle, double rate) {
        if (!BarelyArpeggiator_SetRate(arpeggiatorHandle, rate) &&
            arpeggiatorHandle != IntPtr.Zero) {
          Debug.LogError("Failed to set arpeggiator rate " + rate);
        }
      }

      /// Sets an arpeggiator style.
      ///
      /// @param arpeggiatorHandle Arpeggiator handle.
      /// @param style Style.
      public static void Arpeggiator_SetStyle(IntPtr arpeggiatorHandle, ArpeggiatorStyle style) {
        if (!BarelyArpeggiator_SetStyle(arpeggiatorHandle, style) &&
            arpeggiatorHandle != IntPtr.Zero) {
          Debug.LogError("Failed to set arpeggiator style " + style);
        }
      }

      /// Returns whether an repeater is playing or not.
      ///
      /// @param repeaterHandle Repeater handle.
      /// @return True if playing, false otherwise.
      public static bool Repeater_IsPlaying(IntPtr repeaterHandle) {
        bool isPlaying = false;
        if (!BarelyRepeater_IsPlaying(repeaterHandle, ref isPlaying) &&
            repeaterHandle != IntPtr.Zero) {
          Debug.LogError("Failed to get if repeater is playing");
        }
        return isPlaying;
      }

      /// Pops the last note from the end.
      ///
      /// @param repeaterHandle Repeater handle.
      public static void Repeater_Pop(IntPtr repeaterHandle) {
        if (!BarelyRepeater_Pop(repeaterHandle) && repeaterHandle != IntPtr.Zero) {
          Debug.LogError("Failed to pop from repeater");
        }
      }

      /// Pushes a new note to the end.
      ///
      /// @param repeaterHandle Repeater handle.
      /// @param pitchOr Note pitch value or silence.
      /// @param length Note length in beats.
      public static void Repeater_Push(IntPtr repeaterHandle, double? pitchOr, int length) {
        if ((pitchOr.HasValue && !BarelyRepeater_Push(repeaterHandle, pitchOr.Value, length)) ||
            (!pitchOr.HasValue && !BarelyRepeater_PushSilence(repeaterHandle, length)) &&
                repeaterHandle != IntPtr.Zero) {
          Debug.LogError("Failed to pop from repeater");
        }
      }

      /// Sets an repeater instrument.
      ///
      /// @param repeaterHandle Repeater handle.
      /// @param instrument Instrument.
      public static void Repeater_SetInstrument(IntPtr repeaterHandle, Instrument instrument) {
        if (!BarelyRepeater_SetInstrument(repeaterHandle,
                                          Instrument.Internal.GetHandle(instrument)) &&
            repeaterHandle != IntPtr.Zero) {
          Debug.LogError("Failed to set repeater instrument '" + instrument.name + "'");
        }
      }

      /// Sets an repeater rate.
      ///
      /// @param repeaterHandle Repeater handle.
      /// @param rate Rate in notes per beat.
      public static void Repeater_SetRate(IntPtr repeaterHandle, double rate) {
        if (!BarelyRepeater_SetRate(repeaterHandle, rate) && repeaterHandle != IntPtr.Zero) {
          Debug.LogError("Failed to set repeater rate " + rate);
        }
      }

      /// Sets an repeater style.
      ///
      /// @param repeaterHandle Repeater handle.
      /// @param style Style.
      public static void Repeater_SetStyle(IntPtr repeaterHandle, RepeaterStyle style) {
        if (!BarelyRepeater_SetStyle(repeaterHandle, style) && repeaterHandle != IntPtr.Zero) {
          Debug.LogError("Failed to set repeater style " + style);
        }
      }

      /// Starts a repeater.
      ///
      /// @param repeaterHandle Repeater handle.
      /// @param pitchOffset Pitch offset.
      public static void Repeater_Start(IntPtr repeaterHandle, double pitchOffset) {
        if (!BarelyRepeater_Start(repeaterHandle, pitchOffset) && repeaterHandle != IntPtr.Zero) {
          Debug.LogError("Failed to start repeater with a pitch offset" + pitchOffset);
        }
      }

      /// Stops a repeater.
      ///
      /// @param repeaterHandle Repeater handle.
      public static void Repeater_Stop(IntPtr repeaterHandle) {
        if (!BarelyRepeater_Stop(repeaterHandle) && repeaterHandle != IntPtr.Zero) {
          Debug.LogError("Failed to stop repeater");
        }
      }

      // Internal output samples.
      public static double[] OutputSamples { get; private set; } = null;

      // Note off event definition create callback.
      private delegate void NoteOffEventDefinition_CreateCallback(ref IntPtr state,
                                                                  IntPtr userData);
      [AOT.MonoPInvokeCallback(typeof(NoteOffEventDefinition_CreateCallback))]
      private static void NoteOffEventDefinition_OnCreate(ref IntPtr state, IntPtr userData) {
        state = userData;
      }

      // Note off event definition destroy callback.
      private delegate void NoteOffEventDefinition_DestroyCallback(ref IntPtr state);
      [AOT.MonoPInvokeCallback(typeof(NoteOffEventDefinition_DestroyCallback))]
      private static void NoteOffEventDefinition_OnDestroy(ref IntPtr state) {
        GCHandle.FromIntPtr(state).Free();
      }

      // Note off event definition process callback.
      private delegate void NoteOffEventDefinition_ProcessCallback(ref IntPtr state, double pitch);
      [AOT.MonoPInvokeCallback(typeof(NoteOffEventDefinition_ProcessCallback))]
      private static void NoteOffEventDefinition_OnProcess(ref IntPtr state, double pitch) {
        Instrument instrument = GCHandle.FromIntPtr(state).Target as Instrument;
        Instrument.Internal.OnNoteOffEvent(instrument, pitch);
      }

      // Note off event definition.
      [StructLayout(LayoutKind.Sequential)]
      private struct NoteOffEventDefinition {
        // Create callback.
        public NoteOffEventDefinition_CreateCallback createCallback;

        // Destroy callback.
        public NoteOffEventDefinition_DestroyCallback destroyCallback;

        // Process callback.
        public NoteOffEventDefinition_ProcessCallback processCallback;
      }

      // Note on event definition create callback.
      private delegate void NoteOnEventDefinition_CreateCallback(ref IntPtr state, IntPtr userData);
      [AOT.MonoPInvokeCallback(typeof(NoteOnEventDefinition_CreateCallback))]
      private static void NoteOnEventDefinition_OnCreate(ref IntPtr state, IntPtr userData) {
        state = userData;
      }

      // Note on event definition destroy callback.
      private delegate void NoteOnEventDefinition_DestroyCallback(ref IntPtr state);
      [AOT.MonoPInvokeCallback(typeof(NoteOnEventDefinition_DestroyCallback))]
      private static void NoteOnEventDefinition_OnDestroy(ref IntPtr state) {
        GCHandle.FromIntPtr(state).Free();
      }

      // Note on event definition process callback.
      private delegate void NoteOnEventDefinition_ProcessCallback(ref IntPtr state, double pitch,
                                                                  double intensity);
      [AOT.MonoPInvokeCallback(typeof(NoteOnEventDefinition_ProcessCallback))]
      private static void NoteOnEventDefinition_OnProcess(ref IntPtr state, double pitch,
                                                          double intensity) {
        Instrument instrument = GCHandle.FromIntPtr(state).Target as Instrument;
        Instrument.Internal.OnNoteOnEvent(instrument, pitch, intensity);
      }

      // Note on event definition.
      [StructLayout(LayoutKind.Sequential)]
      private struct NoteOnEventDefinition {
        // Create callback.
        public NoteOnEventDefinition_CreateCallback createCallback;

        // Destroy callback.
        public NoteOnEventDefinition_DestroyCallback destroyCallback;

        // Process callback.
        public NoteOnEventDefinition_ProcessCallback processCallback;
      }

      // Sample data definition.
      [StructLayout(LayoutKind.Sequential)]
      private struct SampleDataDefinition {
        // Root note pitch.
        public double rootPitch;

        // Sampling rate in hertz.
        public Int32 sampleRate;

        // Array of mono samples.
        public double[] samples;

        // Number of mono samples.
        public Int32 sampleCount;
      }

      // Task definition create callback.
      private delegate void TaskDefinition_CreateCallback(ref IntPtr state, IntPtr userData);
      [AOT.MonoPInvokeCallback(typeof(TaskDefinition_CreateCallback))]
      private static void TaskDefinition_OnCreate(ref IntPtr state, IntPtr userData) {
        state = userData;
      }

      // Task definition destroy callback.
      private delegate void TaskDefinition_DestroyCallback(ref IntPtr state);
      [AOT.MonoPInvokeCallback(typeof(TaskDefinition_DestroyCallback))]
      private static void TaskDefinition_OnDestroy(ref IntPtr state) {
        GCHandle.FromIntPtr(state).Free();
      }

      // Task definition process callback.
      private delegate void TaskDefinition_ProcessCallback(ref IntPtr state);
      [AOT.MonoPInvokeCallback(typeof(TaskDefinition_ProcessCallback))]
      private static void TaskDefinition_OnProcess(ref IntPtr state) {
        (GCHandle.FromIntPtr(state).Target as Action)?.Invoke();
      }

      // Task definition.
      [StructLayout(LayoutKind.Sequential)]
      private struct TaskDefinition {
        // Create callback.
        public TaskDefinition_CreateCallback createCallback;

        // Destroy callback.
        public TaskDefinition_DestroyCallback destroyCallback;

        // Process callback.
        public TaskDefinition_ProcessCallback processCallback;
      }

      // Singleton musician handle.
      private static IntPtr Handle {
        get {
          if (_isShuttingDown) {
            return IntPtr.Zero;
          }
          if (_handle == IntPtr.Zero) {
            var state =
                new GameObject() { hideFlags = HideFlags.HideAndDontSave }.AddComponent<State>();
            GameObject.DontDestroyOnLoad(state.gameObject);
            if (_handle == IntPtr.Zero) {
              GameObject.DestroyImmediate(state.gameObject);
              _isShuttingDown = true;
            }
          }
          return _handle;
        }
      }
      private static IntPtr _handle = IntPtr.Zero;

      // Note off event definition.
      private static NoteOffEventDefinition _noteOffEventDefinition = new NoteOffEventDefinition() {
        createCallback = NoteOffEventDefinition_OnCreate,
        destroyCallback = NoteOffEventDefinition_OnDestroy,
        processCallback = NoteOffEventDefinition_OnProcess,
      };

      // Note on event definition.
      private static NoteOnEventDefinition _noteOnEventDefinition = new NoteOnEventDefinition() {
        createCallback = NoteOnEventDefinition_OnCreate,
        destroyCallback = NoteOnEventDefinition_OnDestroy,
        processCallback = NoteOnEventDefinition_OnProcess,
      };

      // Task definition.
      private static TaskDefinition _taskDefinition = new TaskDefinition() {
        createCallback = TaskDefinition_OnCreate,
        destroyCallback = TaskDefinition_OnDestroy,
        processCallback = TaskDefinition_OnProcess,
      };

      // Denotes if the system is shutting down to avoid re-initialization.
      private static bool _isShuttingDown = false;

      // Latency in seconds.
      private static double _latency = 0.0;

      // Map of scheduled list of task callbacks by their timestamps.
      private static SortedDictionary<double, List<Action>> _scheduledTaskCallbacks = null;

      // Component that manages internal state.
      private class State : MonoBehaviour {
        private void Awake() {
          AudioSettings.OnAudioConfigurationChanged += OnAudioConfigurationChanged;
          Initialize();
        }

        private void OnDestroy() {
          AudioSettings.OnAudioConfigurationChanged -= OnAudioConfigurationChanged;
          Shutdown();
        }

        private void OnApplicationQuit() {
          GameObject.Destroy(gameObject);
        }

        private void OnAudioConfigurationChanged(bool deviceWasChanged) {
          Shutdown();
          var instruments = FindObjectsOfType<Instrument>();
          for (int i = 0; i < instruments.Length; ++i) {
            instruments[i].enabled = false;
          }
          Initialize();
          for (int i = 0; i < instruments.Length; ++i) {
            instruments[i].enabled = true;
          }
        }

        private void FixedUpdate() {
          double nextTimestamp = GetNextTimestamp();
          while (_scheduledTaskCallbacks.Count > 0) {
            double taskTimestamp = _scheduledTaskCallbacks.ElementAt(0).Key;
            if (taskTimestamp > nextTimestamp) {
              break;
            }
            BarelyMusician_Update(_handle, taskTimestamp);
            var callbacks = _scheduledTaskCallbacks.ElementAt(0).Value;
            for (int i = 0; i < callbacks.Count; ++i) {
              callbacks[i]?.Invoke();
            }
            _scheduledTaskCallbacks.Remove(taskTimestamp);
          }
          BarelyMusician_Update(_handle, nextTimestamp);
        }

        // Initializes the native state.
        private void Initialize() {
          _isShuttingDown = false;
          var config = AudioSettings.GetConfiguration();
          if (!BarelyMusician_Create(config.sampleRate, /*C4=*/261.625565301, ref _handle)) {
            Debug.LogError("Failed to initialize BarelyMusician");
            return;
          }
          BarelyMusician_SetTempo(_handle, _tempo);
          OutputSamples = new double[config.dspBufferSize * (int)config.speakerMode];
          _latency = (double)config.dspBufferSize / config.sampleRate;
          _scheduledTaskCallbacks = new SortedDictionary<double, List<Action>>();
          BarelyMusician_Update(_handle, GetNextTimestamp());
        }

        // Shuts down the native state.
        private void Shutdown() {
          _isShuttingDown = true;
          BarelyMusician_Destroy(_handle);
          _handle = IntPtr.Zero;
          _scheduledTaskCallbacks = null;
        }

        // Returns the next timestamp to update.
        private double GetNextTimestamp() {
          double lookahead = Math.Max(_latency, (double)Time.fixedDeltaTime);
          return AudioSettings.dspTime + lookahead;
        }
      }

#if !UNITY_EDITOR && UNITY_IOS
      private const string pluginName = "__Internal";
#else
      private const string pluginName = "barelymusicianunity";
#endif  // !UNITY_EDITOR && UNITY_IOS

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_GetControl")]
      private static extern bool BarelyInstrument_GetControl(IntPtr instrument, Int32 index,
                                                             ref double outValue);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_GetNoteControl")]
      private static extern bool BarelyInstrument_GetNoteControl(IntPtr instrument, double pitch,
                                                                 Int32 index, ref double outValue);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_IsNoteOn")]
      private static extern bool BarelyInstrument_IsNoteOn(IntPtr instrument, double pitch,
                                                           ref bool outIsNoteOn);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_Process")]
      private static extern bool BarelyInstrument_Process(IntPtr instrument,
                                                          [In, Out] double[] outputSamples,
                                                          Int32 outputChannelCount,
                                                          Int32 outputFrameCount, double timestamp);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_ResetControl")]
      private static extern bool BarelyInstrument_ResetControl(IntPtr instrument, Int32 index);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_ResetNoteControl")]
      private static extern bool BarelyInstrument_ResetNoteControl(IntPtr instrument, double pitch,
                                                                   Int32 index);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_SetAllNotesOff")]
      private static extern bool BarelyInstrument_SetAllNotesOff(IntPtr instrument);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_SetControl")]
      private static extern bool BarelyInstrument_SetControl(IntPtr instrument, Int32 index,
                                                             double value);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_SetNoteControl")]
      private static extern bool BarelyInstrument_SetNoteControl(IntPtr instrument, double pitch,
                                                                 Int32 index, double value);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_SetNoteOff")]
      private static extern bool BarelyInstrument_SetNoteOff(IntPtr instrument, double pitch);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_SetNoteOffEvent")]
      private static extern bool BarelyInstrument_SetNoteOffEvent(IntPtr instrument,
                                                                  NoteOffEventDefinition definition,
                                                                  IntPtr userData);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_SetNoteOn")]
      private static extern bool BarelyInstrument_SetNoteOn(IntPtr instrument, double pitch,
                                                            double intensity);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_SetNoteOnEvent")]
      private static extern bool BarelyInstrument_SetNoteOnEvent(IntPtr instrument,
                                                                 NoteOnEventDefinition definition,
                                                                 IntPtr userData);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_SetSampleData")]
      private static extern bool BarelyInstrument_SetSampleData(
          IntPtr instrument, [In] SampleDataDefinition[] definitions, Int32 definitionCount);

      [DllImport(pluginName, EntryPoint = "BarelyMusician_AddInstrument")]
      private static extern bool BarelyMusician_AddInstrument(IntPtr musician,
                                                              ref IntPtr outInstrument);

      [DllImport(pluginName, EntryPoint = "BarelyMusician_AddPerformer")]
      private static extern bool BarelyMusician_AddPerformer(IntPtr musician, Int32 processOrder,
                                                             ref IntPtr outPerformer);

      [DllImport(pluginName, EntryPoint = "BarelyMusician_Create")]
      private static extern bool BarelyMusician_Create(Int32 frameRate, double referenceFrequency,
                                                       ref IntPtr outMusician);

      [DllImport(pluginName, EntryPoint = "BarelyMusician_Destroy")]
      private static extern bool BarelyMusician_Destroy(IntPtr musician);

      [DllImport(pluginName, EntryPoint = "BarelyMusician_GetBeatsFromSeconds")]
      private static extern bool BarelyMusician_GetBeatsFromSeconds(IntPtr musician, double seconds,
                                                                    ref double outBeats);

      [DllImport(pluginName, EntryPoint = "BarelyMusician_GetSecondsFromBeats")]
      private static extern bool BarelyMusician_GetSecondsFromBeats(IntPtr musician, double beats,
                                                                    ref double outSeconds);

      [DllImport(pluginName, EntryPoint = "BarelyMusician_GetTempo")]
      private static extern bool BarelyMusician_GetTempo(IntPtr musician, ref double outTempo);

      [DllImport(pluginName, EntryPoint = "BarelyMusician_GetTimestamp")]
      private static extern bool BarelyMusician_GetTimestamp(IntPtr musician,
                                                             ref double outTimestamp);

      [DllImport(pluginName, EntryPoint = "BarelyMusician_RemoveInstrument")]
      private static extern bool BarelyMusician_RemoveInstrument(IntPtr musician,
                                                                 IntPtr instrument);

      [DllImport(pluginName, EntryPoint = "BarelyMusician_RemovePerformer")]
      private static extern bool BarelyMusician_RemovePerformer(IntPtr musician, IntPtr performer);

      [DllImport(pluginName, EntryPoint = "BarelyMusician_SetTempo")]
      private static extern bool BarelyMusician_SetTempo(IntPtr musician, double tempo);

      [DllImport(pluginName, EntryPoint = "BarelyMusician_Update")]
      private static extern bool BarelyMusician_Update(IntPtr musician, double timestamp);

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_AddTask")]
      private static extern bool BarelyPerformer_AddTask(IntPtr performer,
                                                         TaskDefinition definition, double position,
                                                         IntPtr userData, ref IntPtr outTask);

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_CancelAllOneOffTasks")]
      private static extern bool BarelyPerformer_CancelAllOneOffTasks(IntPtr performer);

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_GetLoopBeginPosition")]
      private static extern bool BarelyPerformer_GetLoopBeginPosition(
          IntPtr performer, ref double outLoopBeginPosition);

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_GetLoopLength")]
      private static extern bool BarelyPerformer_GetLoopLength(IntPtr performer,
                                                               ref double outLoopLength);

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_GetPosition")]
      private static extern bool BarelyPerformer_GetPosition(IntPtr performer,
                                                             ref double outPosition);

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_IsLooping")]
      private static extern bool BarelyPerformer_IsLooping(IntPtr performer, ref bool outIsLooping);

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_IsPlaying")]
      private static extern bool BarelyPerformer_IsPlaying(IntPtr performer, ref bool outIsPlaying);

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_RemoveTask")]
      private static extern bool BarelyPerformer_RemoveTask(IntPtr performer, IntPtr task);

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_ScheduleOneOffTask")]
      private static extern bool BarelyPerformer_ScheduleOneOffTask(IntPtr performer,
                                                                    TaskDefinition definition,
                                                                    double position,
                                                                    IntPtr userData);

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_SetLoopBeginPosition")]
      private static extern bool BarelyPerformer_SetLoopBeginPosition(IntPtr performer,
                                                                      double loopBeginPosition);

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_SetLoopLength")]
      private static extern bool BarelyPerformer_SetLoopLength(IntPtr performer, double loopLength);

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_SetLooping")]
      private static extern bool BarelyPerformer_SetLooping(IntPtr performer, bool isLooping);

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_SetPosition")]
      private static extern bool BarelyPerformer_SetPosition(IntPtr performer, double position);

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_Start")]
      private static extern bool BarelyPerformer_Start(IntPtr performer);

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_Stop")]
      private static extern bool BarelyPerformer_Stop(IntPtr performer);

      [DllImport(pluginName, EntryPoint = "BarelyTask_GetPosition")]
      private static extern bool BarelyTask_GetPosition(IntPtr task, ref double outPosition);

      [DllImport(pluginName, EntryPoint = "BarelyTask_SetPosition")]
      private static extern bool BarelyTask_SetPosition(IntPtr task, double position);

      // Components.
      [DllImport(pluginName, EntryPoint = "BarelyArpeggiator_Create")]
      private static extern bool BarelyArpeggiator_Create(IntPtr musician, Int32 processOrder,
                                                          ref IntPtr outArpeggiator);

      [DllImport(pluginName, EntryPoint = "BarelyArpeggiator_Destroy")]
      private static extern bool BarelyArpeggiator_Destroy(IntPtr arpeggiator);

      [DllImport(pluginName, EntryPoint = "BarelyArpeggiator_IsNoteOn")]
      private static extern bool BarelyArpeggiator_IsNoteOn(IntPtr arpeggiator, double pitch,
                                                            ref bool outIsNoteOn);

      [DllImport(pluginName, EntryPoint = "BarelyArpeggiator_IsPlaying")]
      private static extern bool BarelyArpeggiator_IsPlaying(IntPtr arpeggiator,
                                                             ref bool outIsPlaying);

      [DllImport(pluginName, EntryPoint = "BarelyArpeggiator_SetAllNotesOff")]
      private static extern bool BarelyArpeggiator_SetAllNotesOff(IntPtr arpeggiator);

      [DllImport(pluginName, EntryPoint = "BarelyArpeggiator_SetGateRatio")]
      private static extern bool BarelyArpeggiator_SetGateRatio(IntPtr arpeggiator,
                                                                double gateRatio);

      [DllImport(pluginName, EntryPoint = "BarelyArpeggiator_SetInstrument")]
      private static extern bool BarelyArpeggiator_SetInstrument(IntPtr arpeggiator,
                                                                 IntPtr instrument);

      [DllImport(pluginName, EntryPoint = "BarelyArpeggiator_SetNoteOff")]
      private static extern bool BarelyArpeggiator_SetNoteOff(IntPtr arpeggiator, double gateRatio);

      [DllImport(pluginName, EntryPoint = "BarelyArpeggiator_SetNoteOn")]
      private static extern bool BarelyArpeggiator_SetNoteOn(IntPtr arpeggiator, double gateRatio);

      [DllImport(pluginName, EntryPoint = "BarelyArpeggiator_SetRate")]
      private static extern bool BarelyArpeggiator_SetRate(IntPtr arpeggiator, double rate);

      [DllImport(pluginName, EntryPoint = "BarelyArpeggiator_SetStyle")]
      private static extern bool BarelyArpeggiator_SetStyle(IntPtr arpeggiator,
                                                            ArpeggiatorStyle style);

      [DllImport(pluginName, EntryPoint = "BarelyRepeater_Clear")]
      private static extern bool BarelyRepeater_Clear(IntPtr repeater);

      [DllImport(pluginName, EntryPoint = "BarelyRepeater_Create")]
      private static extern bool BarelyRepeater_Create(IntPtr musician, Int32 processOrder,
                                                       ref IntPtr outRepeater);

      [DllImport(pluginName, EntryPoint = "BarelyRepeater_Destroy")]
      private static extern bool BarelyRepeater_Destroy(IntPtr repeater);

      [DllImport(pluginName, EntryPoint = "BarelyRepeater_IsPlaying")]
      private static extern bool BarelyRepeater_IsPlaying(IntPtr repeater, ref bool outIsPlaying);

      [DllImport(pluginName, EntryPoint = "BarelyRepeater_Pop")]
      private static extern bool BarelyRepeater_Pop(IntPtr repeater);

      [DllImport(pluginName, EntryPoint = "BarelyRepeater_Push")]
      private static extern bool BarelyRepeater_Push(IntPtr repeater, double pitch, Int32 length);

      [DllImport(pluginName, EntryPoint = "BarelyRepeater_PushSilence")]
      private static extern bool BarelyRepeater_PushSilence(IntPtr repeater, Int32 length);

      [DllImport(pluginName, EntryPoint = "BarelyRepeater_SetGateRatio")]
      private static extern bool BarelyRepeater_SetGateRatio(IntPtr repeater, double gateRatio);

      [DllImport(pluginName, EntryPoint = "BarelyRepeater_SetInstrument")]
      private static extern bool BarelyRepeater_SetInstrument(IntPtr repeater, IntPtr instrument);

      [DllImport(pluginName, EntryPoint = "BarelyRepeater_SetRate")]
      private static extern bool BarelyRepeater_SetRate(IntPtr repeater, double rate);

      [DllImport(pluginName, EntryPoint = "BarelyRepeater_SetStyle")]
      private static extern bool BarelyRepeater_SetStyle(IntPtr repeater, RepeaterStyle style);

      [DllImport(pluginName, EntryPoint = "BarelyRepeater_Start")]
      private static extern bool BarelyRepeater_Start(IntPtr repeater, double pitchOffset);

      [DllImport(pluginName, EntryPoint = "BarelyRepeater_Stop")]
      private static extern bool BarelyRepeater_Stop(IntPtr repeater);
    }
  }
}  // namespace Barely
