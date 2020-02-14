using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using BarelyApi;

// Example class that plays instrument.
public class InstrumentPlayer : MonoBehaviour {
  // Note.
  [Range(0.0f, 128.0f)]
  public float rootIndex = 60.0f;

  // Instrument to play.
  private Instrument instrument = null;

  [System.Serializable]
  public struct Note {
    [Range(-24.0f, 24.0f)]
    public float index;
    [Range(0.0f, 1.0f)]
    public float intensity;
    [Range(0.0f, 1.0f)]
    public float position;
    [Range(0.0f, 1.0f)]
    public float duration;

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

  void OnEnable() {
    BarelyMusician.OnBeat += OnBeat;
  }

  void OnDisable() {
    BarelyMusician.OnBeat -= OnBeat;
  }

  void Update() {
    if (!interactive) return;
    if (Input.GetKeyDown(KeyCode.S) && !isPlaying) {
      instrument.NoteOn(rootIndex, 1.0f);
      isPlaying = true;
    } else if (Input.GetKeyUp(KeyCode.S) && isPlaying) {
      instrument.NoteOff(rootIndex);
      isPlaying = false;
    }
  }

  void OnBeat(int beat) {
    for (int i = 0; i < beatNotes.Length; ++i) {
      var note = beatNotes[i];
      var index = rootIndex + note.index + note.beatMultiplier * (beat % note.beatPeriod);
      float startPosition = note.position + beat + 1.0f;
      instrument.ScheduleNoteOn(startPosition, index, note.intensity);
      float endPosition = startPosition + note.duration;
      instrument.ScheduleNoteOff(endPosition, index);
    }
  }
}
