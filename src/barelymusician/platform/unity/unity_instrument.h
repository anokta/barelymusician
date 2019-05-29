#ifndef BARELYMUSICIAN_PLATFORM_UNITY_UNITY_INSTRUMENT_H_
#define BARELYMUSICIAN_PLATFORM_UNITY_UNITY_INSTRUMENT_H_

#include <functional>

#include "barelymusician/instrument/instrument.h"

namespace barelyapi {
namespace unity {

class UnityInstrument : public Instrument {
 public:
  // Instrument callback signatures.
  using NoteOffCallback = std::function<void(float)>;
  using NoteOnCallback = std::function<void(float, float)>;
  using ProcessCallback = std::function<void(float*, int, int)>;
  using ResetCallback = std::function<void()>;

  // Constructs new |UnityInstrument|.
  //
  // @param note_off_callback Note off callback.
  // @param note_on_callback Note on callback.
  // @param process_callback Process callback.
  // @param reset_callback Reset callback.
  UnityInstrument(NoteOffCallback&& note_off_callback,
                  NoteOnCallback&& note_on_callback,
                  ProcessCallback&& process_callback,
                  ResetCallback&& reset_callback);

  // Implements |Instrument|.
  void NoteOff(float index) override;
  void NoteOn(float index, float intensity) override;
  void Process(float* output, int num_channels, int num_frames) override;
  void Reset() override;

 private:
  // Instrument callbacks.
  NoteOffCallback note_off_callback_;
  NoteOnCallback note_on_callback_;
  ProcessCallback process_callback_;
  ResetCallback reset_callback_;
};

}  // namespace unity
}  // namespace barelyapi

#endif  // BARELYMUSICIAN_PLATFORM_UNITY_UNITY_INSTRUMENT_H_
