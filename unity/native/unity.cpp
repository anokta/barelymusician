#include "unity/native/unity.h"

#include <algorithm>
#include <memory>
#include <mutex>
#include <utility>

#include "barelymusician/common/id.h"
#include "barelymusician/common/id_generator.h"
#include "barelymusician/common/logging.h"
#include "barelymusician/common/status.h"
#include "barelymusician/engine/instrument_definition.h"
#include "barelymusician/engine/instrument_manager.h"
#include "examples/instruments/synth_instrument.h"
#include "unity/native/unity_log_writer.h"

namespace barelyapi::unity {

using ::barelyapi::examples::SynthInstrument;

// Unity plugin.
struct BarelyMusician {
  BarelyMusician(int sample_rate) : instrument_manager(sample_rate) {}

  // Engine.
  IdGenerator id_generator;
  InstrumentManager instrument_manager;

  // Unity log writer.
  UnityLogWriter writer;
};

BarelyMusician* BarelyInitialize(int sample_rate,
                                 DebugCallback* debug_callback_ptr) {
  BarelyMusician* barelymusician = new BarelyMusician(sample_rate);

  if (debug_callback_ptr) {
    const auto debug_callback = [debug_callback_ptr](int severity,
                                                     const char* message) {
      debug_callback_ptr(severity, message);
    };
    barelymusician->writer.SetDebugCallback(debug_callback);
  } else {
    barelymusician->writer.SetDebugCallback(nullptr);
  }
  logging::SetLogWriter(&barelymusician->writer);

  return barelymusician;
}

void BarelyShutdown(BarelyMusician* barelymusician) {
  if (barelymusician) {
    logging::SetLogWriter(nullptr);
    delete barelymusician;
  }
}

Id BarelyCreateSynthInstrument(BarelyMusician* barelymusician,
                               double timestamp) {
  if (barelymusician) {
    const Id instrument_id = barelymusician->id_generator.Generate();
    if (IsOk(barelymusician->instrument_manager.Create(
            instrument_id, timestamp, SynthInstrument::GetDefinition(),
            SynthInstrument::GetDefaultParams()))) {
      return instrument_id;
    }
  }
  return kInvalidId;
}

bool BarelyDestroyInstrument(BarelyMusician* barelymusician, Id instrument_id,
                             double timestamp) {
  if (barelymusician) {
    return IsOk(
        barelymusician->instrument_manager.Destroy(instrument_id, timestamp));
  }
  return false;
}

float BarelyGetInstrumentParam(BarelyMusician* barelymusician, Id instrument_id,
                               int param_id) {
  if (barelymusician) {
    return GetStatusOrValue(barelymusician->instrument_manager.GetParam(
                                instrument_id, param_id))
        .GetValue();
  }
  return 0.0f;
}

bool BarelyIsInstrumentNoteOn(BarelyMusician* barelymusician, Id instrument_id,
                              float note_pitch) {
  if (barelymusician) {
    return GetStatusOrValue(
        barelymusician->instrument_manager.IsNoteOn(instrument_id, note_pitch));
  }
  return false;
}

void BarelyProcessInstrument(BarelyMusician* barelymusician, Id instrument_id,
                             double timestamp, float* output, int num_channels,
                             int num_frames) {
  if (barelymusician) {
    barelymusician->instrument_manager.Process(instrument_id, timestamp, output,
                                               num_channels, num_frames);
  }
}

bool BarelySetAllInstrumentNotesOff(BarelyMusician* barelymusician,
                                    Id instrument_id, double timestamp) {
  if (barelymusician) {
    return IsOk(barelymusician->instrument_manager.SetAllNotesOff(instrument_id,
                                                                  timestamp));
  }
  return false;
}

bool BarelySetAllInstrumentParamsToDefault(BarelyMusician* barelymusician,
                                           Id instrument_id, double timestamp) {
  if (barelymusician) {
    return IsOk(barelymusician->instrument_manager.SetAllParamsToDefault(
        instrument_id, timestamp));
  }
  return false;
}

bool BarelySetInstrumentNoteOff(BarelyMusician* barelymusician,
                                Id instrument_id, double timestamp,
                                float note_pitch) {
  if (barelymusician) {
    return IsOk(barelymusician->instrument_manager.SetNoteOff(
        instrument_id, timestamp, note_pitch));
  }
  return false;
}

void BarelySetInstrumentNoteOffCallback(
    BarelyMusician* barelymusician, NoteOffCallback* note_off_callback_ptr) {
  if (barelymusician) {
    if (note_off_callback_ptr) {
      barelymusician->instrument_manager.SetNoteOffCallback(
          [note_off_callback_ptr](Id instrument_id, double /*timestamp*/,
                                  float note_pitch) {
            note_off_callback_ptr(instrument_id, note_pitch);
          });
    } else {
      barelymusician->instrument_manager.SetNoteOffCallback(nullptr);
    }
  }
}

bool BarelySetInstrumentNoteOn(BarelyMusician* barelymusician, Id instrument_id,
                               double timestamp, float note_pitch,
                               float note_intensity) {
  if (barelymusician) {
    return IsOk(barelymusician->instrument_manager.SetNoteOn(
        instrument_id, timestamp, note_pitch, note_intensity));
  }
  return false;
}

void BarelySetInstrumentNoteOnCallback(BarelyMusician* barelymusician,
                                       NoteOnCallback* note_on_callback_ptr) {
  if (barelymusician) {
    if (note_on_callback_ptr) {
      barelymusician->instrument_manager.SetNoteOnCallback(
          [note_on_callback_ptr](Id instrument_id, double /*timestamp*/,
                                 float note_pitch, float note_intensity) {
            note_on_callback_ptr(instrument_id, note_pitch, note_intensity);
          });
    } else {
      barelymusician->instrument_manager.SetNoteOnCallback(nullptr);
    }
  }
}

bool BarelySetInstrumentParam(BarelyMusician* barelymusician, Id instrument_id,
                              double timestamp, int param_id,
                              float param_value) {
  if (barelymusician) {
    return IsOk(barelymusician->instrument_manager.SetParam(
        instrument_id, timestamp, param_id, param_value));
  }
  return false;
}

bool BarelySetInstrumentParamToDefault(BarelyMusician* barelymusician,
                                       Id instrument_id, double timestamp,
                                       int param_id) {
  if (barelymusician) {
    return IsOk(barelymusician->instrument_manager.SetParamToDefault(
        instrument_id, timestamp, param_id));
  }
  return false;
}

void BarelySetSampleRate(BarelyMusician* barelymusician, double timestamp,
                         int sample_rate) {
  if (barelymusician) {
    barelymusician->instrument_manager.SetSampleRate(timestamp, sample_rate);
  }
}

void BarelyUpdate(BarelyMusician* barelymusician, double /*timestamp*/) {
  if (barelymusician) {
    barelymusician->instrument_manager.Update();
  }
}

}  // namespace barelyapi::unity
