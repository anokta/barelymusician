#include <barelymusician.h>

#include <algorithm>
#include <array>
#include <optional>

#include "daisy_pod.h"

namespace {

using ::barely::Engine;
using ::barely::Instrument;
using ::barely::InstrumentControlType;
using ::daisy::AudioHandle;
using ::daisy::DaisyPod;
using ::daisy::MidiMessageType;
using ::daisy::MidiUsbHandler;
using ::daisy::MidiUsbTransport;
using ::daisy::SaiHandle;

// System audio settings.
constexpr int kSampleRate = 48000;
constexpr int kChannelCount = 2;
constexpr int kFrameCount = 16;

// Instrument settings.
constexpr float kGain = 0.125f;
constexpr float kAttack = 0.05f;
constexpr float kRelease = 0.125f;
constexpr float kOscShapeIncrement = 0.1f;
constexpr int kVoiceCount = 16;

DaisyPod g_hw;  // target the Daisy Pod hardware.
MidiUsbHandler g_midi;

std::optional<Engine> g_engine = std::nullopt;
std::optional<Instrument> g_instrument = std::nullopt;
float g_osc_shape = 0.0f;
std::array<float, kChannelCount * kFrameCount> g_output_samples;

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size) {
  // Update controls.
  g_hw.ProcessAllControls();
  if (const auto increment = g_hw.encoder.Increment(); increment != 0) {
    g_osc_shape =
        std::clamp(g_osc_shape + kOscShapeIncrement * static_cast<float>(increment), 0.0f, 1.0f);
    g_instrument->SetControl(InstrumentControlType::kOscShape, g_osc_shape);
  }
  // Process the output samples.
  const int frame_count = static_cast<int>(size);
  g_engine->Process(g_output_samples.data(), kChannelCount, frame_count, /*timestamp=*/0.0);
  for (int frame = 0; frame < frame_count; ++frame) {
    for (int channel = 0; channel < kChannelCount; ++channel) {
      out[channel][frame] = g_output_samples[frame * kChannelCount + channel];
    }
  }
}

}  // namespace

int main(void) {
  // Initialize the Daisy hardware.
  g_hw.Init();
  g_hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
  g_hw.SetAudioBlockSize(kFrameCount);

  // Initialize USB MIDI.
  MidiUsbHandler::Config midi_cfg;
  midi_cfg.transport_config.periph = MidiUsbTransport::Config::INTERNAL;
  g_midi.Init(midi_cfg);

  // Initialize the instrument.
  g_engine.emplace(kSampleRate, kFrameCount);
  g_instrument = g_engine->CreateInstrument({{
      {InstrumentControlType::kGain, kGain},
      {InstrumentControlType::kOscMix, 1.0f},
      {InstrumentControlType::kOscShape, g_osc_shape},
      {InstrumentControlType::kAttack, kAttack},
      {InstrumentControlType::kRelease, kRelease},
      {InstrumentControlType::kVoiceCount, kVoiceCount},
  }});

  // Start processing.
  g_hw.StartAdc();
  g_hw.StartAudio(AudioCallback);

  while (true) {
    // Listen to MIDI events.
    g_midi.Listen();

    while (g_midi.HasEvents()) {
      auto midi_event = g_midi.PopEvent();
      switch (midi_event.type) {
        case MidiMessageType::NoteOn:
          if (const auto note_on_event = midi_event.AsNoteOn(); note_on_event.velocity != 0) {
            g_instrument->SetNoteOn(note_on_event.note);
          }
          break;
        case MidiMessageType::NoteOff:
          g_instrument->SetNoteOff(midi_event.AsNoteOff().note);
          break;
        default:
          break;
      }
    }
  }
}
