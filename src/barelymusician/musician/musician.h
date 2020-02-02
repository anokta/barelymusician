#ifndef BARELYMUSICIAN_MUSICIAN_MUSICIAN_H_
#define BARELYMUSICIAN_MUSICIAN_MUSICIAN_H_

#include <algorithm>
#include <cmath>
#include <functional>
#include <utility>
#include <vector>

#include "barelymusician/base/logging.h"
#include "barelymusician/engine/clock.h"
#include "barelymusician/instrument/instrument.h"
#include "barelymusician/message/message_buffer.h"
#include "barelymusician/message/message_data.h"
#include "barelymusician/musician/note.h"

namespace barelyapi {

class Musician {
 public:
  // Musical ensemble.
  struct Ensemble {
    // Bar composer callback signature.
    using BarComposerCallback = std::function<int(int bar, int num_beats)>;

    // Beat composer callback signature.
    using BeatComposerCallback =
        std::function<void(int bar, int beat, int num_beats, int harmonic,
                           std::vector<Note>* notes)>;

    // Bar composer callback.
    BarComposerCallback bar_composer_callback;

    struct Performer {
      Performer(Instrument* instrument,
                BeatComposerCallback beat_composer_callback)
          : instrument(instrument),
            beat_composer_callback(beat_composer_callback) {}

      Instrument* instrument;
      BeatComposerCallback beat_composer_callback;
      MessageBuffer score;
    };

    // List of performers.
    std::vector<Performer> performers;
  };

  explicit Musician(int sample_rate)
      : clock_(sample_rate), position_(0.0), last_position_(0.0) {}

  void SetNumBeats(int num_beats) { num_beats_ = num_beats; }

  void SetTempo(double tempo) { clock_.SetTempo(tempo); }

  void Update(int num_samples) {
    last_position_ = position_;
    clock_.UpdatePosition(num_samples);
    position_ = clock_.GetPosition();

    for (double beat = std::ceil(last_position_); beat < position_; ++beat) {
      ProcessBeat(static_cast<int>(beat));
    }
  }

  void Process(float* output, int num_channels, int num_frames,
               Ensemble::Performer* performer) {
    int frame = 0;
    // Process notes.
    const auto& messages =
        performer->score.GetIterator(last_position_, position_);
    const double frames_per_beat =
        static_cast<double>(num_frames) / (position_ - last_position_);

    for (auto it = messages.cbegin; it != messages.cend; ++it) {
      const int message_frame =
          static_cast<int>(frames_per_beat * (it->position - last_position_));
      if (frame < message_frame) {
        performer->instrument->Process(&output[num_channels * frame],
                                       num_channels, message_frame - frame);
        frame = message_frame;
      }
      std::visit(MessageProcessor{performer->instrument}, it->data);
    }
    performer->score.Clear(messages);
    // Process the rest of the buffer.
    if (frame < num_frames) {
      performer->instrument->Process(&output[num_channels * frame],
                                     num_channels, num_frames - frame);
    }
  }

  Ensemble& ensemble() { return ensemble_; }
  const Ensemble& ensemble() const { return ensemble_; }

 private:
  // Instrument message processor.
  struct MessageProcessor {
    // Processes |NoteOffData|.
    void operator()(const NoteOffData& note_off_data) {
      instrument->NoteOff(note_off_data.index);
    }

    // Processes |NoteOnData|.
    void operator()(const NoteOnData& note_on_data) {
      instrument->NoteOn(note_on_data.index, note_on_data.intensity);
    }

    // Instrument to process.
    Instrument* instrument;
  };

  void ProcessBeat(int beat) {
    // Update transport.
    bar_ = beat / num_beats_;
    beat_ = beat % num_beats_;

    if (beat_ == 0) {
      // Compose next bar.
      harmonic_ = ensemble_.bar_composer_callback(bar_, num_beats_);
    }
    // Update performers.
    for (Ensemble::Performer& performer : ensemble_.performers) {
      // Compose next beat notes.
      temp_notes_.clear();
      performer.beat_composer_callback(bar_, beat_, num_beats_, harmonic_,
                                       &temp_notes_);
      for (const Note& note : temp_notes_) {
        const double position = static_cast<double>(beat) + note.offset_beats;
        performer.score.Push(
            {NoteOnData{note.index, note.intensity}, position});
        performer.score.Push(
            {NoteOffData{note.index}, position + note.duration_beats});
      }
    }
  }

  Clock clock_;

  Ensemble ensemble_;

  int harmonic_;

  // Current bar.
  int bar_;

  // Current beat.
  int beat_;

  // Number of beats per bar.
  int num_beats_;

  // Current clock position.
  double position_;

  // Last clock position.
  double last_position_;

  // TODO: not necessary, use Score instead?
  std::vector<Note> temp_notes_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_MUSICIAN_MUSICIAN_H_
