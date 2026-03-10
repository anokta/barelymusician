#include "vst/controller.h"

#include <array>
#include <cassert>
#include <span>

namespace barely::vst {

using ::Steinberg::tresult;
using ::Steinberg::Vst::RangeParameter;

namespace {

#define BARELY_DEFINE_VST_PARAM(EnumType, Name, Default, Min, Max, Label) \
  RangeParameter(STR16(#Label), Barely##EnumType##_k##Name, STR16(""), Min, Max, Default),

const std::array<RangeParameter, BarelyInstrumentControlType_kCount> kParams = {
    BARELY_INSTRUMENT_CONTROL_TYPES(InstrumentControlType, BARELY_DEFINE_VST_PARAM)};

}  // namespace

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
