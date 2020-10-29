#include "examples/util/wav_file.h"

#include "barelymusician/base/logging.h"

#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"

namespace barelyapi {
namespace examples {

const std::vector<float>& WavFile::GetData() const { return data_; }

int WavFile::GetNumChannels() const { return num_channels_; }

int WavFile::GetSampleRate() const { return sample_rate_; }

bool WavFile::Load(const std::string& file_path) {
  // Read the file.
  unsigned int wav_num_channels;
  unsigned int wav_sample_rate;
  drwav_uint64 wav_num_frames;
  float* wav_data = drwav_open_file_and_read_pcm_frames_f32(
      file_path.c_str(), &wav_num_channels, &wav_sample_rate, &wav_num_frames);
  if (!wav_data) {
    LOG(ERROR) << "Failed to read WAV file: " << file_path;
    return false;
  }

  // Copy the contents.
  num_channels_ = static_cast<int>(wav_num_channels);
  sample_rate_ = static_cast<int>(wav_sample_rate);
  data_.resize(wav_num_channels * wav_num_frames);
  std::copy_n(wav_data, data_.size(), data_.begin());

  // Free the original file.
  drwav_free(wav_data);

  return true;
}

}  // namespace examples
}  // namespace barelyapi
