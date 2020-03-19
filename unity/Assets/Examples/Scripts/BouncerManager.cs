﻿using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BouncerManager : MonoBehaviour {
  public GameObject bouncerPrefab;

  public bool shouldAutoGenerate = false;

  public float spawnLatency = 1.0f;

  private void Start() {
    if (shouldAutoGenerate) {
      InvokeRepeating("InstantiateNewBouncer", 0.0f, spawnLatency);
    }
  }

  private void Update() {
    if (Input.GetKeyDown(KeyCode.Space)) {
      InstantiateNewBouncer();
    }
  }

  private void InstantiateNewBouncer() {
    GameObject.Instantiate(bouncerPrefab);
  }
}