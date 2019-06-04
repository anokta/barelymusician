#ifndef EXAMPLES_UTIL_WAV_FILE_H_
#define EXAMPLES_UTIL_WAV_FILE_H_

#include <string>
#include <vector>

namespace barelyapi {
namespace examples {

// RIFF Wave file.
class WavFile {
 public:
  // Returns the audio data.
  //
  // @return Audio data.
  const std::vector<float>& GetData() const;

  // Returns the number of audio channels.
  //
  // @return Number of audio channels.
  int GetNumChannels() const;

  // Returns the audio sampling rate.
  //
  // @return Audio sampling rate.
  int GetSampleRate() const;

  // Loads Wave file with the given |file_path|.
  //
  // @param file_path Absolute file path.
  // @return True if successful.
  bool Load(const std::string& file_path);

 private:
  // Number of audio channels.
  int num_channels_;

  // Audio sampling rate.
  int sample_rate_;

  // Audio data.
  std::vector<float> data_;
};

}  // namespace examples
}  // namespace barelyapi

#endif  // EXAMPLES_UTIL_WAV_FILE_H_