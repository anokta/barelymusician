#ifndef BARELYMUSICIAN_EXAMPLES_COMMON_WAV_FILE_H_
#define BARELYMUSICIAN_EXAMPLES_COMMON_WAV_FILE_H_

#include <string>
#include <vector>

namespace barely::examples {

// Wraps a RIFF Wave file.
class WavFile {
 public:
  bool Load(const std::string& file_path);

  [[nodiscard]] int GetChannelCount() const noexcept { return channel_count_; }
  [[nodiscard]] int GetSampleRate() const noexcept { return sample_rate_; }
  [[nodiscard]] const std::vector<float>& GetData() const noexcept { return data_; }

 private:
  int channel_count_ = 0;
  int sample_rate_ = 0;
  std::vector<float> data_;
};

}  // namespace barely::examples

#endif  // BARELYMUSICIAN_EXAMPLES_COMMON_WAV_FILE_H_
