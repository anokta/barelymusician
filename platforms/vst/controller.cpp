#include "vst/controller.h"

#include <array>
#include <cassert>
#include <span>

namespace barely::vst {

using ::Steinberg::tresult;
using ::Steinberg::Vst::RangeParameter;

namespace {

const std::array<RangeParameter, BarelyInstrumentControlType_kCount> kParams = {
    RangeParameter(STR16("Gain"), BarelyInstrumentControlType_kGain, STR16(""), 0.0, 1.0, 1.0),
    RangeParameter(STR16("Pitch Shift"), BarelyInstrumentControlType_kPitchShift, STR16(""), -4.0,
                   4.0, 0.0),
    RangeParameter(STR16("Stereo Pan"), BarelyInstrumentControlType_kStereoPan, STR16(""), -1.0,
                   1.0, 0.0),
    RangeParameter(STR16("Attack"), BarelyInstrumentControlType_kAttack, STR16("sec"), 0.0, 8.0,
                   0.01),
    RangeParameter(STR16("Decay"), BarelyInstrumentControlType_kDecay, STR16("sec"), 0.0, 8.0, 0.0),
    RangeParameter(STR16("Sustain"), BarelyInstrumentControlType_kSustain, STR16(""), 0.0, 1.0,
                   1.0),
    RangeParameter(STR16("Release"), BarelyInstrumentControlType_kRelease, STR16("sec"), 0.0, 8.0,
                   0.05),
    RangeParameter(STR16("Slice Mode"), BarelyInstrumentControlType_kSliceMode, STR16(""), 0,
                   BarelySliceMode_kCount - 1, 0, BarelySliceMode_kCount - 1),
    RangeParameter(STR16("Osc Mix"), BarelyInstrumentControlType_kOscMix, STR16(""), 0.0, 1.0, 1.0),
    RangeParameter(STR16("Osc Mode"), BarelyInstrumentControlType_kOscMode, STR16(""), 0,
                   BarelyOscMode_kCount - 1, 0, BarelyOscMode_kCount - 1),
    RangeParameter(STR16("Osc Noise Mix"), BarelyInstrumentControlType_kOscNoiseMix, STR16(""), 0.0,
                   1.0, 0.0),
    RangeParameter(STR16("Osc Pitch Shift"), BarelyInstrumentControlType_kOscPitchShift, STR16(""),
                   -4.0, 4.0, 0.0),
    RangeParameter(STR16("Osc Shape"), BarelyInstrumentControlType_kOscShape, STR16(""), 0.0, 1.0,
                   0.0),
    RangeParameter(STR16("Osc Skew"), BarelyInstrumentControlType_kOscSkew, STR16(""), -1.0, 1.0,
                   0.0),
    RangeParameter(STR16("Crush Depth"), BarelyInstrumentControlType_kCrushDepth, STR16(""), 0.0,
                   1.0, 0.0),
    RangeParameter(STR16("Crush Rate"), BarelyInstrumentControlType_kCrushRate, STR16(""), 0.0, 1.0,
                   0.0),
    RangeParameter(STR16("Distortion Mix"), BarelyInstrumentControlType_kDistortionMix, STR16(""),
                   0.0, 1.0, 0.0),
    RangeParameter(STR16("Distortion Drive"), BarelyInstrumentControlType_kDistortionDrive,
                   STR16(""), 0.0, 1.0, 0.0),
    RangeParameter(STR16("Filter Cutoff"), BarelyInstrumentControlType_kFilterCutoff, STR16(""),
                   0.0, 1.0, 1.0),
    RangeParameter(STR16("Filter Resonance"), BarelyInstrumentControlType_kFilterResonance,
                   STR16(""), 0.0, 1.0, 0.5),
    RangeParameter(STR16("Filter Tone"), BarelyInstrumentControlType_kFilterTone, STR16(""), -1.0,
                   1.0, 0.0),
    RangeParameter(STR16("Delay Send"), BarelyInstrumentControlType_kDelaySend, STR16(""), 0.0, 1.0,
                   0.0),
    RangeParameter(STR16("Reverb Send"), BarelyInstrumentControlType_kReverbSend, STR16(""), 0.0,
                   1.0, 0.0),
    RangeParameter(STR16("Sidechain Send"), BarelyInstrumentControlType_kSidechainSend, STR16(""),
                   -1.0, 1.0, 0.0),
    RangeParameter(STR16("Arp Mode"), BarelyInstrumentControlType_kArpMode, STR16(""), 0,
                   BarelyArpMode_kCount - 1, 0, BarelyArpMode_kCount - 1),
    RangeParameter(STR16("Arp Gate"), BarelyInstrumentControlType_kArpGate, STR16(""), 0.0, 1.0,
                   0.5),
    RangeParameter(STR16("Arp Rate"), BarelyInstrumentControlType_kArpRate, STR16(""), 0.0, 16.0,
                   1.0),
    RangeParameter(STR16("Retrigger"), BarelyInstrumentControlType_kRetrigger, STR16(""), 0, 1, 0,
                   1),
    RangeParameter(STR16("Voice Count"), BarelyInstrumentControlType_kVoiceCount, STR16(""), 1, 16,
                   8, 15),
};

}

const Steinberg::FUID Controller::kId(0x3bd2bd7a, 0x5e0940bb, 0x8b9db4af, 0x6175e06f);

Steinberg::FUnknown* Controller::Create(void* /*context*/) {
  return static_cast<Steinberg::Vst::IEditController*>(new Controller());
}

float Controller::ToPlainControlValue(InstrumentControlType type,
                                      double normalized_value) noexcept {
  assert(static_cast<int>(type) < BarelyInstrumentControlType_kCount && "Invalid control type");
  return static_cast<float>(kParams[static_cast<int>(type)].toPlain(normalized_value));
}

tresult PLUGIN_API Controller::initialize(Steinberg::FUnknown* context) {
  const tresult result = EditController::initialize(context);
  if (result != Steinberg::kResultOk) {
    return result;
  }

  // Initialize plugin parameters.
  for (int i = 0; i < BarelyInstrumentControlType_kCount; ++i) {
    if (i == BarelyInstrumentControlType_kOscMix || i == BarelyInstrumentControlType_kOscMode ||
        i == BarelyInstrumentControlType_kOscPitchShift ||
        i == BarelyInstrumentControlType_kSliceMode ||
        i == BarelyInstrumentControlType_kDelaySend ||
        i == BarelyInstrumentControlType_kReverbSend ||
        i == BarelyInstrumentControlType_kSidechainSend ||
        i == BarelyInstrumentControlType_kArpMode || i == BarelyInstrumentControlType_kArpGate ||
        i == BarelyInstrumentControlType_kArpRate) {
      // TODO(#162): Support sampling features.
      continue;
    }
    auto* param = new Steinberg::Vst::RangeParameter(kParams[i]);
    param->setPrecision((param->getInfo().stepCount == 0) ? 2 : 0);
    parameters.addParameter(param);
  }

  // TODO(#162): Add support for note expression parameters.

  return result;
}

tresult PLUGIN_API Controller::setComponentState(Steinberg::IBStream* /*state*/) {
  return Steinberg::kResultOk;
}

tresult PLUGIN_API Controller::setParamNormalized(Steinberg::Vst::ParamID /*tag*/,
                                                  Steinberg::Vst::ParamValue /*value*/) {
  return Steinberg::kResultOk;
}

}  // namespace barely::vst
