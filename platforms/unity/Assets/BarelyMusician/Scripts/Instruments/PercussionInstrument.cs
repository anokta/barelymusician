using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using UnityEngine;

namespace Barely {
  /// A representation of a simple sample-based percussion instrument.
  public class PercussionInstrument : Instrument {
    /// Percussion pad.
    [Serializable]
    public class Pad {
      /// Note pitch.
      [Range(0, 127)]
      public int Pitch = 0;

      /// Sample.
      public AudioClip Sample = null;

      /// Internal data.
      public double[] Data {
        get {
          if (_data == null || HasChanged) {
            _pitch = Pitch;
            _sample = Sample;
            if (_sample == null || _sample.samples == 0) {
              _data = null;
              return _data;
            }
            if (_data == null || _sample.samples + 3 != Data.Length) {
              _data = new double[_sample.samples + 3];
            }
            // Write the meta data.
            _data[0] = (double)_pitch;
            _data[1] = (double)_sample.frequency;
            _data[2] = (double)_sample.samples;
            // Write the sample data.
            float[] sampleData = new float[_sample.samples * _sample.channels];
            _sample.GetData(sampleData, 0);
            for (int frame = 0; frame < _sample.samples; ++frame) {
              _data[frame + 3] = 0.0;
              for (int channel = 0; channel < _sample.channels; ++channel) {
                _data[frame + 3] += (double)sampleData[frame * _sample.channels + channel];
              }
            }
          }
          return _data;
        }
      }
      private double[] _data = null;

      /// Denotes whether any changes has occured since the last update.
      public bool HasChanged {
        get { return Sample != _sample || Pitch != _pitch; }
      }

      /// Current note.
      private int _pitch = 0;

      // Current sample.
      private AudioClip _sample = null;
    }

    /// Gain.
    [Range(0.0f, 1.0f)]
    public double Gain = 1.0;

    /// Pad envelope release.
    [Range(0.0f, 60.0f)]
    public double PadRelease = 1.0;

    /// List of pads.
    public List<Pad> Pads = null;
    private int _padCount = 0;

    private void Update() {
      if (Pads != null) {
        UpdatePadData();
      }
      SetControl(0, Gain);
      SetControl(1, PadRelease);
    }

    // Updates pad data.
    private void UpdatePadData() {
      if (Pads.Count == _padCount && !Pads.Any(pad => pad.HasChanged)) {
        return;
      }
      _padCount = 0;
      int length = 1;
      for (int i = 0; i < Pads.Count; ++i) {
        if (Pads[i].Data != null) {
          ++_padCount;
          length += Pads[i].Data.Length;
        }
      }
      // Write into an unmanaged pointer.
      int size = length * sizeof(double);
      IntPtr dataPtr = Marshal.AllocHGlobal(size);
      Marshal.Copy(new double[1] { (double)_padCount }, 0, dataPtr, 1);
      IntPtr padDataPtr = IntPtr.Add(dataPtr, sizeof(double));
      for (int i = 0; i < Pads.Count; ++i) {
        double[] padData = Pads[i].Data;
        if (padData != null) {
          Marshal.Copy(padData, 0, padDataPtr, padData.Length);
          padDataPtr = IntPtr.Add(padDataPtr, padData.Length * sizeof(double));
        }
      }
      SetData(dataPtr, size);
      Marshal.FreeHGlobal(dataPtr);
    }
  }
}  // namespace Barely
