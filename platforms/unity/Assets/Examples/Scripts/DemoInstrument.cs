using System.Collections;
using System.Collections.Generic;
using Barely;
using UnityEngine;

public class DemoInstrument : CustomInstrument {
  protected override ParameterDefinition[] GetParameterDefinitions() {
    return null;
  }

  protected override void ProcessCallback(double[] output, int size) {}

  protected override void SetDataCallback(byte[] data) {}

  protected override void SetNoteOffCallback(double pitch) {}

  protected override void SetNoteOnCallback(double pitch, double intensity) {}

  protected override void SetParameterCallback(int index, double value) {}
}
