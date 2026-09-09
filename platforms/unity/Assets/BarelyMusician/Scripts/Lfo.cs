using System;
using System.Data.Common;
using UnityEngine;

namespace Barely {
  /// A representation of a low-frequency oscillator.
  [Serializable]
  public sealed class Lfo : IDisposable {
    /// Noise mix.
    public float NoiseMix {
      get { return _noiseMix; }
      set {
        _noiseMix = value;
        Engine.Internal.Lfo_SetControl(id, Engine.Internal.LfoControlType.NOISE_MIX, _noiseMix);
      }
    }
    [SerializeField]
    [Range(0.0f, 1.0f)]
    private float _noiseMix = 0.0f;

    /// Shape.
    public float Shape {
      get { return _shape; }
      set {
        _shape = value;
        Engine.Internal.Lfo_SetControl(id, Engine.Internal.LfoControlType.SHAPE, _shape);
      }
    }
    [SerializeField]
    [Range(0.0f, 1.0f)]
    private float _shape = 0.0f;

    /// Skew.
    public float Skew {
      get { return _skew; }
      set {
        _skew = value;
        Engine.Internal.Lfo_SetControl(id, Engine.Internal.LfoControlType.SKEW, _skew);
      }
    }
    [SerializeField]
    [Range(-1.0f, 1.0f)]
    private float _skew = 0.0f;

    /// Phase.
    public double Phase {
      get { return (_id > 0) ? Engine.Internal.Lfo_GetPhase(_id) : _phase; }
      set {
        _phase = value;
        Engine.Internal.Lfo_SetPhase(id, _phase);
      }
    }
    [SerializeField]
    [Range(0.0f, 1.0f)]
    private double _phase = 0.0;

    /// Speed.
    public double Speed {
      get { return _speed; }
      set {
        _speed = value;
        Engine.Internal.Lfo_SetSpeed(id, _speed);
      }
    }
    [SerializeField]
    [Min(0.0f)]
    private double _speed = 1.0;

    /// Evaluates the lfo at its current phase.
    public float Evaluate() {
      return Engine.Internal.Lfo_Evaluate(id);
    }

    public Lfo() {}

    public void Dispose() {
      Engine.Internal.Lfo_Destroy(ref _id);
    }

    public void Reset() {
      Engine.Internal.Lfo_Destroy(ref _id);
      Initialize();
    }

    private void Initialize() {
      Engine.Internal.Lfo_Create(this, ref _id);
      Engine.Internal.Lfo_SetControl(_id, Engine.Internal.LfoControlType.NOISE_MIX, _noiseMix);
      Engine.Internal.Lfo_SetControl(_id, Engine.Internal.LfoControlType.SHAPE, _shape);
      Engine.Internal.Lfo_SetControl(_id, Engine.Internal.LfoControlType.SKEW, _skew);
      Engine.Internal.Lfo_SetPhase(_id, _phase);
      Engine.Internal.Lfo_SetSpeed(_id, _speed);
    }

    private UInt32 id {
      get {
        if (_id == 0) {
          Initialize();
        }
        return _id;
      }
    }
    private UInt32 _id = 0;
  }
}
