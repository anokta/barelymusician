#ifndef BARELYMUSICIAN_EXAMPLES_INSTRUMENTS_INSTRUMENT_H_
#define BARELYMUSICIAN_EXAMPLES_INSTRUMENTS_INSTRUMENT_H_

#include <any>
#include <functional>

#include "barelymusician/engine/instrument_definition.h"

namespace barelyapi {
namespace examples {

// Generic instrument interface.
class Instrument {
 public:
  // Base destructor to ensure the derived classes get destroyed properly.
  virtual ~Instrument() = default;

  // Stops note with the given |index|.
  //
  // @param index Note index.
  virtual void NoteOff(float index) = 0;

  // Starts note with the given |index| and |intensity|.
  //
  // @param index Note index.
  // @param intensity Note intensity.
  virtual void NoteOn(float index, float intensity) = 0;

  // Processes the next |output| buffer.
  //
  // @param output Pointer to the output buffer.
  // @param num_channels Number of output channels.
  // @param num_frames Number of output frames.
  virtual void Process(float* output, int num_channels, int num_frames) = 0;

  // Sets param |value| with the given |id|.
  //
  // @param id Param id.
  // @param value Param value.
  virtual void SetParam(int id, float value) = 0;
};

using CreateFn = std::function<std::unique_ptr<Instrument>(int sample_rate)>;
InstrumentDefinition GetInstrumentDefinition(CreateFn create_fn);

}  // namespace examples
}  // namespace barelyapi

#endif  // BARELYMUSICIAN_EXAMPLES_INSTRUMENTS_INSTRUMENT_H_
