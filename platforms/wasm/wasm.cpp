#include <barelymusician.h>

#include "emscripten/bind.h"

using ::barely::Engine;
using ::barely::EngineControlType;
using ::barely::Instrument;
using ::barely::InstrumentControlType;
using ::barely::NoteEventType;
using ::barely::Performer;
using ::barely::Quantization;
using ::barely::Task;
using ::barely::TaskEventType;
using ::emscripten::allow_raw_pointers;
using ::emscripten::class_;
using ::emscripten::optional_override;
using ::emscripten::val;
using ::emscripten::return_value_policy::take_ownership;

[[nodiscard]] static Instrument Engine_CreateInstrument(Engine& engine) noexcept {
  return engine.CreateInstrument({{
      {InstrumentControlType::kGain, 0.25f},
      {InstrumentControlType::kAttack, 0.05f},
      {InstrumentControlType::kRelease, 0.1f},
      {InstrumentControlType::kOscMix, 1.0f},
      {InstrumentControlType::kDelaySend, 0.1f},
      {InstrumentControlType::kArpRate, 2.0f},
  }});
}

static void Engine_SetControl(Engine& engine, int type, float value) noexcept {
  engine.SetControl(static_cast<EngineControlType>(type), value);
}

static void Engine_Process(Engine& engine, uintptr_t output_samples, int output_channel_count,
                           int output_frame_count, double timestamp) noexcept {
  engine.Process(reinterpret_cast<float*>(output_samples), output_channel_count, output_frame_count,
                 timestamp);
}

[[nodiscard]] static uintptr_t Instrument_GetHandle(Instrument& instrument) noexcept {
  return reinterpret_cast<uintptr_t>(static_cast<BarelyInstrumentHandle>(instrument));
}

static void Instrument_SetControl(Instrument& instrument, int type, float value) noexcept {
  instrument.SetControl(static_cast<InstrumentControlType>(type), value);
}

static void Instrument_SetNoteOn(Instrument& instrument, float pitch) noexcept {
  instrument.SetNoteOn(pitch);
}

[[nodiscard]] static Task Performer_CreateTask(Performer& performer, double position,
                                               double duration) noexcept {
  return performer.CreateTask(position, duration, /*priority=*/0, /*callback=*/nullptr);
}

[[nodiscard]] static uintptr_t Performer_GetHandle(Performer& performer) noexcept {
  return reinterpret_cast<uintptr_t>(static_cast<BarelyPerformerHandle>(performer));
}

[[nodiscard]] static uintptr_t Task_GetHandle(Task& task) noexcept {
  return reinterpret_cast<uintptr_t>(static_cast<BarelyTaskHandle>(task));
}

EMSCRIPTEN_BINDINGS(barelymusician_main) {
  class_<Engine>("Engine")
      .constructor<int, int, float>()
      .function("createInstrument", &Engine_CreateInstrument, take_ownership())
      .function("createPerformer", &Engine::CreatePerformer, take_ownership())
      .function("generateRandomNumber",
                static_cast<double (Engine::*)()>(&Engine::GenerateRandomNumber))
      .function("generateRandomNumber", &Engine::GenerateRandomNumber<double>)
      .function("generateRandomInteger", &Engine::GenerateRandomNumber<int>)
      .function("getControl", &Engine::GetControl<float>)
      .function("process", &Engine_Process, allow_raw_pointers())
      .function("setControl", &Engine_SetControl)
      .function("update", &Engine::Update)
      .property("seed", &Engine::GetSeed, &Engine::SetSeed)
      .property("tempo", &Engine::GetTempo, &Engine::SetTempo)
      .property("timestamp", &Engine::GetTimestamp);

  class_<Instrument>("Instrument")
      .function("getControl", &Instrument::GetControl<float>)
      .function("getHandle", &Instrument_GetHandle, allow_raw_pointers())
      .function("getNoteControl", &Instrument::GetNoteControl<float>)
      .function("isNoteOn", &Instrument::IsNoteOn)
      .function("setAllNotesOff", &Instrument::SetAllNotesOff)
      .function("setControl", &Instrument_SetControl)
      .function("setNoteControl", &Instrument::SetNoteControl<float>)
      .function(
          "setNoteEventCallback", optional_override([](Instrument& instrument, val js_callback) {
            return instrument.SetNoteEventCallback([js_callback](NoteEventType type, float pitch) {
              js_callback(static_cast<int>(type), pitch);
            });
          }))
      .function("setNoteOff", &Instrument::SetNoteOff)
      .function("setNoteOn", &Instrument_SetNoteOn)
      .function("setNoteOn",
                static_cast<void (Instrument::*)(float, float)>(&Instrument::SetNoteOn))
      .function("setNoteOn",
                static_cast<void (Instrument::*)(float, float, float)>(&Instrument::SetNoteOn));
  // TODO(#164): Add sample data support.

  class_<Performer>("Performer")
      .function("createTask", &Performer_CreateTask, take_ownership())
      .function("getHandle", &Performer_GetHandle, allow_raw_pointers())
      .function("start", &Performer::Start)
      .function("stop", &Performer::Stop)
      .property("isLooping", &Performer::IsLooping, &Performer::SetLooping)
      .property("isPlaying", &Performer::IsPlaying)
      .property("loopBeginPosition", &Performer::GetLoopBeginPosition,
                &Performer::SetLoopBeginPosition)
      .property("loopLength", &Performer::GetLoopLength, &Performer::SetLoopLength)
      .property("position", &Performer::GetPosition, &Performer::SetPosition);

  class_<Quantization>("Quantization")
      .constructor<int, float>()
      .function("getPosition", &Quantization::GetPosition);

  class_<Task>("Task")
      .function("getHandle", &Task_GetHandle, allow_raw_pointers())
      .function("setEventCallback", optional_override([](Task& task, val js_callback) {
                  return task.SetEventCallback(
                      [js_callback](TaskEventType type) { js_callback(static_cast<int>(type)); });
                }))
      .property("isActive", &Task::IsActive)
      .property("duration", &Task::GetDuration, &Task::SetDuration)
      .property("position", &Task::GetPosition, &Task::SetPosition);
}
