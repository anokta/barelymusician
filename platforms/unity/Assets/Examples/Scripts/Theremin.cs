using UnityEngine;
using Barely;

/// Theremin definition.
public class ThereminDefinition : CustomInstrumentDefinition {
  private double _frequency = 0.0;
  private double _amplitude = 0.0;
  private bool _isOn = false;
  private double _frameInterval = 0.0;
  private double _phase = 0.0;
  private double _targetFrequency = 0.0;
  private double _targetAmplitude = 0.0;

  public void OnCreate(int frameRate) {
    _frameInterval = 1.0 / (double)frameRate;
  }

  public void OnDestroy() {}

  public void OnProcess(double[] outputSamples, int outputChannelCount, int outputFrameCount) {
    for (int frame = 0; frame < outputFrameCount; ++frame) {
      _amplitude = Lerp(_amplitude, _targetAmplitude, 25.0 * _frameInterval);
      _frequency = Lerp(_frequency, _targetFrequency, 100.0 * _frameInterval);
      double monoSample = _amplitude * System.Math.Sin(_phase * 2.0 * System.Math.PI);
      for (int channel = 0; channel < outputChannelCount; ++channel) {
        outputSamples[frame * outputChannelCount + channel] = monoSample;
      }
      _phase += _frequency * _frameInterval;
      if (_phase >= 1.0) {
        _phase -= 1.0;
      }
    }
  }

  public void OnSetControl(int index, double value, double slopePerBeat) {
    switch (index) {
      case 0:
        _targetFrequency = GetFrequency(value);
        break;
      case 1:
        _targetAmplitude = _isOn ? value : 0.0;
        break;
    }
  }

  public void OnSetData(System.IntPtr dataPtr, int size) {
    Debug.LogWarning("Theremin does not implement OnSetData");
  }

  public void OnSetNoteControl(double pitch, int index, double value, double slopePerBeat) {
    Debug.LogWarning("Theremin does not implement OnSetNoteControl");
  }

  public void OnSetNoteOff(double pitch) {
    _isOn = false;
    _targetAmplitude = 0.0;
  }

  public void OnSetNoteOn(double pitch, double intensity) {
    _isOn = true;
    _phase = 0.0;
    _targetAmplitude = intensity;
  }

  // Middle A (A4) frequency.
  private const double FrequencyA4 = 440.0;

  // Returns the corresponding frequency for a given pitch.
  private double GetFrequency(double pitch) {
    return FrequencyA4 * System.Math.Pow(2.0, pitch);
  }

  // Linearly interpolates between given points.
  private double Lerp(double a, double b, double t) {
    return a + t * (b - a);
  }
}

// A simple theremin that is contolled by the mouse position on the screen.
public class Theremin : CustomInstrument<ThereminDefinition> {
  public Texture2D texture = null;
  public Color color = Color.white;

  public void OnGUI() {
    color.a = Mathf.Lerp(color.a, IsNoteOn(0.0) ? 0.25f : 0.0f, 8.0f * Time.deltaTime);
    GUI.color = color;
    float size = 0.1f * Mathf.Max(Screen.width, Screen.height);
    GUI.DrawTexture(new Rect(Input.mousePosition.x - 0.5f * size,
                             Screen.height - Input.mousePosition.y - 0.5f * size, size, size),
                    texture);
  }

  public void Update() {
    double pitch = Input.mousePosition.x / Screen.width;
    double amplitude = Input.mousePosition.y / Screen.height;
    if (Input.GetMouseButtonDown(0)) {
      SetNoteOn(0.0, amplitude);
    } else if (Input.GetMouseButtonUp(0)) {
      SetNoteOff(0.0);
    }
    SetControl(0, pitch);
    SetControl(1, amplitude);
  }

  protected override sealed ControlDefinition[] GetControlDefinitions() {
    ControlDefinition[] definitions = new ControlDefinition[2] {
      // Continuous pitch.
      new ControlDefinition() {
        defaultValue = 0.0,
        minValue = 0.0,
        maxValue = 1.0,
      },
      // Continuous amplitude.
      new ControlDefinition() {
        defaultValue = 0.0,
        minValue = 0.0,
        maxValue = 1.0,
      },
    };
    return definitions;
  }

  protected override sealed ControlDefinition[] GetNoteControlDefinitions() {
    return null;
  }
}
