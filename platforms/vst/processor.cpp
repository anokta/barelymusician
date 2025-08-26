#include "vst/processor.h"

#include <algorithm>

#include "base/source/fstreamer.h"
#include "pluginterfaces/base/funknown.h"
#include "pluginterfaces/vst/ivstevents.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"
#include "public.sdk/source/main/pluginfactory.h"
#include "public.sdk/source/vst/vstaudioeffect.h"
#include "vst/controller.h"

namespace barely::vst {

namespace {

float MidiNoteToPitch(Steinberg::int16 midi_note) noexcept {
  return (static_cast<float>(midi_note) - 60.0f) / 12.0f;
}

}  // namespace

const Steinberg::FUID Processor::kId(0xf58580c8, 0x45ec40a5, 0xa870850c, 0xbe9fcc75);

Steinberg::FUnknown* Processor::Create(void* /*context*/) {
  return static_cast<Steinberg::Vst::IAudioProcessor*>(new Processor());
}

Processor::Processor() noexcept { setControllerClass(Controller::kId); }

Steinberg::tresult PLUGIN_API Processor::canProcessSampleSize(Steinberg::int32 sample_size) {
  if (sample_size == Steinberg::Vst::kSample32) {
    return Steinberg::kResultTrue;
  }
  return Steinberg::kResultFalse;
}

Steinberg::tresult PLUGIN_API Processor::initialize(FUnknown* context) {
  const Steinberg::tresult result = AudioEffect::initialize(context);
  if (result != Steinberg::kResultTrue) {
    return result;
  }

  addAudioOutput(STR16("Audio Output"), Steinberg::Vst::SpeakerArr::kStereo);
  addEventInput(STR16("Event Input"));

  return result;
}

Steinberg::tresult PLUGIN_API Processor::process(Steinberg::Vst::ProcessData& data) {
  if (data.numOutputs == 0 || data.numSamples <= 0) {
    return Steinberg::kResultTrue;
  }
  if (!engine_.has_value() || !instrument_.has_value()) {
    return Steinberg::kResultTrue;
  }

  // TODO(#162): The parameter changes and the MIDI events below can be made sample accurate.

  // Process parameter changes.
  if (data.inputParameterChanges) {
    for (Steinberg::int32 i = 0; i < data.inputParameterChanges->getParameterCount(); ++i) {
      auto* param_queue = data.inputParameterChanges->getParameterData(i);
      if (param_queue == nullptr) {
        continue;
      }

      const ControlType type = static_cast<ControlType>(param_queue->getParameterId());
      for (int queue_index = 0; queue_index < param_queue->getPointCount(); ++queue_index) {
        Steinberg::int32 sample_offset = 0;
        double value = 0.0;
        if (param_queue->getPoint(queue_index, sample_offset, value) == Steinberg::kResultTrue) {
          instrument_->SetControl(type, Controller::ToPlainControlValue(type, value));
        }
      }
    }
  }

  // Process MIDI events.
  if (data.inputEvents) {
    for (Steinberg::int32 i = 0; i < data.inputEvents->getEventCount(); ++i) {
      Steinberg::Vst::Event event;
      if (data.inputEvents->getEvent(i, event) != Steinberg::kResultTrue) {
        continue;
      }
      if (event.type == Steinberg::Vst::Event::kNoteOnEvent) {
        instrument_->SetNoteOn(MidiNoteToPitch(event.noteOn.pitch), event.noteOn.velocity);
      } else if (event.type == Steinberg::Vst::Event::kNoteOffEvent) {
        instrument_->SetNoteOff(MidiNoteToPitch(event.noteOff.pitch));
      }
    }
  }

  // Process instrument.
  engine_->Process({data.outputs[0].channelBuffers32,
                    data.outputs[0].channelBuffers32 + data.outputs[0].numChannels},
                   data.numSamples, /*timestamp=*/0.0);

  return Steinberg::kResultTrue;
}

Steinberg::tresult PLUGIN_API Processor::setBusArrangements(
    Steinberg::Vst::SpeakerArrangement* inputs, Steinberg::int32 input_count,
    Steinberg::Vst::SpeakerArrangement* outputs, Steinberg::int32 output_count) {
  if (input_count > 0) {
    return Steinberg::kResultFalse;
  }
  if (output_count != 1 || outputs[0] != Steinberg::Vst::SpeakerArr::kStereo) {
    return Steinberg::kResultFalse;
  }
  return AudioEffect::setBusArrangements(inputs, input_count, outputs, output_count);
}

Steinberg::tresult PLUGIN_API Processor::setProcessing(Steinberg::TBool /*state*/) {
  return Steinberg::kResultTrue;
}

Steinberg::tresult PLUGIN_API Processor::setupProcessing(Steinberg::Vst::ProcessSetup& setup) {
  instrument_ = std::nullopt;
  engine_ = Engine(static_cast<int>(setup.sampleRate), static_cast<int>(setup.maxSamplesPerBlock));
  instrument_ = engine_->CreateInstrument(Controller::GetDefaultControls());
  return Steinberg::kResultTrue;
}

Steinberg::tresult PLUGIN_API Processor::getState(Steinberg::IBStream* state) {
  Steinberg::IBStreamer stream(state, kLittleEndian);

  if (!stream.writeBool(engine_.has_value() && instrument_.has_value())) {
    return Steinberg::kResultFalse;
  }

  for (int i = 0; i < BarelyControlType_kCount; ++i) {
    if (!stream.writeFloat(instrument_->GetControl<float>(static_cast<ControlType>(i)))) {
      return Steinberg::kResultFalse;
    }
  }

  return Steinberg::kResultTrue;
}

Steinberg::tresult PLUGIN_API Processor::setState(Steinberg::IBStream* state) {
  Steinberg::IBStreamer stream(state, kLittleEndian);

  bool initialized = false;
  if (!stream.readBool(initialized)) {
    return Steinberg::kResultFalse;
  }
  if (!initialized) {
    return Steinberg::kResultTrue;
  }

  if (!engine_.has_value() || !instrument_.has_value()) {
    return Steinberg::kResultFalse;
  }

  for (int i = 0; i < BarelyControlType_kCount; ++i) {
    float value = 0.0f;
    if (!stream.readFloat(value)) {
      return Steinberg::kResultFalse;
    }
    instrument_->SetControl<float>(static_cast<ControlType>(i), value);
  }

  return Steinberg::kResultTrue;
}

}  // namespace barely::vst
