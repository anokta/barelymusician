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

bool InstrumentManager::Destroy(int instrument_id) {
  if (instrument_params_.erase(instrument_id) > 0) {
    task_runner_.Add(
        [this, instrument_id]() { instruments_.erase(instrument_id); });
  }
  return false;
}

bool InstrumentManager::GetParam(int instrument_id, int param_id,
                                 float* value) const {
  const InstrumentParamData* instrument_param_data =
      FindOrNull(instrument_params_, instrument_id);
  if (instrument_param_data == nullptr) {
    return false;
  }
  const float* param_value =
      FindOrNull(instrument_param_data->values, param_id);
  if (param_value == nullptr) {
    return false;
  }
  *value = *param_value;
  return true;
}

bool InstrumentManager::NoteOff(int instrument_id, float index) {
  if (FindOrNull(instrument_params_, instrument_id) == nullptr) {
    return false;
  }
  task_runner_.Add([this, instrument_id, index]() {
    InstrumentData* instrument_data = FindOrNull(instruments_, instrument_id);
    if (instrument_data != nullptr) {
      instrument_data->instrument->NoteOff(index);
    }
  });
  return true;
}

bool InstrumentManager::NoteOn(int instrument_id, float index,
                               float intensity) {
  if (FindOrNull(instrument_params_, instrument_id) == nullptr) {
    return false;
  }
  task_runner_.Add([this, instrument_id, index, intensity]() {
    InstrumentData* instrument_data = FindOrNull(instruments_, instrument_id);
    if (instrument_data != nullptr) {
      instrument_data->instrument->NoteOn(index, intensity);
    }
  });
  return true;
}

bool InstrumentManager::Process(int instrument_id, double begin_timestamp,
                                double end_timestamp, float* output,
                                int num_channels, int num_frames) {
  task_runner_.Run();
  InstrumentData* instrument_data = FindOrNull(instruments_, instrument_id);
  if (instrument_data == nullptr) {
    return false;
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
  return true;
}

bool InstrumentManager::SetParam(int instrument_id, int param_id, float value) {
  InstrumentParamData* instrument_param_data =
      FindOrNull(instrument_params_, instrument_id);
  if (instrument_param_data == nullptr) {
    return false;
  }
  float* param_value = FindOrNull(instrument_param_data->values, param_id);
  if (param_value == nullptr || *param_value == value) {
    return false;
  }
  *param_value = value;
  task_runner_.Add([this, instrument_id, param_id, value]() {
    InstrumentData* instrument_data = FindOrNull(instruments_, instrument_id);
    if (instrument_data != nullptr) {
      instrument_data->instrument->SetParam(param_id, value);
    }
  });
  return true;
}

bool InstrumentManager::ScheduleNoteOff(int instrument_id, double timestamp,
                                        float index) {
  if (FindOrNull(instrument_params_, instrument_id) == nullptr) {
    return false;
  }
  task_runner_.Add([this, instrument_id, timestamp, index]() {
    InstrumentData* instrument_data = FindOrNull(instruments_, instrument_id);
    if (instrument_data != nullptr) {
      instrument_data->messages.Push(timestamp, NoteOffData{index});
    }
  });
  return true;
}

bool InstrumentManager::ScheduleNoteOn(int instrument_id, double timestamp,
                                       float index, float intensity) {
  if (FindOrNull(instrument_params_, instrument_id) == nullptr) {
    return false;
  }
  task_runner_.Add([this, instrument_id, timestamp, index, intensity]() {
    InstrumentData* instrument_data = FindOrNull(instruments_, instrument_id);
    if (instrument_data != nullptr) {
      instrument_data->messages.Push(timestamp, NoteOnData{index, intensity});
    }
  });
  return true;
}

}  // namespace barelyapi
