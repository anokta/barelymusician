#include "unity/native/unity.h"

#include <algorithm>
#include <memory>
#include <utility>

#include "barelymusician/common/id.h"
#include "barelymusician/common/logging.h"
#include "barelymusician/engine/instrument_definition.h"
#include "barelymusician/engine/instrument_manager.h"
#include "examples/instruments/synth_instrument.h"
#include "unity/native/unity_log_writer.h"

namespace barelyapi::unity {

namespace {

using ::barelyapi::examples::SynthInstrument;

// Invalid instrument_id.
inline constexpr Id kInvalidId = -1;

}  // namespace

// Unity plugin.
struct BarelyMusician {
  BarelyMusician(int sample_rate)
      : instrument_manager(sample_rate, &id_generator) {}

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

Id BarelyCreateSynthInstrument(BarelyMusician* barelymusician) {
  if (barelymusician) {
    return barelymusician->instrument_manager.Create(
        SynthInstrument::GetDefinition(), SynthInstrument::GetDefaultParams());
  }
  return kInvalidId;
}

bool BarelyDestroyInstrument(BarelyMusician* barelymusician, Id instrument_id) {
  if (barelymusician) {
    return barelymusician->instrument_manager.Destroy(instrument_id);
  }
  return false;
}

float BarelyGetInstrumentParam(BarelyMusician* barelymusician, Id instrument_id,
                               int param_id) {
  if (barelymusician) {
    if (const float* param = barelymusician->instrument_manager.GetParam(
            instrument_id, param_id)) {
      return *param;
    }
  }
  return 0.0f;
}

bool BarelyIsInstrumentNoteOn(BarelyMusician* barelymusician, Id instrument_id,
                              float note_pitch) {
  if (barelymusician) {
    return barelymusician->instrument_manager.IsNoteOn(instrument_id,
                                                       note_pitch);
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

void BarelyResetAllInstrumentParams(BarelyMusician* barelymusician,
                                    Id instrument_id, double timestamp) {
  if (barelymusician) {
    barelymusician->instrument_manager.SetEvent(instrument_id, timestamp,
                                                ResetAllParams{});
  }
}

void BarelySetAllInstrumentNotesOff(BarelyMusician* barelymusician,
                                    Id instrument_id, double timestamp) {
  if (barelymusician) {
    barelymusician->instrument_manager.SetEvent(instrument_id, timestamp,
                                                SetAllNotesOff{});
  }
}

void BarelySetInstrumentNoteOff(BarelyMusician* barelymusician,
                                Id instrument_id, double timestamp,
                                float note_pitch) {
  if (barelymusician) {
    barelymusician->instrument_manager.SetEvent(instrument_id, timestamp,
                                                SetNoteOff{note_pitch});
  }
}

void BarelySetInstrumentNoteOffCallback(
    BarelyMusician* barelymusician, NoteOffCallback* note_off_callback_ptr) {
  if (barelymusician) {
    if (note_off_callback_ptr) {
      barelymusician->instrument_manager.SetNoteOffCallback(
          [note_off_callback_ptr](Id instrument_id, float note_pitch) {
            note_off_callback_ptr(instrument_id, note_pitch);
          });
    } else {
      barelymusician->instrument_manager.SetNoteOffCallback(nullptr);
    }
  }
}

void BarelySetInstrumentNoteOn(BarelyMusician* barelymusician, Id instrument_id,
                               double timestamp, float note_pitch,
                               float note_intensity) {
  if (barelymusician) {
    barelymusician->instrument_manager.SetEvent(
        instrument_id, timestamp, SetNoteOn{note_pitch, note_intensity});
  }
}

void BarelySetInstrumentNoteOnCallback(BarelyMusician* barelymusician,
                                       NoteOnCallback* note_on_callback_ptr) {
  if (barelymusician) {
    if (note_on_callback_ptr) {
      barelymusician->instrument_manager.SetNoteOnCallback(
          [note_on_callback_ptr](Id instrument_id, float note_pitch,
                                 float note_intensity) {
            note_on_callback_ptr(instrument_id, note_pitch, note_intensity);
          });
    } else {
      barelymusician->instrument_manager.SetNoteOnCallback(nullptr);
    }
  }
}

void BarelySetInstrumentParam(BarelyMusician* barelymusician, Id instrument_id,
                              double timestamp, int param_id,
                              float param_value) {
  if (barelymusician) {
    barelymusician->instrument_manager.SetEvent(
        instrument_id, timestamp, SetParam{param_id, param_value});
  }
}

void BarelyUpdate(BarelyMusician* barelymusician, double timestamp) {
  if (barelymusician) {
    barelymusician->instrument_manager.Update(timestamp);
  }
}

}  // namespace barelyapi::unity
