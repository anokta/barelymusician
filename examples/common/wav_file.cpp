#include "common/wav_file.h"

#include <algorithm>
#include <string>
#include <vector>

#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"

namespace barely::examples {

bool WavFile::Load(const std::string& file_path) {
  // Read the file.
  unsigned int wav_channel_count;
  unsigned int wav_sample_rate;
  drwav_uint64 wav_frame_count;
  float* wav_data = drwav_open_file_and_read_pcm_frames_f32(
      file_path.c_str(), &wav_channel_count, &wav_sample_rate, &wav_frame_count, nullptr);
  if (!wav_data) {
    return false;
  }

  // Copy the contents.
  channel_count_ = static_cast<int>(wav_channel_count);
  sample_rate_ = static_cast<int>(wav_sample_rate);
  data_.resize(wav_channel_count * wav_frame_count);
  std::copy_n(wav_data, data_.size(), data_.begin());

  // Free the original file.
  drwav_free(wav_data, nullptr);

  return true;
}

}  // namespace barely::examples
