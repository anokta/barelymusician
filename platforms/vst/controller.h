#ifndef BARELYMUSICIAN_VST_CONTROLLER_H_
#define BARELYMUSICIAN_VST_CONTROLLER_H_

#include <barelymusician.h>

#include <span>

#include "public.sdk/source/vst/vsteditcontroller.h"

namespace barely::vst {

class Controller : public Steinberg::Vst::EditController {
 public:
  static Steinberg::FUnknown* Create(void* context);

  static std::span<InstrumentControlOverride> GetDefaultControls() noexcept;
  static float ToPlainControlValue(InstrumentControlType type, double normalized_value) noexcept;

  static const Steinberg::FUID kId;

  Controller() noexcept = default;

  // Implements `Steinberg::Vst::EditController`.
  Steinberg::tresult PLUGIN_API initialize(Steinberg::FUnknown* context) SMTG_OVERRIDE;
  Steinberg::tresult PLUGIN_API setComponentState(Steinberg::IBStream* state);
  Steinberg::tresult PLUGIN_API setParamNormalized(Steinberg::Vst::ParamID tag,
                                                   Steinberg::Vst::ParamValue value) SMTG_OVERRIDE;
};

}  // namespace barely::vst

#endif  // BARELYMUSICIAN_VST_CONTROLLER_H_
