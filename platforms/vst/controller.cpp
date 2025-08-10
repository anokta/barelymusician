#include "vst/controller.h"

#include <array>
#include <cassert>
#include <span>

namespace barely::vst {

using ::Steinberg::tresult;
using ::Steinberg::Vst::RangeParameter;

namespace {

const std::array<RangeParameter, BarelyControlType_kCount> kParams = {
    RangeParameter(STR16("Gain"), BarelyControlType_kGain, STR16(""), 0.0, 1.0, 0.125),
    RangeParameter(STR16("Pitch Shift"), BarelyControlType_kPitchShift, STR16(""), -4.0, 4.0, 0.0),
    RangeParameter(STR16("Retrigger"), BarelyControlType_kRetrigger, STR16(""), 0, 1, 0, 1),
    RangeParameter(STR16("Stereo Pan"), BarelyControlType_kStereoPan, STR16(""), -1.0, 1.0, 0.0),
    RangeParameter(STR16("Voice Count"), BarelyControlType_kVoiceCount, STR16(""), 1, 20, 8, 19),
    RangeParameter(STR16("Attack"), BarelyControlType_kAttack, STR16("sec"), 0.0, 60.0, 0.01),
    RangeParameter(STR16("Decay"), BarelyControlType_kDecay, STR16("sec"), 0.0, 60.0, 0.0),
    RangeParameter(STR16("Sustain"), BarelyControlType_kSustain, STR16(""), 0.0, 1.0, 1.0),
    RangeParameter(STR16("Release"), BarelyControlType_kRelease, STR16("sec"), 0.0, 60.0, 0.05),
    RangeParameter(STR16("Osc Mix"), BarelyControlType_kOscMix, STR16(""), 0.0, 1.0, 1.0),
    RangeParameter(STR16("Osc Mode"), BarelyControlType_kOscMode, STR16(""), 0,
                   BarelyOscMode_kCount - 1, 0, BarelyOscMode_kCount - 1),
    RangeParameter(STR16("Osc Noise Mix"), BarelyControlType_kOscNoiseMix, STR16(""), 0.0, 1.0,
                   0.0),
    RangeParameter(STR16("Osc Pitch Shift"), BarelyControlType_kOscPitchShift, STR16(""), -4.0, 4.0,
                   0.0),
    RangeParameter(STR16("Osc Shape"), BarelyControlType_kOscShape, STR16(""), 0.0, 1.0, 0.0),
    RangeParameter(STR16("Osc Skew"), BarelyControlType_kOscSkew, STR16(""), -0.5, 0.5, 0.0),
    RangeParameter(STR16("Slice Mode"), BarelyControlType_kSliceMode, STR16(""), 0,
                   BarelySliceMode_kCount - 1, 0, BarelySliceMode_kCount - 1),
    RangeParameter(STR16("Filter Type"), BarelyControlType_kFilterType, STR16(""), 0,
                   BarelyFilterType_kCount - 1, 0, BarelyFilterType_kCount - 1),
    RangeParameter(STR16("Filter Frequency"), BarelyControlType_kFilterFrequency, STR16("hz"), 0.0,
                   48000.0, 0.0),
    RangeParameter(STR16("Filter Q"), BarelyControlType_kFilterQ, STR16(""), 0.1, 10.0, 0.71),
    RangeParameter(STR16("Bitcrusher Depth"), BarelyControlType_kBitCrusherDepth, STR16(""), 1.0,
                   16.0, 16.0),
    RangeParameter(STR16("Bitcrusher Rate"), BarelyControlType_kBitCrusherRate, STR16(""), 0.0, 1.0,
                   1.0),
};

}

const Steinberg::FUID Controller::kId(0x3bd2bd7a, 0x5e0940bb, 0x8b9db4af, 0x6175e06f);

Steinberg::FUnknown* Controller::Create(void* /*context*/) {
  return static_cast<Steinberg::Vst::IEditController*>(new Controller());
}

std::span<ControlOverride> Controller::GetDefaultControls() noexcept {
  static std::array<ControlOverride, BarelyControlType_kCount> controls;
  for (int i = 0; i < BarelyControlType_kCount; ++i) {
    controls[i] = {
        static_cast<ControlType>(i),
        static_cast<float>(kParams[i].toPlain(kParams[i].getInfo().defaultNormalizedValue)),
    };
  }
  return controls;
}

float Controller::ToPlainControlValue(ControlType type, double normalized_value) noexcept {
  assert(static_cast<int>(type) < BarelyControlType_kCount && "Invalid control type");
  return static_cast<float>(kParams[static_cast<int>(type)].toPlain(normalized_value));
}

tresult PLUGIN_API Controller::initialize(Steinberg::FUnknown* context) {
  const tresult result = EditController::initialize(context);
  if (result != Steinberg::kResultOk) {
    return result;
  }

  // Initialize plugin parameters.
  for (int i = 0; i < BarelyControlType_kCount; ++i) {
    if (i == BarelyControlType_kOscMix || i == BarelyControlType_kOscMode ||
        i == BarelyControlType_kOscPitchShift || i == BarelyControlType_kSliceMode) {
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
