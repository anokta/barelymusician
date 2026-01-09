#ifndef BARELYMUSICIAN_VST_PROCESSOR_H_
#define BARELYMUSICIAN_VST_PROCESSOR_H_

#include <barelymusician.h>

#include <optional>
#include <vector>

#include "public.sdk/source/vst/vstaudioeffect.h"

namespace barely::vst {

class Processor : public Steinberg::Vst::AudioEffect {
 public:
  /// Creates a `Processor` instance for the VST3 factory.
  static Steinberg::FUnknown* Create(void* context);

  /// Identifier.
  static const Steinberg::FUID kId;

  /// Constructs a new `Processor`.
  Processor() noexcept;

  // Implements `Steinberg::Vst::AudioEffect`.
  Steinberg::tresult PLUGIN_API canProcessSampleSize(Steinberg::int32 sample_size) SMTG_OVERRIDE;
  Steinberg::tresult PLUGIN_API initialize(FUnknown* context) SMTG_OVERRIDE;
  Steinberg::tresult PLUGIN_API process(Steinberg::Vst::ProcessData& data) SMTG_OVERRIDE;
  Steinberg::tresult PLUGIN_API setBusArrangements(Steinberg::Vst::SpeakerArrangement* inputs,
                                                   Steinberg::int32 input_count,
                                                   Steinberg::Vst::SpeakerArrangement* outputs,
                                                   Steinberg::int32 output_count) SMTG_OVERRIDE;
  Steinberg::tresult PLUGIN_API setProcessing(Steinberg::TBool state) SMTG_OVERRIDE;
  Steinberg::tresult PLUGIN_API setupProcessing(Steinberg::Vst::ProcessSetup& setup) SMTG_OVERRIDE;
  Steinberg::tresult PLUGIN_API getState(Steinberg::IBStream* state) SMTG_OVERRIDE;
  Steinberg::tresult PLUGIN_API setState(Steinberg::IBStream* state) SMTG_OVERRIDE;

 private:
  std::optional<Engine> engine_;
  Instrument instrument_;
  std::vector<float> output_samples_;
};

}  // namespace barely::vst

#endif  // BARELYMUSICIAN_VST_PROCESSOR_H_
