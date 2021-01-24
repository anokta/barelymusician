using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using BarelyApi;

// Example class that plays instrument.
public class InstrumentPlayer : MonoBehaviour {
  // Note.
  [Range(0.0f, 128.0f)]
  public float rootPitch = 60.0f;

  // Instrument to play.
  private Instrument instrument = null;

  [System.Serializable]
  public struct Note {
    [Range(-24.0f, 24.0f)]
    public float pitch;
    [Range(0.0f, 1.0f)]
    public float intensity;
    [Range(0.0f, 1.0f)]
    public double position;
    [Range(0.0f, 1.0f)]
    public double duration;

    [Range(0, 12)]
    public int beatMultiplier;
    [Range(1, 8)]
    public int beatPeriod;
  }
  public Note[] beatNotes;

  public bool interactive = false;

  // Is playing?
  private bool isPlaying = false;

  private void Awake() {
    instrument = GetComponent<Instrument>();
  }

  private void OnEnable() {
    BarelyMusician.OnBeat += OnBeat;
  }

  private void OnDisable() {
    BarelyMusician.OnBeat -= OnBeat;
  }

  private void Update() {
    if (!interactive) return;
    if (Input.GetKeyDown(KeyCode.Alpha1) && !isPlaying) {
      instrument.SetNoteOn(rootPitch, 1.0f);
      isPlaying = true;
    } else if (Input.GetKeyUp(KeyCode.Alpha1) && isPlaying) {
      instrument.SetNoteOff(rootPitch);
      isPlaying = false;
    }
  }

  private void OnBeat(double dspTime, int beat) {
    for (int i = 0; i < beatNotes.Length; ++i) {
      var note = beatNotes[i];
      double position = note.position + (double)(beat);
      float pitch =
          rootPitch + note.pitch + (float)(note.beatMultiplier * (beat % note.beatPeriod));
      instrument.ScheduleNote(position, note.duration, pitch, note.intensity);
    }
  }
}
