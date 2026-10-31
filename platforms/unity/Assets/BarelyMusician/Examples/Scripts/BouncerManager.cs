using UnityEngine;

namespace Barely.Examples {
  public class BouncerManager : MonoBehaviour {
    public GameObject bouncerPrefab;

    public bool shouldAutoGenerate = false;

    [Min(0.0f)]
    public float spawnLatency = 1.0f;

    private void Update() {
      if (((Application.platform == RuntimePlatform.Android ||
            Application.platform == RuntimePlatform.IPhonePlayer) &&
           Input.GetMouseButtonDown(0)) ||
          Input.GetKeyDown(KeyCode.Space)) {
        InstantiateNewBouncer();
      }
      if (shouldAutoGenerate && !IsInvoking("InstantiateNewBouncer")) {
        InvokeRepeating("InstantiateNewBouncer", 0.0f, spawnLatency);
      } else if (!shouldAutoGenerate && IsInvoking("InstantiateNewBouncer")) {
        CancelInvoke();
      }
    }

    private void InstantiateNewBouncer() {
      GameObject.Instantiate(bouncerPrefab);
    }
  }
}  // namespace Barely.Examples
