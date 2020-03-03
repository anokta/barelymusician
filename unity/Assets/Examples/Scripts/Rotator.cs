using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using BarelyApi;

public class Rotator : MonoBehaviour {
  private void Update() {
    float angle = 0.5f * Mathf.PI * Mathf.Rad2Deg * (float)BarelyApi.BarelyMusician.GetPosition();
    Vector3 axis = Vector3.one;
    transform.rotation = Quaternion.AngleAxis(angle, axis);
  }
}
