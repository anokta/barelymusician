#ifndef BARELYMUSICIAN_VST_CONTROLLER_H_
#define BARELYMUSICIAN_VST_CONTROLLER_H_

#include <barelymusician.h>

#include <span>

#include "public.sdk/source/vst/vsteditcontroller.h"

namespace barely::vst {

class Controller : public Steinberg::Vst::EditController {
 public:
  /// Creates a `Controller` instance for the VST3 factory.
  static Steinberg::FUnknown* Create(void* context);

  /// Returns the span of default control values.
  static std::span<InstrumentControlOverride> GetDefaultControls() noexcept;

  /// Converts a normalized parameter value to a plain control value.
  static float ToPlainControlValue(InstrumentControlType type, double normalized_value) noexcept;

  /// Identifier.
  static const Steinberg::FUID kId;

  /// Constructs a new `Controller`.
  Controller() noexcept = default;

  /// Implements `Steinberg::Vst::EditController`.
  Steinberg::tresult PLUGIN_API initialize(Steinberg::FUnknown* context) SMTG_OVERRIDE;
  Steinberg::tresult PLUGIN_API setComponentState(Steinberg::IBStream* state);
  Steinberg::tresult PLUGIN_API setParamNormalized(Steinberg::Vst::ParamID tag,
                                                   Steinberg::Vst::ParamValue value) SMTG_OVERRIDE;
};

}  // namespace barely::vst

#endif  // BARELYMUSICIAN_VST_CONTROLLER_H_
