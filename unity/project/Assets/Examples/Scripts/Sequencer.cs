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

    public float index;

    public float intensity;
  }

  public Instrument instrument;

  [Range(0.0f, 128.0f)]
  public float rootIndex = 60.0f;

  public double start = 0.0;

  // Length in beats.
  [Range(0, 32)]
  public int length = 4;

  public bool loop = true;

  public Note[] notes;

  private void OnEnable() {
    BarelyMusician.OnBeat += OnBeat;
  }
  private void OnDisable() {
    BarelyMusician.OnBeat -= OnBeat;
  }

  void OnBeat(double dspTime, int beat) {
    double startPosition = (double)beat;
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
        float index = note.index + rootIndex;
        instrument.ScheduleNote(position, note.duration, index, note.intensity);
      }
    }
  }

}
