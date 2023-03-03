﻿using UnityEngine;

namespace Barely {
  namespace Examples {
    public class BouncerManager : MonoBehaviour {
      public GameObject bouncerPrefab;

      public bool shouldAutoGenerate = false;

      [Min(0.0f)]
      public float spawnLatency = 1.0f;

      private void Start() {
        if (shouldAutoGenerate) {
          InvokeRepeating("InstantiateNewBouncer", 0.0f, spawnLatency);
        }
      }

      private void Update() {
        if (((Application.platform == RuntimePlatform.Android ||
              Application.platform == RuntimePlatform.IPhonePlayer) &&
             Input.GetMouseButtonDown(0)) ||
            Input.GetKeyDown(KeyCode.Space)) {
          InstantiateNewBouncer();
        }
      }

      private void InstantiateNewBouncer() {
        GameObject.Instantiate(bouncerPrefab);
      }
    }
  }  // namespace Examples
}  // namespace Barely
