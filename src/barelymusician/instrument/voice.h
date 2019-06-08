#ifndef BARELYMUSICIAN_INSTRUMENT_VOICE_H_
#define BARELYMUSICIAN_INSTRUMENT_VOICE_H_

namespace barelyapi {

// Instrument voice interface.
class Voice {
 public:
  // Returns whether the voice is currently active (i.e., playing).
  //
  // @return True if active.
  virtual bool IsActive() const = 0;

  // Returns the next output sample for the given output channel.
  //
  // @param channel Output channel.
  // @return Output sample.
  virtual float Next(int channel) = 0;

  // Starts the voice.
  virtual void Start() = 0;

  // Stops the voice.
  virtual void Stop() = 0;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_INSTRUMENT_VOICE_H_
