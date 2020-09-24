#include "barelymusician/engine/instrument_manager.h"

#include "barelymusician/engine/engine_utils.h"
#include "barelymusician/engine/message.h"

namespace barelyapi {

namespace {

// Maximum number of tasks to be added per each Process call.
constexpr int kNumMaxTasks = 500;

}  // namespace

InstrumentManager::InstrumentManager()
    : id_counter_(0), task_runner_(kNumMaxTasks) {}

int InstrumentManager::Create(std::unique_ptr<Instrument> instrument) {
  const int instrument_id = ++id_counter_;
  task_runner_.Add([this, instrument_id,
                    instrument = std::make_shared<std::unique_ptr<Instrument>>(
                        std::move(instrument))]() {
    instruments_.emplace(instrument_id, InstrumentData{std::move(*instrument)});
  });
  return instrument_id;
}

void InstrumentManager::Destroy(int instrument_id) {
  task_runner_.Add(
      [this, instrument_id]() { instruments_.erase(instrument_id); });
}

void InstrumentManager::NoteOff(int instrument_id, double timestamp,
                                float index) {
  task_runner_.Add([this, instrument_id, timestamp, index]() {
    InstrumentData* instrument_data = FindOrNull(instruments_, instrument_id);
    if (instrument_data != nullptr) {
      instrument_data->messages.Push(timestamp, NoteOffData{index});
    }
  });
}

void InstrumentManager::NoteOn(int instrument_id, double timestamp, float index,
                               float intensity) {
  task_runner_.Add([this, instrument_id, timestamp, index, intensity]() {
    InstrumentData* instrument_data = FindOrNull(instruments_, instrument_id);
    if (instrument_data != nullptr) {
      instrument_data->messages.Push(timestamp, NoteOnData{index, intensity});
    }
  });
}

void InstrumentManager::Process(int instrument_id, double begin_timestamp,
                                double end_timestamp, float* output,
                                int num_channels, int num_frames) {
  task_runner_.Run();
  InstrumentData* instrument_data = FindOrNull(instruments_, instrument_id);
  if (instrument_data == nullptr) {
    return;
  }
  Instrument* instrument = instrument_data->instrument.get();
  int frame = 0;
  // Process mmessages.
  if (begin_timestamp < end_timestamp) {
    // Include *all* messages before |end_timestamp|.
    const auto messages =
        instrument_data->messages.GetIterator(0.0, end_timestamp);
    const double frame_rate =
        static_cast<double>(num_frames) / (end_timestamp - begin_timestamp);
    for (auto it = messages.cbegin; it != messages.cend; ++it) {
      int message_frame =
          static_cast<int>(frame_rate * (it->timestamp - begin_timestamp));
      if (frame < message_frame) {
        instrument->Process(&output[num_channels * frame], num_channels,
                            message_frame - frame);
        frame = message_frame;
      }
      std::visit(
          MessageVisitor{[this, instrument](const NoteOffData& note_off_data) {
                           instrument->NoteOff(note_off_data.index);
                         },
                         [this, instrument](const NoteOnData& note_on_data) {
                           instrument->NoteOn(note_on_data.index,
                                              note_on_data.intensity);
                         }},
          it->data);
    }
    instrument_data->messages.Clear(messages);
  }
  // Process the rest of the buffer.
  if (frame < num_frames) {
    instrument->Process(&output[num_channels * frame], num_channels,
                        num_frames - frame);
  }
}

}  // namespace barelyapi
