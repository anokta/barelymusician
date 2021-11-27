using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Barely;

/// Simple metronome that controls the playback.
public class Metronome : MonoBehaviour {
  [Range(30.0f, 960.0f)]
  public double tempo = 120.0;

  public Instrument instrument;

  [Range(1, 16)]
  public int numBeats = 4;

  [Range(-4.0f, 4.0f)]
  public float barPitch = 1.0f;

  [Range(-4.0f, 4.0f)]
  public float beatPitch = 0.0f;

  [Range(0.0f, 1.0f)]
  public float intensity = 1.0f;

  public bool logToConsole = true;

  public bool tick = true;

  void OnEnable() {
    Musician.OnPlaybackBeat += OnPlaybackBeat;
  }

  void OnDisable() {
    Musician.OnPlaybackBeat -= OnPlaybackBeat;
  }

  void Update() {
    Musician.SetPlaybackTempo(tempo);
    if (Input.GetKeyDown(KeyCode.Space)) {
      if (Musician.IsPlaying()) {
        Musician.StopPlayback();
        if (logToConsole) {
          Debug.Log("Playback stopped");
        }
      } else {
        Musician.StartPlayback();
        if (logToConsole) {
          Debug.Log("Playback started");
        }
      }
    } else if (Input.GetKeyDown(KeyCode.Backspace)) {
      Musician.StopPlayback();
      Musician.SetPlaybackPosition(0.0);
    }
  }

  private void OnPlaybackBeat(double position) {
    int bar = (int)(position) / numBeats;
    int beat = (int)(position) % numBeats;
    float pitch = (beat == 0.0) ? barPitch : beatPitch;
    if (tick && instrument) {
      instrument.SetNoteOn(pitch, intensity);
      instrument.SetNoteOff(pitch);
    }
    if (logToConsole) {
      Debug.Log("Tick " + bar + "." + beat);
    }
  }
}
