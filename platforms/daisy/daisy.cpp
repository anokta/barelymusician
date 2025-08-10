#include <barelymusician.h>

#include <algorithm>
#include <array>

#include "daisy_pod.h"

using ::barely::ControlType;
using ::barely::Engine;
using ::barely::Instrument;
using ::barely::kStereoChannelCount;
using ::daisy::AudioHandle;
using ::daisy::DaisyPod;
using ::daisy::MidiMessageType;
using ::daisy::MidiUsbHandler;
using ::daisy::MidiUsbTransport;
using ::daisy::SaiHandle;

// System audio settings.
constexpr int kSampleRate = 48000;
constexpr size_t kFrameCount = 16;

// Instrument settings.
constexpr float kGain = 0.125f;
constexpr float kAttack = 0.05f;
constexpr float kRelease = 0.125f;
constexpr float kOscShapeIncrement = 0.1f;
constexpr int kVoiceCount = 16;

static DaisyPod hw;  // target the Daisy Pod hardware.
static MidiUsbHandler midi;

static Engine* engine_ptr = nullptr;
static Instrument* instrument_ptr = nullptr;
static float osc_shape = 0.0f;
static std::array<float, kFrameCount> output_samples;

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size) {
  // Update controls.
  hw.ProcessAllControls();

  if (const auto increment = hw.encoder.Increment(); increment != 0) {
    osc_shape =
        std::clamp(osc_shape + kOscShapeIncrement * static_cast<float>(increment), 0.0f, 1.0f);
    instrument_ptr->SetControl(ControlType::kOscShape, osc_shape);
  }

  // Process the output samples.
  const int output_frame_count = static_cast<int>(size);
  engine_ptr->Process(output_samples.data(), kStereoChannelCount, output_frame_count, 0.0);
  for (int frame = 0; frame < output_frame_count; ++frame) {
    out[0][frame] = output_samples[frame * kStereoChannelCount];
    out[1][frame] = output_samples[frame * kStereoChannelCount + 1];
  }
}

int main(void) {
  // Initialize the Daisy hardware.
  hw.Init();
  hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
  hw.SetAudioBlockSize(kFrameCount);

  // Initialize USB MIDI.
  MidiUsbHandler::Config midi_cfg;
  midi_cfg.transport_config.periph = MidiUsbTransport::Config::INTERNAL;
  midi.Init(midi_cfg);

  // Initialize the instrument.
  Engine engine(kSampleRate);
  engine_ptr = &engine;

  Instrument instrument = engine.CreateInstrument({{
      {ControlType::kGain, kGain},
      {ControlType::kOscMix, 1.0f},
      {ControlType::kOscShape, osc_shape},
      {ControlType::kAttack, kAttack},
      {ControlType::kRelease, kRelease},
      {ControlType::kVoiceCount, kVoiceCount},
  }});
  instrument_ptr = &instrument;

  // Start processing.
  hw.StartAdc();
  hw.StartAudio(AudioCallback);

  while (true) {
    // Listen to MIDI events.
    midi.Listen();

    while (midi.HasEvents()) {
      auto midi_event = midi.PopEvent();
      switch (midi_event.type) {
        case MidiMessageType::NoteOn:
          if (const auto note_on_event = midi_event.AsNoteOn(); note_on_event.velocity != 0) {
            instrument.SetNoteOn(note_on_event.note);
          }
          break;
        case MidiMessageType::NoteOff:
          instrument.SetNoteOff(midi_event.AsNoteOff().note);
          break;
        default:
          break;
      }
    }
  }
}
