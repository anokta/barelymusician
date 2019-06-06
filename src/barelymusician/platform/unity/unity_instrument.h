#ifndef BARELYMUSICIAN_PLATFORM_UNITY_UNITY_INSTRUMENT_H_
#define BARELYMUSICIAN_PLATFORM_UNITY_UNITY_INSTRUMENT_H_

#include <functional>

#include "barelymusician/instrument/instrument.h"

namespace barelyapi {
namespace unity {

class UnityInstrument : public Instrument {
 public:
  // Instrument callback signatures.
  using ClearCallback = std::function<void()>;
  using NoteOffCallback = std::function<void(float)>;
  using NoteOnCallback = std::function<void(float, float)>;
  using ProcessCallback = std::function<void(float*, int, int)>;

  // Constructs new |UnityInstrument|.
  //
  // @param clear_callback Clear callback.
  // @param note_off_callback Note off callback.
  // @param note_on_callback Note on callback.
  // @param process_callback Process callback.
  UnityInstrument(ClearCallback&& clear_callback,
                  NoteOffCallback&& note_off_callback,
                  NoteOnCallback&& note_on_callback,
                  ProcessCallback&& process_callback);

  // Implements |Instrument|.
  void Clear() override;
  void NoteOff(float index) override;
  void NoteOn(float index, float intensity) override;
  void Process(float* output, int num_channels, int num_frames) override;

 private:
  // Instrument callbacks.
  ClearCallback clear_callback_;
  NoteOffCallback note_off_callback_;
  NoteOnCallback note_on_callback_;
  ProcessCallback process_callback_;
};

}  // namespace unity
}  // namespace barelyapi

#endif  // BARELYMUSICIAN_PLATFORM_UNITY_UNITY_INSTRUMENT_H_
