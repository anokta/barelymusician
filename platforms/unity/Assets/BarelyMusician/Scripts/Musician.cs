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
      /// @param componentPtr Pointer to component.
      public static void Component_Create(Component component, ref IntPtr componentPtr) {
        if (Ptr == IntPtr.Zero || componentPtr != IntPtr.Zero) {
          return;
        }
        switch (component) {
          case Arpeggiator arpeggiator:
            if (BarelyArpeggiator_Create(_ptr, arpeggiator.ProcessOrder, ref componentPtr)) {
              return;
            }
            break;
          case Repeater repeater:
            if (BarelyRepeater_Create(_ptr, repeater.ProcessOrder, ref componentPtr)) {
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
      /// @param componentPtr Pointer to component.
      public static void Component_Destroy(Component component, ref IntPtr componentPtr) {
        if (Ptr == IntPtr.Zero || componentPtr == IntPtr.Zero) {
          return;
        }
        bool success = true;
        switch (component) {
          case Arpeggiator arpeggiator:
            success = BarelyArpeggiator_Destroy(componentPtr);
            break;
          case Repeater repeater:
            success = BarelyRepeater_Destroy(componentPtr);
            break;
          default:
            Debug.LogError("Unsupported component type: " + component.GetType());
            return;
        }
        if (!success) {
          Debug.LogError("Failed to destroy component '" + component.name + "'");
        }
        componentPtr = IntPtr.Zero;
      }

      /// Creates a new instrument.
      ///
      /// @param instrument Instrument.
      /// @param instrumentPtr Pointer to instrument.
      public static void Instrument_Create(Instrument instrument, ref IntPtr instrumentPtr) {
        if (Ptr == IntPtr.Zero || instrumentPtr != IntPtr.Zero) {
          return;
        }
        bool success = BarelyInstrument_Create(Ptr, ref instrumentPtr);
        if (!success) {
          Debug.LogError("Failed to create instrument '" + instrument.name + "'");
          return;
        }
        GCHandle controlEventHandle = GCHandle.Alloc(instrument);
        BarelyInstrument_SetControlEvent(instrumentPtr, _controlEventDefinition,
                                         GCHandle.ToIntPtr(controlEventHandle));
        GCHandle noteControlEventHandle = GCHandle.Alloc(instrument);
        BarelyInstrument_SetNoteControlEvent(instrumentPtr, _noteControlEventDefinition,
                                             GCHandle.ToIntPtr(noteControlEventHandle));
        GCHandle noteOffEventHandle = GCHandle.Alloc(instrument);
        BarelyInstrument_SetNoteOffEvent(instrumentPtr, _noteOffEventDefinition,
                                         GCHandle.ToIntPtr(noteOffEventHandle));
        GCHandle noteOnEventHandle = GCHandle.Alloc(instrument);
        BarelyInstrument_SetNoteOnEvent(instrumentPtr, _noteOnEventDefinition,
                                        GCHandle.ToIntPtr(noteOnEventHandle));
      }

      /// Destroys an instrument.
      ///
      /// @param instrumentPtr Pointer to instrument.
      public static void Instrument_Destroy(ref IntPtr instrumentPtr) {
        if (Ptr == IntPtr.Zero || instrumentPtr == IntPtr.Zero) {
          instrumentPtr = IntPtr.Zero;
          return;
        }
        if (!BarelyInstrument_Destroy(instrumentPtr)) {
          Debug.LogError("Failed to destroy instrument");
        }
        instrumentPtr = IntPtr.Zero;
      }

      /// Returns the value of an instrument control.
      ///
      /// @param instrumentPtr Pointer to instrument.
      /// @param id Control identifier.
      /// @return Control value.
      public static double Instrument_GetControl(IntPtr instrumentPtr, int id) {
        double value = 0.0;
        if (!BarelyInstrument_GetControl(instrumentPtr, id, ref value) &&
            instrumentPtr != IntPtr.Zero) {
          Debug.LogError("Failed to get instrument control " + id);
        }
        return value;
      }

      /// Returns the value of an instrument note control.
      ///
      /// @param instrumentPtr Pointer to instrument.
      /// @param pitch Note pitch.
      /// @param id Note control identifier.
      /// @return Note control value.
      public static double Instrument_GetNoteControl(IntPtr instrumentPtr, double pitch, int id) {
        double value = 0.0;
        if (!BarelyInstrument_GetNoteControl(instrumentPtr, pitch, id, ref value) &&
            instrumentPtr != IntPtr.Zero) {
          Debug.LogError("Failed to get instrument note " + pitch + " control " + id + " value");
        }
        return value;
      }

      /// Returns whether an instrument note is on or not.
      ///
      /// @param instrumentPtr Pointer to instrument.
      /// @param pitch Note pitch.
      /// @return True if on, false otherwise.
      public static bool Instrument_IsNoteOn(IntPtr instrumentPtr, double pitch) {
        bool isNoteOn = false;
        if (!BarelyInstrument_IsNoteOn(instrumentPtr, pitch, ref isNoteOn) &&
            instrumentPtr != IntPtr.Zero) {
          Debug.LogError("Failed to get if instrument note " + pitch + " is on");
        }
        return isNoteOn;
      }

      /// Processes instrument output samples.
      ///
      /// @param instrumentPtr Pointer to instrument.
      /// @param outputSamples Output samples.
      /// @param outputChannelCount Number of output channels.
      public static void Instrument_Process(IntPtr instrumentPtr, float[] outputSamples,
                                            int outputChannelCount) {
        if (_ptr == IntPtr.Zero) {
          for (int i = 0; i < outputSamples.Length; ++i) {
            outputSamples[i] = 0.0f;
          }
          return;
        }
        if (BarelyInstrument_Process(instrumentPtr, OutputSamples, outputChannelCount,
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
      /// @param instrumentPtr Pointer to instrument.
      /// @param controlId Control identifier.
      public static void Instrument_ResetControl(IntPtr instrumentPtr, int controlId) {
        if (!BarelyInstrument_ResetControl(instrumentPtr, controlId) &&
            instrumentPtr != IntPtr.Zero) {
          Debug.LogError("Failed to reset instrument control " + controlId + " value");
        }
      }

      /// Resets an instrument note control value.
      ///
      /// @param instrumentPtr Pointer to instrument.
      /// @param pitch Note pitch.
      /// @param controlId Note control identifier.
      public static void Instrument_ResetNoteControl(IntPtr instrumentPtr, double pitch,
                                                     int controlId) {
        if (!BarelyInstrument_ResetNoteControl(instrumentPtr, pitch, controlId) &&
            instrumentPtr != IntPtr.Zero) {
          Debug.LogError("Failed to reset instrument note " + pitch + " control " + controlId +
                         " value");
        }
      }

      /// Sets all instrument notes off.
      ///
      /// @param instrumentPtr Pointer to instrument.
      public static void Instrument_SetAllNotesOff(IntPtr instrumentPtr) {
        if (!BarelyInstrument_SetAllNotesOff(instrumentPtr) && instrumentPtr != IntPtr.Zero) {
          Debug.LogError("Failed to stop all instrument notes");
        }
      }

      /// Sets an instrument control value.
      ///
      /// @param instrumentPtr Pointer to instrument.
      /// @param controlId Control identifier.
      /// @param value Control value.
      public static void Instrument_SetControl(IntPtr instrumentPtr, int controlId, double value) {
        if (!BarelyInstrument_SetControl(instrumentPtr, controlId, value) &&
            instrumentPtr != IntPtr.Zero) {
          Debug.LogError("Failed to set instrument control " + controlId + " value to " + value);
        }
      }

      /// Sets instrument data.
      ///
      /// @param instrumentPtr Pointer to instrument.
      /// @param dataPtr Pointer to data.
      /// @param size Data size in bytes.
      public static void Instrument_SetData(IntPtr instrumentPtr, IntPtr dataPtr, int size) {
        if (!BarelyInstrument_SetData(instrumentPtr, dataPtr, size) &&
            instrumentPtr != IntPtr.Zero) {
          Debug.LogError("Failed to set instrument data");
        }
      }

      /// Sets an instrument note control value.
      ///
      /// @param instrumentPtr Pointer to instrument.
      /// @param pitch Note pitch.
      /// @param controlId Note control identifier.
      /// @param value Note control value.
      public static void Instrument_SetNoteControl(IntPtr instrumentPtr, double pitch,
                                                   int controlId, double value) {
        if (!BarelyInstrument_SetNoteControl(instrumentPtr, pitch, controlId, value) &&
            instrumentPtr != IntPtr.Zero) {
          Debug.LogError("Failed to set instrument note " + pitch + " control " + controlId +
                         " value to " + value);
        }
      }

      /// Sets an instrument note off.
      ///
      /// @param instrumentPtr Pointer to instrument.
      /// @param pitch Note pitch.
      public static void Instrument_SetNoteOff(IntPtr instrumentPtr, double pitch) {
        if (!BarelyInstrument_SetNoteOff(instrumentPtr, pitch) && instrumentPtr != IntPtr.Zero) {
          Debug.LogError("Failed to stop instrument note " + pitch + "");
        }
      }

      /// Sets an instrument note on.
      ///
      /// @param instrumentPtr Pointer to instrument.
      /// @param pitch Note pitch.
      public static void Instrument_SetNoteOn(IntPtr instrumentPtr, double pitch,
                                              double intensity) {
        if (!BarelyInstrument_SetNoteOn(instrumentPtr, pitch, intensity) &&
            instrumentPtr != IntPtr.Zero) {
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
        if (!BarelyMusician_GetSecondsFromBeats(Ptr, beats, ref seconds) && _ptr != IntPtr.Zero) {
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
        if (!BarelyMusician_GetBeatsFromSeconds(Ptr, seconds, ref beats) && _ptr != IntPtr.Zero) {
          Debug.LogError("Failed to get musician beats for " + seconds + " seconds");
        }
        return beats;
      }

      /// Returns the tempo of a musician.
      ///
      /// @return Tempo in beats per minute.
      public static double Musician_GetTempo() {
        double tempo = 0.0;
        if (!BarelyMusician_GetTempo(Ptr, ref tempo) && _ptr != IntPtr.Zero) {
          Debug.LogError("Failed to get musician tempo");
        }
        return tempo;
      }

      /// Returns the timestamp of a musician.
      ///
      /// @return Timestamp in seconds.
      public static double Musician_GetTimestamp() {
        double timestamp = 0.0;
        if (!BarelyMusician_GetTimestamp(Ptr, ref timestamp) && _ptr != IntPtr.Zero) {
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
        if (!BarelyMusician_SetTempo(Ptr, tempo) && _ptr != IntPtr.Zero) {
          Debug.LogError("Failed to set musician tempo");
        }
      }

      /// Cancels all one-off tasks.
      ///
      /// @param performerPtr Pointer to performer.
      public static void Performer_CancelAllOneOffTasks(IntPtr performerPtr) {
        if (!BarelyPerformer_CancelAllOneOffTasks(performerPtr) && performerPtr != IntPtr.Zero) {
          Debug.LogError("Failed to cancel all one-off tasks");
        }
      }

      /// Creates a new performer.
      ///
      /// @param performer Performer.
      /// @param performerPtr Pointer to performer.
      public static void Performer_Create(Performer performer, ref IntPtr performerPtr) {
        if (Ptr == IntPtr.Zero || performerPtr != IntPtr.Zero) {
          return;
        }
        if (!BarelyPerformer_Create(Ptr, /*processOrder=*/0, ref performerPtr)) {
          Debug.LogError("Failed to create performer '" + performer.name + "'");
        }
      }

      /// Destroys a performer.
      ///
      /// @param performerPtr Pointer to performer.
      public static void Performer_Destroy(ref IntPtr performerPtr) {
        if (Ptr == IntPtr.Zero || performerPtr == IntPtr.Zero) {
          performerPtr = IntPtr.Zero;
          return;
        }
        if (!BarelyPerformer_Destroy(performerPtr)) {
          Debug.LogError("Failed to destroy performer");
        }
        performerPtr = IntPtr.Zero;
      }

      /// Returns the loop begin position of a performer.
      ///
      /// @param performerPtr Pointer to performer.
      /// @return Loop begin position in beats.
      public static double Performer_GetLoopBeginPosition(IntPtr performerPtr) {
        double loopBeginPosition = 0.0;
        if (!BarelyPerformer_GetLoopBeginPosition(performerPtr, ref loopBeginPosition) &&
            performerPtr != IntPtr.Zero) {
          Debug.LogError("Failed to get performer loop begin position");
        }
        return loopBeginPosition;
      }

      /// Returns the loop length of a performer.
      ///
      /// @param performerPtr Pointer to performer.
      /// @return Loop length in beats.
      public static double Performer_GetLoopLength(IntPtr performerPtr) {
        double loopLength = 0.0;
        if (!BarelyPerformer_GetLoopLength(performerPtr, ref loopLength) &&
            performerPtr != IntPtr.Zero) {
          Debug.LogError("Failed to get performer loop length");
        }
        return loopLength;
      }

      /// Returns the position of a performer.
      ///
      /// @param performerPtr Pointer to performer.
      /// @return Position in beats.
      public static double Performer_GetPosition(IntPtr performerPtr) {
        double position = 0.0;
        if (!BarelyPerformer_GetPosition(performerPtr, ref position) &&
            performerPtr != IntPtr.Zero) {
          Debug.LogError("Failed to get performer position");
        }
        return position;
      }

      /// Returns whether a performer is looping or not.
      ///
      /// @param performerPtr Pointer to performer.
      /// @return True if looping, false otherwise.
      public static bool Performer_IsLooping(IntPtr performerPtr) {
        bool isLooping = false;
        if (!BarelyPerformer_IsLooping(performerPtr, ref isLooping) &&
            performerPtr != IntPtr.Zero) {
          Debug.LogError("Failed to get if performer is looping");
        }
        return isLooping;
      }

      /// Returns whether a performer is playing or not.
      ///
      /// @param performerPtr Pointer to performer.
      /// @return True if playing, false otherwise.
      public static bool Performer_IsPlaying(IntPtr performerPtr) {
        bool isPlaying = false;
        if (!BarelyPerformer_IsPlaying(performerPtr, ref isPlaying) &&
            performerPtr != IntPtr.Zero) {
          Debug.LogError("Failed to get if performer is playing");
        }
        return isPlaying;
      }

      /// Schedules a one-off task.
      ///
      /// @param performerPtr Pointer to performer.
      /// @param callback Task callback.
      /// @param position Task position.
      public static void Performer_ScheduleOneOffTask(IntPtr performerPtr, Action callback,
                                                      double position) {
        if (Ptr == null || callback == null) {
          return;
        }
        GCHandle pointer = GCHandle.Alloc(callback);
        if (!BarelyPerformer_ScheduleOneOffTask(performerPtr, _taskDefinition, position,
                                                GCHandle.ToIntPtr(pointer)) &&
            performerPtr != IntPtr.Zero) {
          pointer.Free();
          Debug.LogError("Failed to set performer loop begin position");
        }
      }

      /// Sets the loop begin position of a performer.
      ///
      /// @param performerPtr Pointer to performer.
      /// @param loopBeginPosition Loop begin position in beats.
      public static void Performer_SetLoopBeginPosition(IntPtr performerPtr,
                                                        double loopBeginPosition) {
        if (!BarelyPerformer_SetLoopBeginPosition(performerPtr, loopBeginPosition) &&
            performerPtr != IntPtr.Zero) {
          Debug.LogError("Failed to set performer loop begin position");
        }
      }

      /// Sets the loop length of a performer.
      ///
      /// @param performerPtr Pointer to performer.
      /// @param loopLength Loop length in beats.
      public static void Performer_SetLoopLength(IntPtr performerPtr, double loopLength) {
        if (!BarelyPerformer_SetLoopLength(performerPtr, loopLength) &&
            performerPtr != IntPtr.Zero) {
          Debug.LogError("Failed to set performer loop length");
        }
      }

      /// Sets whether a performer is looping or not.
      ///
      /// @param performerPtr Pointer to performer.
      /// @param isLooping True if looping, false otherwise.
      public static void Performer_SetLooping(IntPtr performerPtr, bool isLooping) {
        if (!BarelyPerformer_SetLooping(performerPtr, isLooping) && performerPtr != IntPtr.Zero) {
          Debug.LogError("Failed to set performer looping");
        }
      }

      /// Sets the position of a performer.
      ///
      /// @param performerPtr Pointer to performer.
      /// @param position Position in beats.
      public static void Performer_SetPosition(IntPtr performerPtr, double position) {
        if (!BarelyPerformer_SetPosition(performerPtr, position) && performerPtr != IntPtr.Zero) {
          Debug.LogError("Failed to set performer position");
        }
      }

      /// Starts a performer.
      ///
      /// @param performerPtr Pointer to performer.
      public static void Performer_Start(IntPtr performerPtr) {
        if (!BarelyPerformer_Start(performerPtr) && performerPtr != IntPtr.Zero) {
          Debug.LogError("Failed to start performer");
        }
      }

      /// Stops a performer.
      ///
      /// @param performerPtr Pointer to performer.
      public static void Performer_Stop(IntPtr performerPtr) {
        if (!BarelyPerformer_Stop(performerPtr) && performerPtr != IntPtr.Zero) {
          Debug.LogError("Failed to stop performer");
        }
      }

      /// Creates a new task.
      ///
      /// @param performerPtr Pointer to performer.
      /// @param callback Task callback.
      /// @param position Task position.
      /// @param taskPtr Pointer to task.
      public static void Task_Create(IntPtr performerPtr, Action callback, double position,
                                     ref IntPtr taskPtr) {
        if (Ptr == IntPtr.Zero || taskPtr != IntPtr.Zero) {
          return;
        }
        GCHandle pointer = GCHandle.Alloc(callback);
        if (!BarelyTask_Create(performerPtr, _taskDefinition, position, GCHandle.ToIntPtr(pointer),
                               ref taskPtr)) {
          pointer.Free();
        }
      }

      /// Destroys a task.
      ///
      /// @param taskPtr Pointer to task.
      public static void Task_Destroy(ref IntPtr taskPtr) {
        if (Ptr == IntPtr.Zero || taskPtr == IntPtr.Zero) {
          taskPtr = IntPtr.Zero;
          return;
        }
        if (!BarelyTask_Destroy(taskPtr)) {
          Debug.LogError("Failed to destroy performer task");
        }
        taskPtr = IntPtr.Zero;
      }

      /// Returns the position of a task.
      ///
      /// @param taskPtr Pointer to task.
      /// @return Position in beats.
      public static double Task_GetPosition(IntPtr taskPtr) {
        double position = 0.0;
        if (!BarelyTask_GetPosition(taskPtr, ref position) && taskPtr != IntPtr.Zero) {
          Debug.LogError("Failed to get performer task position");
        }
        return position;
      }

      /// Sets the position of a task.
      ///
      /// @param taskPtr Pointer to task.
      /// @param position Position in beats.
      public static void Task_SetPosition(IntPtr taskPtr, double position) {
        if (!BarelyTask_SetPosition(taskPtr, position) && taskPtr != IntPtr.Zero) {
          Debug.LogError("Failed to set performer task position");
        }
      }

      /// Returns whether an arpeggiator note is on or not.
      ///
      /// @param arpeggiatorPtr Pointer to arpeggiator.
      /// @param pitch Note pitch.
      /// @return True if on, false otherwise.
      public static bool Arpeggiator_IsNoteOn(IntPtr arpeggiatorPtr, double pitch) {
        bool isNoteOn = false;
        if (!BarelyArpeggiator_IsNoteOn(arpeggiatorPtr, pitch, ref isNoteOn) &&
            arpeggiatorPtr != IntPtr.Zero) {
          Debug.LogError("Failed to get if arpeggiator note " + pitch + " is on");
        }
        return isNoteOn;
      }

      /// Returns whether an arpeggiator is playing or not.
      ///
      /// @param arpeggiatorPtr Pointer to arpeggiator.
      /// @return True if playing, false otherwise.
      public static bool Arpeggiator_IsPlaying(IntPtr arpeggiatorPtr) {
        bool isPlaying = false;
        if (!BarelyArpeggiator_IsPlaying(arpeggiatorPtr, ref isPlaying) &&
            arpeggiatorPtr != IntPtr.Zero) {
          Debug.LogError("Failed to get if arpeggiator is playing");
        }
        return isPlaying;
      }

      /// Sets all arpeggiator notes off.
      ///
      /// @param arpeggiatorPtr Pointer to arpeggiator.
      public static void Arpeggiator_SetAllNotesOff(IntPtr arpeggiatorPtr) {
        if (!BarelyArpeggiator_SetAllNotesOff(arpeggiatorPtr) && arpeggiatorPtr != IntPtr.Zero) {
          Debug.LogError("Failed to stop all arpeggiator notes");
        }
      }

      /// Sets an arpeggiator gate ratio.
      ///
      /// @param arpeggiatorPtr Pointer to arpeggiator.
      /// @param gateRatio Gate ratio.
      public static void Arpeggiator_SetGateRatio(IntPtr arpeggiatorPtr, double gateRatio) {
        if (!BarelyArpeggiator_SetGateRatio(arpeggiatorPtr, gateRatio) &&
            arpeggiatorPtr != IntPtr.Zero) {
          Debug.LogError("Failed to stop arpeggiator gate ratio " + gateRatio);
        }
      }

      /// Sets an arpeggiator instrument.
      ///
      /// @param arpeggiatorPtr Pointer to arpeggiator.
      /// @param instrument Instrument.
      public static void Arpeggiator_SetInstrument(IntPtr arpeggiatorPtr, Instrument instrument) {
        if (!BarelyArpeggiator_SetInstrument(arpeggiatorPtr,
                                             Instrument.Internal.GetPtr(instrument)) &&
            arpeggiatorPtr != IntPtr.Zero) {
          Debug.LogError("Failed to set arpeggiator instrument '" + instrument.name + "'");
        }
      }

      /// Sets an arpeggiator note off.
      ///
      /// @param arpeggiatorPtr Pointer to arpeggiator.
      /// @param pitch Note pitch.
      public static void Arpeggiator_SetNoteOff(IntPtr arpeggiatorPtr, double pitch) {
        if (!BarelyArpeggiator_SetNoteOff(arpeggiatorPtr, pitch) && arpeggiatorPtr != IntPtr.Zero) {
          Debug.LogError("Failed to stop arpeggiator note " + pitch);
        }
      }

      /// Sets an arpeggiator note on.
      ///
      /// @param arpeggiatorPtr Pointer to arpeggiator.
      /// @param pitch Note pitch.
      public static void Arpeggiator_SetNoteOn(IntPtr arpeggiatorPtr, double pitch) {
        if (!BarelyArpeggiator_SetNoteOn(arpeggiatorPtr, pitch) && arpeggiatorPtr != IntPtr.Zero) {
          Debug.LogError("Failed to start arpeggiator note " + pitch);
        }
      }

      /// Sets an arpeggiator rate.
      ///
      /// @param arpeggiatorPtr Pointer to arpeggiator.
      /// @param rate Rate in notes per beat.
      public static void Arpeggiator_SetRate(IntPtr arpeggiatorPtr, double rate) {
        if (!BarelyArpeggiator_SetRate(arpeggiatorPtr, rate) && arpeggiatorPtr != IntPtr.Zero) {
          Debug.LogError("Failed to set arpeggiator rate " + rate);
        }
      }

      /// Sets an arpeggiator style.
      ///
      /// @param arpeggiatorPtr Pointer to arpeggiator.
      /// @param style Style.
      public static void Arpeggiator_SetStyle(IntPtr arpeggiatorPtr, ArpeggiatorStyle style) {
        if (!BarelyArpeggiator_SetStyle(arpeggiatorPtr, style) && arpeggiatorPtr != IntPtr.Zero) {
          Debug.LogError("Failed to set arpeggiator style " + style);
        }
      }

      /// Returns whether an repeater is playing or not.
      ///
      /// @param repeaterPtr Pointer to repeater.
      /// @return True if playing, false otherwise.
      public static bool Repeater_IsPlaying(IntPtr repeaterPtr) {
        bool isPlaying = false;
        if (!BarelyRepeater_IsPlaying(repeaterPtr, ref isPlaying) && repeaterPtr != IntPtr.Zero) {
          Debug.LogError("Failed to get if repeater is playing");
        }
        return isPlaying;
      }

      /// Pops the last note from the end.
      ///
      /// @param repeaterPtr Pointer to repeater.
      public static void Repeater_Pop(IntPtr repeaterPtr) {
        if (!BarelyRepeater_Pop(repeaterPtr) && repeaterPtr != IntPtr.Zero) {
          Debug.LogError("Failed to pop from repeater");
        }
      }

      /// Pushes a new note to the end.
      ///
      /// @param repeaterPtr Pointer to repeater.
      /// @param pitchOr Note pitch value or silence.
      /// @param length Note length in beats.
      public static void Repeater_Push(IntPtr repeaterPtr, double? pitchOr, int length) {
        if ((pitchOr.HasValue && !BarelyRepeater_Push(repeaterPtr, pitchOr.Value, length)) ||
            (!pitchOr.HasValue && !BarelyRepeater_PushSilence(repeaterPtr, length)) &&
                repeaterPtr != IntPtr.Zero) {
          Debug.LogError("Failed to pop from repeater");
        }
      }

      /// Sets an repeater instrument.
      ///
      /// @param repeaterPtr Pointer to repeater.
      /// @param instrument Instrument.
      public static void Repeater_SetInstrument(IntPtr repeaterPtr, Instrument instrument) {
        if (!BarelyRepeater_SetInstrument(repeaterPtr, Instrument.Internal.GetPtr(instrument)) &&
            repeaterPtr != IntPtr.Zero) {
          Debug.LogError("Failed to set repeater instrument '" + instrument.name + "'");
        }
      }

      /// Sets an repeater rate.
      ///
      /// @param repeaterPtr Pointer to repeater.
      /// @param rate Rate in notes per beat.
      public static void Repeater_SetRate(IntPtr repeaterPtr, double rate) {
        if (!BarelyRepeater_SetRate(repeaterPtr, rate) && repeaterPtr != IntPtr.Zero) {
          Debug.LogError("Failed to set repeater rate " + rate);
        }
      }

      /// Sets an repeater style.
      ///
      /// @param repeaterPtr Pointer to repeater.
      /// @param style Style.
      public static void Repeater_SetStyle(IntPtr repeaterPtr, RepeaterStyle style) {
        if (!BarelyRepeater_SetStyle(repeaterPtr, style) && repeaterPtr != IntPtr.Zero) {
          Debug.LogError("Failed to set repeater style " + style);
        }
      }

      /// Starts a repeater.
      ///
      /// @param repeaterPtr Pointer to repeater.
      /// @param pitchOffset Pitch offset.
      public static void Repeater_Start(IntPtr repeaterPtr, double pitchOffset) {
        if (!BarelyRepeater_Start(repeaterPtr, pitchOffset) && repeaterPtr != IntPtr.Zero) {
          Debug.LogError("Failed to start repeater with a pitch offset" + pitchOffset);
        }
      }

      /// Stops a repeater.
      ///
      /// @param repeaterPtr Pointer to repeater.
      public static void Repeater_Stop(IntPtr repeaterPtr) {
        if (!BarelyRepeater_Stop(repeaterPtr) && repeaterPtr != IntPtr.Zero) {
          Debug.LogError("Failed to stop repeater");
        }
      }

      // Internal output samples.
      public static double[] OutputSamples { get; private set; } = null;

      // Control event definition create callback.
      private delegate void ControlEventDefinition_CreateCallback(ref IntPtr state,
                                                                  IntPtr userData);
      [AOT.MonoPInvokeCallback(typeof(ControlEventDefinition_CreateCallback))]
      private static void ControlEventDefinition_OnCreate(ref IntPtr state, IntPtr userData) {
        state = userData;
      }

      // Control event definition destroy callback.
      private delegate void ControlEventDefinition_DestroyCallback(ref IntPtr state);
      [AOT.MonoPInvokeCallback(typeof(ControlEventDefinition_DestroyCallback))]
      private static void ControlEventDefinition_OnDestroy(ref IntPtr state) {
        GCHandle.FromIntPtr(state).Free();
      }

      // Control event definition process callback.
      private delegate void ControlEventDefinition_ProcessCallback(ref IntPtr state, int id,
                                                                   double value);
      [AOT.MonoPInvokeCallback(typeof(ControlEventDefinition_ProcessCallback))]
      private static void ControlEventDefinition_OnProcess(ref IntPtr state, int id, double value) {
        Instrument instrument = GCHandle.FromIntPtr(state).Target as Instrument;
        Instrument.Internal.OnControlEvent(instrument, id, value);
      }

      // Control event definition.
      [StructLayout(LayoutKind.Sequential)]
      private struct ControlEventDefinition {
        // Create callback.
        public ControlEventDefinition_CreateCallback createCallback;

        // Destroy callback.
        public ControlEventDefinition_DestroyCallback destroyCallback;

        // Process callback.
        public ControlEventDefinition_ProcessCallback processCallback;
      }

      // Note control event definition create callback.
      private delegate void NoteControlEventDefinition_CreateCallback(ref IntPtr state,
                                                                      IntPtr userData);
      [AOT.MonoPInvokeCallback(typeof(NoteControlEventDefinition_CreateCallback))]
      private static void NoteControlEventDefinition_OnCreate(ref IntPtr state, IntPtr userData) {
        state = userData;
      }

      // Note control event definition destroy callback.
      private delegate void NoteControlEventDefinition_DestroyCallback(ref IntPtr state);
      [AOT.MonoPInvokeCallback(typeof(NoteControlEventDefinition_DestroyCallback))]
      private static void NoteControlEventDefinition_OnDestroy(ref IntPtr state) {
        GCHandle.FromIntPtr(state).Free();
      }

      // Note control event definition process callback.
      private delegate void NoteControlEventDefinition_ProcessCallback(ref IntPtr state,
                                                                       double pitch, int id,
                                                                       double value);
      [AOT.MonoPInvokeCallback(typeof(NoteControlEventDefinition_ProcessCallback))]
      private static void NoteControlEventDefinition_OnProcess(ref IntPtr state, double pitch,
                                                               int id, double value) {
        Instrument instrument = GCHandle.FromIntPtr(state).Target as Instrument;
        Instrument.Internal.OnNoteControlEvent(instrument, pitch, id, value);
      }

      // Note control event definition.
      [StructLayout(LayoutKind.Sequential)]
      private struct NoteControlEventDefinition {
        // Create callback.
        public NoteControlEventDefinition_CreateCallback createCallback;

        // Destroy callback.
        public NoteControlEventDefinition_DestroyCallback destroyCallback;

        // Process callback.
        public NoteControlEventDefinition_ProcessCallback processCallback;
      }

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

      // Singleton musician pointer.
      private static IntPtr Ptr {
        get {
          if (_isShuttingDown) {
            return IntPtr.Zero;
          }
          if (_ptr == IntPtr.Zero) {
            var state =
                new GameObject() { hideFlags = HideFlags.HideAndDontSave }.AddComponent<State>();
            GameObject.DontDestroyOnLoad(state.gameObject);
            if (_ptr == IntPtr.Zero) {
              GameObject.DestroyImmediate(state.gameObject);
              _isShuttingDown = true;
            }
          }
          return _ptr;
        }
      }
      private static IntPtr _ptr = IntPtr.Zero;

      // Control event definition.
      private static ControlEventDefinition _controlEventDefinition = new ControlEventDefinition() {
        createCallback = ControlEventDefinition_OnCreate,
        destroyCallback = ControlEventDefinition_OnDestroy,
        processCallback = ControlEventDefinition_OnProcess,
      };

      // Note control event definition.
      private static NoteControlEventDefinition _noteControlEventDefinition =
          new NoteControlEventDefinition() {
            createCallback = NoteControlEventDefinition_OnCreate,
            destroyCallback = NoteControlEventDefinition_OnDestroy,
            processCallback = NoteControlEventDefinition_OnProcess,
          };

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
            BarelyMusician_Update(_ptr, taskTimestamp);
            var callbacks = _scheduledTaskCallbacks.ElementAt(0).Value;
            for (int i = 0; i < callbacks.Count; ++i) {
              callbacks[i]?.Invoke();
            }
            _scheduledTaskCallbacks.Remove(taskTimestamp);
          }
          BarelyMusician_Update(_ptr, nextTimestamp);
        }

        // Initializes the native state.
        private void Initialize() {
          _isShuttingDown = false;
          var config = AudioSettings.GetConfiguration();
          if (!BarelyMusician_Create(config.sampleRate, /*C4=*/261.625565301, ref _ptr)) {
            Debug.LogError("Failed to initialize BarelyMusician");
            return;
          }
          BarelyMusician_SetTempo(_ptr, _tempo);
          OutputSamples = new double[config.dspBufferSize * (int)config.speakerMode];
          _latency = (double)config.dspBufferSize / config.sampleRate;
          _scheduledTaskCallbacks = new SortedDictionary<double, List<Action>>();
          BarelyMusician_Update(_ptr, GetNextTimestamp());
        }

        // Shuts down the native state.
        private void Shutdown() {
          _isShuttingDown = true;
          BarelyMusician_Destroy(_ptr);
          _ptr = IntPtr.Zero;
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

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_Create")]
      private static extern bool BarelyInstrument_Create(IntPtr musician, ref IntPtr outInstrument);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_Destroy")]
      private static extern bool BarelyInstrument_Destroy(IntPtr instrument);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_GetControl")]
      private static extern bool BarelyInstrument_GetControl(IntPtr instrument, Int32 id,
                                                             ref double outValue);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_GetNoteControl")]
      private static extern bool BarelyInstrument_GetNoteControl(IntPtr instrument, double pitch,
                                                                 Int32 id, ref double outValue);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_IsNoteOn")]
      private static extern bool BarelyInstrument_IsNoteOn(IntPtr instrument, double pitch,
                                                           ref bool outIsNoteOn);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_Process")]
      private static extern bool BarelyInstrument_Process(IntPtr instrument,
                                                          [In, Out] double[] outputSamples,
                                                          Int32 outputChannelCount,
                                                          Int32 outputFrameCount, double timestamp);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_ResetControl")]
      private static extern bool BarelyInstrument_ResetControl(IntPtr instrument, Int32 id);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_ResetNoteControl")]
      private static extern bool BarelyInstrument_ResetNoteControl(IntPtr instrument, double pitch,
                                                                   Int32 id);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_SetAllNotesOff")]
      private static extern bool BarelyInstrument_SetAllNotesOff(IntPtr instrument);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_SetControl")]
      private static extern bool BarelyInstrument_SetControl(IntPtr instrument, Int32 id,
                                                             double value);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_SetControlEvent")]
      private static extern bool BarelyInstrument_SetControlEvent(IntPtr instrument,
                                                                  ControlEventDefinition definition,
                                                                  IntPtr userData);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_SetData")]
      private static extern bool BarelyInstrument_SetData(IntPtr instrument, IntPtr data,
                                                          Int32 size);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_SetNoteControl")]
      private static extern bool BarelyInstrument_SetNoteControl(IntPtr instrument, double pitch,
                                                                 Int32 id, double value);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_SetNoteControlEvent")]
      private static extern bool BarelyInstrument_SetNoteControlEvent(
          IntPtr instrument, NoteControlEventDefinition definition, IntPtr userData);

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

      [DllImport(pluginName, EntryPoint = "BarelyMusician_SetTempo")]
      private static extern bool BarelyMusician_SetTempo(IntPtr musician, double tempo);

      [DllImport(pluginName, EntryPoint = "BarelyMusician_Update")]
      private static extern bool BarelyMusician_Update(IntPtr musician, double timestamp);

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_CancelAllOneOffTasks")]
      private static extern bool BarelyPerformer_CancelAllOneOffTasks(IntPtr performer);

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_Create")]
      private static extern bool BarelyPerformer_Create(IntPtr musician, Int32 processOrder,
                                                        ref IntPtr outPerformer);

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_Destroy")]
      private static extern bool BarelyPerformer_Destroy(IntPtr performer);

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

      [DllImport(pluginName, EntryPoint = "BarelyTask_Create")]
      private static extern bool BarelyTask_Create(IntPtr performer, TaskDefinition definition,
                                                   double position, IntPtr userData,
                                                   ref IntPtr outTask);

      [DllImport(pluginName, EntryPoint = "BarelyTask_Destroy")]
      private static extern bool BarelyTask_Destroy(IntPtr task);

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
