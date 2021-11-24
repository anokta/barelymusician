using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using BarelyApi;

public class Sequencer : MonoBehaviour {
  [System.Serializable]
  public struct Note {
    // Position in beats.
    public double position;

    // Duration in beats.
    public double duration;

    public int note;

    public float intensity;
  }

  public Instrument instrument;

  [Range(0, 128)]
  public int rootNote = 60;

  public double start = 0.0;

  // Length in beats.
  [Range(0, 32)]
  public int length = 4;

  public bool loop = true;

  public Note[] notes;

  private Clock clock;

  void Start() {
    clock = GameObject.FindObjectOfType<Clock>();
  }

  public void OnBeat(double beat) {
    double startPosition = beat;
    if (startPosition < start || (startPosition >= start + (double)length && !loop)) {
      return;
    }
    double noteOffset = start;
    if (loop) {
      noteOffset += length * (double)((int)(startPosition - start) / length);
      startPosition -= noteOffset;
    }
    foreach (var note in notes) {
      if (note.position >= startPosition && note.position < startPosition + 1.0) {
        double position = note.position + noteOffset;
        float pitch = (float)(note.note + rootNote - 69) / 12.0f;
        // instrument.ScheduleNoteOn(clock.GetTimestamp(position), pitch, note.intensity);
        // instrument.ScheduleNoteOff(clock.GetTimestamp(position + note.duration), pitch);
      }
    }
  }

}
