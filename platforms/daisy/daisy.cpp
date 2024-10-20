#include "barelymusician/barelymusician.h"
#include "barelymusician/dsp/oscillator.h"
#include "daisy_pod.h"

using ::barely::ControlType;
using ::barely::Instrument;
using ::barely::InstrumentHandle;
using ::barely::Musician;
using ::barely::OscillatorShape;
using ::daisy::AudioHandle;
using ::daisy::DaisyPod;
using ::daisy::MidiMessageType;
using ::daisy::MidiUsbHandler;
using ::daisy::MidiUsbTransport;
using ::daisy::SaiHandle;

// System audio settings.
constexpr int kFrameRate = 48000;
constexpr size_t kChannelCount = 2;
constexpr size_t kFrameCount = 16;

// Instrument settings.
constexpr double kGain = 0.125;
constexpr OscillatorShape kOscillatorShape = OscillatorShape::kSquare;
constexpr double kAttack = 0.05;
constexpr double kRelease = 0.125;
constexpr int kVoiceCount = 16;

constexpr int kOscCount = static_cast<int>(OscillatorShape::kCount);

static DaisyPod hw;  // Currently targets the Daisy Pod hardware.
static MidiUsbHandler midi;

static InstrumentHandle instrument = {};
static int osc_index = static_cast<int>(kOscillatorShape);
static std::array<double, kChannelCount * kFrameCount> temp_samples{0.0};

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size) {
  // Update controls.
  hw.ProcessAllControls();

  if (const auto increment = hw.encoder.Increment(); increment != 0) {
    osc_index = (osc_index + increment + kOscCount) % kOscCount;
    instrument.SetControl(ControlType::kOscillatorShape, static_cast<OscillatorShape>(osc_index));
  }

  // Process samples.
  instrument.Process(temp_samples.data(), kChannelCount, size, /*timestamp=*/0.0);
  for (int channel = 0; channel < kChannelCount; ++channel) {
    for (int frame = 0; frame < static_cast<int>(size); ++frame) {
      out[channel][frame] = temp_samples[frame * kChannelCount + channel];
    }
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
  Musician musician(kFrameRate);

  instrument = musician.AddInstrument();
  instrument.SetControl(ControlType::kGain, kGain);
  instrument.SetControl(ControlType::kOscillatorShape, kOscillatorShape);
  instrument.SetControl(ControlType::kAttack, kAttack);
  instrument.SetControl(ControlType::kRelease, kRelease);
  instrument.SetControl(ControlType::kVoiceCount, kVoiceCount);

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
