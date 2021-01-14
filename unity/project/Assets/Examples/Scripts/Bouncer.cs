using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using BarelyApi;

public class Bouncer : MonoBehaviour {
  public Instrument instrument;

  public float rootNote = 64.0f;

  private readonly float[] MajorScale = new float[] { 0.0f, 2.0f, 4.0f, 5.0f, 7.0f, 9.0f, 11.0f };

  private readonly float NoteDuration = 0.05f;

  private int lastIndex = 0;

  private void Start() {
    Vector3 force = new Vector3(Random.Range(-1.0f, 1.0f), Random.Range(0.0f, 2.0f), 0.0f);
    //transform.GetComponent<Rigidbody>().AddForce(force, ForceMode.Impulse);
  }

  private void Update() {
    if (transform.position.y < -10.0f) {
      GameObject.Destroy(gameObject);
    }
  }

  private void OnCollisionEnter(Collision collision) {
    float octaveOffset = 12.0f * (lastIndex / MajorScale.Length);
    float pitch = rootNote + octaveOffset + MajorScale[lastIndex++ % MajorScale.Length];
    float intensity = Mathf.Min(1.0f, 0.125f * collision.relativeVelocity.sqrMagnitude);
    StartCoroutine(PlayNote(pitch, intensity, NoteDuration));
  }

  IEnumerator PlayNote(float pitch, float intensity, float duration) {
    instrument.NoteOn(pitch, intensity);
    yield return new WaitForSeconds(duration);
    instrument.NoteOff(pitch);
  }

}
