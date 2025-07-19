#include <barelymusician.h>

#include <span>

#include "emscripten/bind.h"

using ::barely::ControlType;
using ::barely::Engine;
using ::barely::Instrument;
using ::barely::Performer;
using ::barely::Quantization;
using ::barely::Task;
using ::barely::TaskState;
using ::barely::Trigger;
using ::emscripten::allow_raw_pointers;
using ::emscripten::class_;
using ::emscripten::optional_override;
using ::emscripten::val;
using ::emscripten::return_value_policy::take_ownership;

[[nodiscard]] static Instrument Engine_CreateInstrument(Engine& engine) noexcept {
  return engine.CreateInstrument({{
      {ControlType::kGain, 0.25f},
      {ControlType::kAttack, 0.05f},
      {ControlType::kRelease, 0.1f},
      {ControlType::kOscMix, 1.0f},
  }});
}

static void Engine_Process(Engine& engine, uintptr_t samples, int sample_count,
                           double timestamp) noexcept {
  engine.Process(std::span<float>(reinterpret_cast<float*>(samples), sample_count), timestamp);
}

[[nodiscard]] static uintptr_t Instrument_GetHandle(Instrument& instrument) noexcept {
  return reinterpret_cast<uintptr_t>(static_cast<BarelyInstrumentHandle>(instrument));
}

static void Instrument_SetControl(Instrument& instrument, int type, float value) noexcept {
  instrument.SetControl(static_cast<ControlType>(type), value);
}

static void Instrument_SetNoteOn(Instrument& instrument, float pitch) noexcept {
  instrument.SetNoteOn(pitch);
}

[[nodiscard]] static Task Performer_CreateTask(Performer& performer, double position,
                                               double duration) noexcept {
  return performer.CreateTask(position, duration, /*callback=*/nullptr);
}

[[nodiscard]] static Trigger Performer_CreateTrigger(Performer& performer,
                                                     double position) noexcept {
  return performer.CreateTrigger(position, /*callback=*/nullptr);
}

[[nodiscard]] static uintptr_t Performer_GetHandle(Performer& performer) noexcept {
  return reinterpret_cast<uintptr_t>(static_cast<BarelyPerformerHandle>(performer));
}

[[nodiscard]] static uintptr_t Task_GetHandle(Task& task) noexcept {
  return reinterpret_cast<uintptr_t>(static_cast<BarelyTaskHandle>(task));
}

[[nodiscard]] static uintptr_t Trigger_GetHandle(Trigger& trigger) noexcept {
  return reinterpret_cast<uintptr_t>(static_cast<BarelyTriggerHandle>(trigger));
}

EMSCRIPTEN_BINDINGS(barelymusician_main) {
  class_<Engine>("Engine")
      .constructor<int, float>()
      .function("createInstrument", &Engine_CreateInstrument, take_ownership())
      .function("createPerformer", &Engine::CreatePerformer, take_ownership())
      .function("generateRandomNumber",
                static_cast<double (Engine::*)()>(&Engine::GenerateRandomNumber))
      .function("generateRandomNumber", &Engine::GenerateRandomNumber<double>)
      .function("generateRandomInteger", &Engine::GenerateRandomNumber<int>)
      .function("process", &Engine_Process, allow_raw_pointers())
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
      .function("setNoteOff", &Instrument::SetNoteOff)
      .function("setNoteOffCallback",
                optional_override([](Instrument& instrument, val js_callback) {
                  return instrument.SetNoteOffCallback(
                      [js_callback](float pitch) { js_callback(pitch); });
                }))
      .function("setNoteOn", &Instrument_SetNoteOn)
      .function("setNoteOn",
                static_cast<void (Instrument::*)(float, float)>(&Instrument::SetNoteOn))
      .function("setNoteOn",
                static_cast<void (Instrument::*)(float, float, float)>(&Instrument::SetNoteOn))
      .function(
          "setNoteOnCallback", optional_override([](Instrument& instrument, val js_callback) {
            return instrument.SetNoteOnCallback([js_callback](float pitch) { js_callback(pitch); });
          }));
  // TODO(#164): Add sample data support.

  class_<Performer>("Performer")
      .function("createTask", &Performer_CreateTask, take_ownership())
      .function("createTrigger", &Performer_CreateTrigger, take_ownership())
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
      .function("setProcessCallback", optional_override([](Task& task, val js_callback) {
                  return task.SetProcessCallback(
                      [js_callback](TaskState state) { js_callback(static_cast<int>(state)); });
                }))
      .property("isActive", &Task::IsActive)
      .property("duration", &Task::GetDuration, &Task::SetDuration)
      .property("position", &Task::GetPosition, &Task::SetPosition);

  class_<Trigger>("Trigger")
      .function("getHandle", &Trigger_GetHandle, allow_raw_pointers())
      .function("setProcessCallback", optional_override([](Trigger& trigger, val js_callback) {
                  return trigger.SetProcessCallback([js_callback]() { js_callback(); });
                }))
      .property("position", &Trigger::GetPosition, &Trigger::SetPosition);
}
