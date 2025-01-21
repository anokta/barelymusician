#include <algorithm>

#include "barelymusician.h"
#include "daisy_pod.h"

using ::barely::ControlType;
using ::barely::Instrument;
using ::barely::Musician;
using ::barely::OscillatorShape;
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
constexpr float kGain = -18.0f;
constexpr OscillatorShape kOscillatorShape = OscillatorShape::kSquare;
constexpr float kAttack = 0.05f;
constexpr float kRelease = 0.125f;
constexpr int kVoiceCount = 16;

constexpr int kOscCount = static_cast<int>(BarelyOscillatorShape_kCount);

static DaisyPod hw;  // target the Daisy Pod hardware.
static MidiUsbHandler midi;

static Instrument* instrument_ptr = nullptr;
static int osc_index = static_cast<int>(kOscillatorShape);

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size) {
  // Update controls.
  hw.ProcessAllControls();

  if (const auto increment = hw.encoder.Increment(); increment != 0) {
    osc_index = (osc_index + increment + kOscCount) % kOscCount;
    instrument_ptr->SetControl(ControlType::kOscillatorShape,
                               static_cast<OscillatorShape>(osc_index));
  }

  // Process samples.
  instrument_ptr->Process({out[0], out[0] + size}, /*timestamp=*/0.0);
  std::copy_n(out[0], size, out[1]);  // copy onto stereo buffer.
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
  Musician musician(kSampleRate);

  Instrument instrument = musician.CreateInstrument();
  instrument.SetControl(ControlType::kGain, kGain);
  instrument.SetControl(ControlType::kOscillatorShape, kOscillatorShape);
  instrument.SetControl(ControlType::kAttack, kAttack);
  instrument.SetControl(ControlType::kRelease, kRelease);
  instrument.SetControl(ControlType::kVoiceCount, kVoiceCount);
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
