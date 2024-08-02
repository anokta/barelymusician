#include <array>

#include "barelymusician/barelymusician.h"
#include "barelymusician/composition/pitch.h"
#include "barelymusician/dsp/oscillator.h"
#include "barelymusician/instruments/synth_instrument.h"
#include "daisy_pod.h"

using ::barely::OscillatorType;
using ::barely::PitchFromMidi;
using ::barely::Musician;
using ::barely::Scoped;
using ::barely::SynthInstrument;
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
constexpr OscillatorType kOscillatorType = OscillatorType::kSquare;
constexpr double kAttack = 0.05;
constexpr double kRelease = 0.125;
constexpr int kVoiceCount = 16;

constexpr int kOscCount = static_cast<int>(OscillatorType::kCount);

static DaisyPod hw;  // Currently targets the Daisy Pod hardware.
static MidiUsbHandler midi;

static barely::Instrument* instrument_ptr = nullptr;
static int osc_index = static_cast<int>(kOscillatorType);
static std::array<double, kChannelCount * kFrameCount> temp_samples{0.0};

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size) {
  // Update controls.
  hw.ProcessAllControls();

  if (const auto increment = hw.encoder.Increment(); increment != 0) {
    osc_index = (osc_index + increment + kOscCount) % kOscCount;
    instrument_ptr->SetControl(SynthInstrument::Control::kOscillatorType,
                               static_cast<OscillatorType>(osc_index));
  }

  // Process samples.
  instrument_ptr->Process(temp_samples.data(), kChannelCount, size, /*timestamp=*/0.0);
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
  Scoped<Musician> musician;

  auto instrument = musician.CreateInstrument<SynthInstrument>(kFrameRate);
  instrument.SetControl(SynthInstrument::Control::kGain, kGain);
  instrument.SetControl(SynthInstrument::Control::kOscillatorType, kOscillatorType);
  instrument.SetControl(SynthInstrument::Control::kAttack, kAttack);
  instrument.SetControl(SynthInstrument::Control::kRelease, kRelease);
  instrument.SetControl(SynthInstrument::Control::kVoiceCount, kVoiceCount);

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
            instrument.SetNoteOn(PitchFromMidi(note_on_event.note));
          }
          break;
        case MidiMessageType::NoteOff:
          instrument.SetNoteOff(PitchFromMidi(midi_event.AsNoteOff().note));
          break;
        default:
          break;
      }
    }
  }
}
