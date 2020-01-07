#ifndef BARELYMUSICIAN_INSTRUMENT_MODULATION_MATRIX_H_
#define BARELYMUSICIAN_INSTRUMENT_MODULATION_MATRIX_H_

#include <functional>
#include <unordered_map>
#include <utility>

#include "barelymusician/base/logging.h"

namespace barelyapi {

// Type agnostic parameter modulation matrix.
template <typename ParamType>
class ModulationMatrix {
 public:
  // Parameter update function signature.
  using UpdateParamFn = std::function<void(const ParamType& value)>;

  // Returns the value of a parameter with the given ID.
  //
  // @param id Parameter ID.
  // @param value Parameter value to be read into, must a valid pointer.
  // @return True if successful.
  bool GetParam(int id, ParamType* value) const;

  // Registers new parameter.
  //
  // @param id Parameter ID.
  // @param default_value Parameter default value.
  // @param update_param_fn Parameter update function.
  void Register(int id, const ParamType& default_value,
                UpdateParamFn&& update_param_fn = nullptr);

  // Sets the value of a parameter with the given ID.
  //
  // @param id Parameter ID.
  // @param value Parameter value to be set.
  // @return True if successful.
  bool SetParam(int id, const ParamType& value);

 private:
  // Parameter data.
  struct ParamData {
    // Current value.
    ParamType value;

    // Update function.
    UpdateParamFn update_param_fn;
  };

  // Parameter map.
  std::unordered_map<int, ParamData> params_;
};

template <typename ParamType>
bool ModulationMatrix<ParamType>::GetParam(int id, ParamType* value) const {
  const auto it = params_.find(id);
  if (it == params_.cend()) {
    return false;
  }
  DCHECK(value);
  *value = it->second.value;
  return true;
}

template <typename ParamType>
void ModulationMatrix<ParamType>::Register(int id,
                                           const ParamType& default_value,
                                           UpdateParamFn&& update_param_fn) {
  const auto [it, success] =
      params_.insert({id, {default_value, std::move(update_param_fn)}});
  DCHECK(success) << "Failed to register param ID: " << id;

  const auto& param_data = it->second;
  if (param_data.update_param_fn != nullptr) {
    param_data.update_param_fn(default_value);
  }
}

template <typename ParamType>
bool ModulationMatrix<ParamType>::SetParam(int id, const ParamType& value) {
  const auto it = params_.find(id);
  if (it == params_.cend()) {
    return false;
  }
  auto& param_data = it->second;
  if (value != param_data.value) {
    if (param_data.update_param_fn != nullptr) {
      param_data.update_param_fn(value);
    }
    param_data.value = value;
  }
  return true;
}

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_INSTRUMENT_MODULATION_MATRIX_H_
