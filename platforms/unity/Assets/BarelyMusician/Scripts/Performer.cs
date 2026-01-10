using System;
using System.Collections.Generic;
using UnityEngine;

namespace Barely {
  /// A representation of a musical performer that can perform rhytmic tasks in real-time.
  public class Performer : MonoBehaviour {
    /// True if playing on awake, false otherwise.
    public bool PlayOnAwake = false;
    private bool _playOnEnable = false;

    /// True if looping, false otherwise.
    public bool Loop {
      get { return _loop; }
      set {
        if (_id == 0) {
          _loop = value;
          return;
        }
        Engine.Internal.Performer_SetLooping(_id, value);
        _loop = Engine.Internal.Performer_IsLooping(_id);
      }
    }
    [SerializeField]
    private bool _loop = false;

    /// Loop begin position in beats.
    public double LoopBeginPosition {
      get { return _loopBeginPosition; }
      set {
        if (_id == 0) {
          _loopBeginPosition = value;
          return;
        }
        Engine.Internal.Performer_SetLoopBeginPosition(_id, value);
        _loopBeginPosition = Engine.Internal.Performer_GetLoopBeginPosition(_id);
      }
    }
    [SerializeField]
    private double _loopBeginPosition = 0.0;

    /// Loop length in beats.
    public double LoopLength {
      get { return _loopLength; }
      set {
        if (_id == 0) {
          _loopLength = value;
          return;
        }
        Engine.Internal.Performer_SetLoopLength(_id, value);
        _loopLength = Engine.Internal.Performer_GetLoopLength(_id);
      }
    }
    [SerializeField]
    [Min(0.0f)]
    private double _loopLength = 1.0;

    /// List of recurring tasks.
    public List<Task> Tasks = new List<Task>();

    /// True if playing, false otherwise.
    public bool IsPlaying {
      get { return Engine.Internal.Performer_IsPlaying(_id); }
    }

    /// Position in beats.
    public double Position {
      get { return Engine.Internal.Performer_GetPosition(_id); }
      set { Engine.Internal.Performer_SetPosition(_id, value); }
    }

    /// Starts the performer.
    public void Play() {
      _playOnEnable = (_id == 0);
      Engine.Internal.Performer_Start(_id);
    }

    /// Stops the performer.
    public void Stop() {
      _playOnEnable = false;
      Engine.Internal.Performer_Stop(_id);
    }

    /// Class that wraps the internal api.
    public static class Internal {
      /// Returns the identifier.
      public static UInt32 GetId(Performer performer) {
        return performer ? performer._id : 0;
      }
    }

    // Identifier.
    private UInt32 _id = 0;

    private void OnEnable() {
      Engine.Internal.Performer_Create(this, ref _id);
      Update();
      if (PlayOnAwake || _playOnEnable) {
        Play();
      }
    }

    private void Start() {
      if (PlayOnAwake && !IsPlaying) {
        Play();
      }
    }

    private void OnDisable() {
      for (int i = 0; i < Tasks.Count; ++i) {
        Tasks[i].Update(null);
      }
      Engine.Internal.Performer_Destroy(ref _id);
    }

    private void Update() {
      Loop = _loop;
      LoopBeginPosition = _loopBeginPosition;
      LoopLength = _loopLength;
      for (int i = 0; i < Tasks.Count; ++i) {
        Tasks[i].Update(this);
      }
    }
  }
}  // namespace Barely
