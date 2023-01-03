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
  [[nodiscard]] const std::vector<double>& GetData() const noexcept;

  /// Returns the number of audio channels.
  ///
  /// @return Number of audio channels.
  [[nodiscard]] int GetChannelCount() const noexcept;

  /// Returns the audio frame rate.
  ///
  /// @return Audio frame rate in hz.
  [[nodiscard]] int GetFrameRate() const noexcept;

  /// Loads Wave file with the given `file_path`.
  ///
  /// @param file_path Absolute file path.
  /// @return True if successful.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  bool Load(const std::string& file_path) noexcept;

 private:
  // Number of audio channels.
  int channel_count_ = 0;

  // Audio frame rate in hz.
  int frame_rate_ = 0;

  // Audio data.
  std::vector<double> data_;
};

}  // namespace barely::examples

#endif  // EXAMPLES_COMMON_WAV_FILE_H_
