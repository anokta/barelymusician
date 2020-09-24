#ifndef BARELYMUSICIAN_INSTRUMENT_INSTRUMENT_H_
#define BARELYMUSICIAN_INSTRUMENT_INSTRUMENT_H_

namespace barelyapi {

// Generic instrument interface.
class Instrument {
 public:
  // Base destructor to ensure the derived classes get destroyed properly.
  virtual ~Instrument() = default;

  // Sets control |value| with the given |id|.
  //
  // @param id Control id.
  // @param value Control value.
  virtual void Control(int id, float value) = 0;

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
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_INSTRUMENT_INSTRUMENT_H_
