using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Barely;

public class Bouncer : MonoBehaviour {
  public Instrument instrument;

  public double rootPitch = 0.0;

  private readonly double[] MajorScale = new double[] { 0.0, 2.0, 4.0, 5.0, 7.0, 9.0, 11.0 };

  private readonly double NoteDuration = 0.05;

  private int lastIndex = 0;

  private void Start() {
    Vector3 force = new Vector3(Random.Range(-1.0f, 1.0f), Random.Range(0.0f, 2.0f), 0.0f);
    // transform.GetComponent<Rigidbody>().AddForce(force, ForceMode.Impulse);
  }

  private void Update() {
    if (transform.position.y < -10.0f) {
      GameObject.Destroy(gameObject);
    }
  }

  private void OnCollisionEnter(Collision collision) {
    double octaveOffset = lastIndex / MajorScale.Length;
    double pitch = rootPitch + octaveOffset + MajorScale[lastIndex++ % MajorScale.Length] / 12.0;
    double intensity = (double)Mathf.Min(1.0f, 0.125f * collision.relativeVelocity.sqrMagnitude);
    StartCoroutine(PlayNote(pitch, intensity, NoteDuration));
  }

  IEnumerator PlayNote(double pitch, double intensity, double duration) {
    instrument.SetNoteOn(pitch, intensity);
    yield return new WaitForSeconds((float)duration);
    instrument.SetNoteOff(pitch);
  }
}
