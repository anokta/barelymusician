#ifndef BARELYMUSICIAN_INSTRUMENT_MODULATION_MATRIX_H_
#define BARELYMUSICIAN_INSTRUMENT_MODULATION_MATRIX_H_

#include <functional>
#include <unordered_map>
#include <utility>

#include "barelymusician/base/logging.h"
#include "barelymusician/base/types.h"

namespace barelyapi {

template <typename ParamType>
class ModulationMatrix {
 public:
  using ParamUpdater = std::function<void(const ParamType&)>;

  void Register(ParamId id, const ParamType& default_value,
                ParamUpdater&& update_param);

  void Reset();

  bool GetParam(ParamId id, ParamType* value) const;

  bool SetParam(ParamId id, const ParamType& value);

 private:
  struct ParamData {
    const ParamType default_value;

    ParamType current_value;
    ParamUpdater update_param;
  };

  std::unordered_map<ParamId, ParamData> params_;
};

template <typename ParamType>
void ModulationMatrix<ParamType>::Register(ParamId id,
                                           const ParamType& default_value,
                                           ParamUpdater&& update_param) {
  const auto result = params_.insert(
      {id, {default_value, default_value, std::move(update_param)}});
  if (!result.second) {
    LOG(ERROR) << "Failed to register param ID: " << id;
    return;
  }
  result.first->second.update_param(default_value);
}

template <typename ParamType>
void ModulationMatrix<ParamType>::Reset() {
  for (auto& it : params_) {
    auto* param_data = &it.second;
    param_data->update_param(param_data->default_value);
    param_data->current_value = param_data->default_value;
  }
}

template <typename ParamType>
bool ModulationMatrix<ParamType>::GetParam(ParamId id, ParamType* value) const {
  DCHECK(value);
  const auto it = params_.find(id);
  if (it == params_.end()) {
    return false;
  }
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
    param_data->update_param(value);
    param_data->current_value = value;
  }
  return true;
}

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_INSTRUMENT_MODULATION_MATRIX_H_
