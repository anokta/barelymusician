#ifndef PLATFORMS_API_BARELYMUSICIAN_H_
#define PLATFORMS_API_BARELYMUSICIAN_H_

#include <cassert>

#include "platforms/capi/barelymusician_v2.h"

namespace barely {

/// BarelyMusician C++ API.
class Api {
 public:
  /// Constructs new |Api|.
  Api() {
    const auto status = BarelyApi_Create(&api_);
    assert(status == BarelyStatus_kOk);
  }

  /// Destroys |Api|.
  ~Api() {
    const auto status = BarelyApi_Destroy(api_);
    assert(status == BarelyStatus_kOk);
  }

  /// Non-copyable and non-movable.
  Api(const Api& other) = delete;
  Api& operator=(const Api& other) noexcept = delete;
  Api(Api&& other) = delete;
  Api& operator=(Api&& other) noexcept = delete;

 private:
  // API handle.
  BarelyApi api_;
};

}  // namespace barely

#endif PLATFORMS_API_BARELYMUSICIAN_H_
