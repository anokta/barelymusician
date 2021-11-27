#ifndef EXAMPLES_COMMON_WAV_FILE_H_
#define EXAMPLES_COMMON_WAV_FILE_H_

#include <string>
#include <vector>

namespace barely::examples {

/// RIFF Wave file.
class WavFile {
 public:
  /// Returns the audio data.
  ///
  /// @return Audio data.
  const std::vector<float>& GetData() const noexcept;

  /// Returns the number of audio channels.
  ///
  /// @return Number of audio channels.
  int GetNumChannels() const noexcept;

  /// Returns the audio sampling rate.
  ///
  /// @return Audio sampling rate.
  int GetSampleRate() const noexcept;

  /// Loads Wave file with the given |file_path|.
  ///
  /// @param file_path Absolute file path.
  /// @return True if successful.
  bool Load(const std::string& file_path) noexcept;

 private:
  // Number of audio channels.
  int num_channels_ = 0;

  // Audio sampling rate.
  int sample_rate_ = 0;

  // Audio data.
  std::vector<float> data_;
};

}  // namespace barely::examples

#endif  // EXAMPLES_COMMON_WAV_FILE_H_
