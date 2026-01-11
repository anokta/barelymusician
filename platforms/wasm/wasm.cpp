#include <barelymusician.h>

#include "emscripten/bind.h"

using ::barely::Engine;
using ::barely::EngineControlType;
using ::barely::Instrument;
using ::barely::InstrumentControlType;
using ::barely::NoteControlType;
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

[[nodiscard]] static uint32_t Instrument_GetId(Instrument& instrument) noexcept {
  return static_cast<uint32_t>(instrument);
}

static void Instrument_SetControl(Instrument& instrument, int type, float value) noexcept {
  instrument.SetControl(static_cast<InstrumentControlType>(type), value);
}

static void Instrument_SetNoteControl(Instrument& instrument, float pitch, int type,
                                      float value) noexcept {
  instrument.SetNoteControl(pitch, static_cast<NoteControlType>(type), value);
}

static void Instrument_SetNoteOn(Instrument& instrument, float pitch) noexcept {
  instrument.SetNoteOn(pitch);
}

static void Instrument_SetSampleData(Instrument& instrument, uintptr_t slices,
                                     int slice_count) noexcept {
  instrument.SetSampleData({reinterpret_cast<const barely::Slice*>(slices),
                            reinterpret_cast<const barely::Slice*>(slices) + slice_count});
}

[[nodiscard]] static Task Engine_CreateTask(Engine& engine, Performer& performer, double position,
                                            double duration) noexcept {
  return engine.CreateTask(performer, position, duration, /*priority=*/0, /*callback=*/nullptr);
}

[[nodiscard]] static uint32_t Performer_GetId(Performer& performer) noexcept {
  return static_cast<uint32_t>(performer);
}

[[nodiscard]] static uint32_t Task_GetId(Task& task) noexcept {
  return static_cast<uint32_t>(task);
}

EMSCRIPTEN_BINDINGS(barelymusician_main) {
  class_<Engine>("Engine")
      .constructor<int>()
      .function("createInstrument", &Engine_CreateInstrument, take_ownership())
      .function("createPerformer", &Engine::CreatePerformer, take_ownership())
      .function("createTask", &Engine_CreateTask, take_ownership())
      .function("destroyInstrument", &Engine::DestroyInstrument)
      .function("destroyPerformer", &Engine::DestroyPerformer)
      .function("destroyTask", &Engine::DestroyTask)
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
      .function("getId", &Instrument_GetId)
      .function("getNoteControl", &Instrument::GetNoteControl<float>)
      .function("isNoteOn", &Instrument::IsNoteOn)
      .function("setAllNotesOff", &Instrument::SetAllNotesOff)
      .function("setControl", &Instrument_SetControl)
      .function("setNoteControl", &Instrument_SetNoteControl)
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
                static_cast<void (Instrument::*)(float, float, float)>(&Instrument::SetNoteOn))
      .function("setSampleData", &Instrument_SetSampleData, allow_raw_pointers());

  class_<Performer>("Performer")
      .function("getId", &Performer_GetId)
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
      .function("getId", &Task_GetId)
      .function("setEventCallback", optional_override([](Task& task, val js_callback) {
                  return task.SetEventCallback(
                      [js_callback](TaskEventType type) { js_callback(static_cast<int>(type)); });
                }))
      .property("isActive", &Task::IsActive)
      .property("duration", &Task::GetDuration, &Task::SetDuration)
      .property("position", &Task::GetPosition, &Task::SetPosition);
}
