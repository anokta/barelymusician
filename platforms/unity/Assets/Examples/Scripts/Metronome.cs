using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using BarelyApi;

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
    BarelyMusician.OnPlaybackBeat += OnPlaybackBeat;
  }

  void OnDisable() {
    BarelyMusician.OnPlaybackBeat -= OnPlaybackBeat;
  }

  void Update() {
    BarelyMusician.SetPlaybackTempo(tempo);
    if (Input.GetKeyDown(KeyCode.Space)) {
      if (BarelyMusician.IsPlaying()) {
        BarelyMusician.StopPlayback();
        if (logToConsole) {
          Debug.Log("Playback stopped");
        }
      } else {
        BarelyMusician.StartPlayback();
        if (logToConsole) {
          Debug.Log("Playback started");
        }
      }
    } else if (Input.GetKeyDown(KeyCode.Backspace)) {
      BarelyMusician.StopPlayback();
      BarelyMusician.SetPlaybackPosition(0.0);
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
