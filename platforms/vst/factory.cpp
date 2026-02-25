#include "pluginterfaces/vst/ivstcomponent.h"
#include "public.sdk/source/main/pluginfactory.h"
#include "vst/controller.h"
#include "vst/processor.h"

namespace {

SMTG_CONSTEXPR Steinberg::Vst::CString kPluginVendor = "anokta";
SMTG_CONSTEXPR Steinberg::Vst::CString kPluginUrl = "https://anokta.io";
SMTG_CONSTEXPR Steinberg::Vst::CString kPluginEmail = "mailto:contact@anokta.io";

SMTG_CONSTEXPR Steinberg::Vst::CString kPluginName = "barelymusician";
SMTG_CONSTEXPR Steinberg::Vst::CString kPluginVersion = "0.7.3";
SMTG_CONSTEXPR Steinberg::Vst::ComponentFlags kPluginFlags = {};

}  // namespace

// clang-format off
BEGIN_FACTORY_DEF(kPluginVendor, kPluginUrl, kPluginEmail)

DEF_CLASS2(
  INLINE_UID_FROM_FUID(barely::vst::Controller::kId),
  Steinberg::PClassInfo::kManyInstances,
  kVstComponentControllerClass,
  kPluginName,
  kPluginFlags,
  nullptr,
  kPluginVersion,
  kVstVersionString,
  barely::vst::Controller::Create
)

DEF_CLASS2(
  INLINE_UID_FROM_FUID(barely::vst::Processor::kId),
  Steinberg::PClassInfo::kManyInstances,
  kVstAudioEffectClass,
  kPluginName,
  kPluginFlags,
  Steinberg::Vst::PlugType::kInstrumentSynth,
  kPluginVersion,
  kVstVersionString,
  barely::vst::Processor::Create
)

END_FACTORY
// clang-format on
