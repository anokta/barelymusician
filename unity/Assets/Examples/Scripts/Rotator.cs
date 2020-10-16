using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using BarelyApi;

public class Rotator : MonoBehaviour {
  public Vector3 axis;

  private void Update() {
    float angle = 0.5f * Mathf.PI * Mathf.Rad2Deg * (float)BarelyApi.BarelyMusician.GetPosition();
    transform.rotation = Quaternion.AngleAxis(angle, axis.normalized);
  }
}
