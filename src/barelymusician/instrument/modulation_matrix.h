#ifndef BARELYMUSICIAN_INSTRUMENT_MODULATION_MATRIX_H_
#define BARELYMUSICIAN_INSTRUMENT_MODULATION_MATRIX_H_

#include <functional>
#include <unordered_map>
#include <utility>

#include "barelymusician/base/logging.h"
#include "barelymusician/base/types.h"

namespace barelyapi {

// Type agnostic parameter modulation matrix.
template <typename ParamType>
class ModulationMatrix {
 public:
  // Parameter update signature.
  using ParamUpdater = std::function<void(const ParamType&)>;

  // Registers new parameter.
  //
  // @param id Parameter ID.
  // @param default_value Parameter default value.
  // @param updater Parameter update function.
  void Register(ParamId id, const ParamType& default_value,
                ParamUpdater&& updater = nullptr);

  // Resets all registered parameters to their default values.
  void Reset();

  // Returns the value of a parameter with the given ID.
  //
  // @param id Parameter ID.
  // @param value Parameter value to be read into, must a valid pointer.
  // @return True if successful.
  bool GetParam(ParamId id, ParamType* value) const;

  // Sets the value of a parameter with the given ID.
  //
  // @param id Parameter ID.
  // @param value Parameter value to be set.
  // @return True if successful.
  bool SetParam(ParamId id, const ParamType& value);

 private:
  // Parameter data.
  struct ParamData {
    // Default value.
    const ParamType default_value;

    // Current value.
    ParamType current_value;

    // Update function.
    ParamUpdater updater;
  };

  // Parameter map.
  std::unordered_map<ParamId, ParamData> params_;
};

template <typename ParamType>
void ModulationMatrix<ParamType>::Register(ParamId id,
                                           const ParamType& default_value,
                                           ParamUpdater&& updater) {
  const auto result =
      params_.insert({id, {default_value, default_value, std::move(updater)}});
  DCHECK(result.second) << "Failed to register param ID: " << id;

  const auto& param_data = result.first->second;
  if (param_data.updater != nullptr) {
    param_data.updater(default_value);
  }
}

template <typename ParamType>
void ModulationMatrix<ParamType>::Reset() {
  for (auto& it : params_) {
    auto* param_data = &it.second;
    if (param_data->updater != nullptr) {
      param_data->updater(param_data->default_value);
    }
    param_data->current_value = param_data->default_value;
  }
}

template <typename ParamType>
bool ModulationMatrix<ParamType>::GetParam(ParamId id, ParamType* value) const {
  const auto it = params_.find(id);
  if (it == params_.end()) {
    return false;
  }
  DCHECK(value);
  *value = it->second.current_value;
  return true;
}

template <typename ParamType>
bool ModulationMatrix<ParamType>::SetParam(ParamId id, const ParamType& value) {
  const auto it = params_.find(id);
  if (it == params_.end()) {
    return false;
  }
  auto* param_data = &it->second;
  if (value != param_data->current_value) {
    if (param_data->updater != nullptr) {
      param_data->updater(value);
    }
    param_data->current_value = value;
  }
  return true;
}

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_INSTRUMENT_MODULATION_MATRIX_H_
