#ifndef BARELYMUSICIAN_MUSICIAN_MUSICIAN_H_
#define BARELYMUSICIAN_MUSICIAN_MUSICIAN_H_

#include <algorithm>
#include <cmath>
#include <functional>
#include <utility>
#include <vector>

#include "barelymusician/base/clock.h"
#include "barelymusician/base/logging.h"
#include "barelymusician/dsp/dsp_utils.h"
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
      // TODO: Figure out a proper way to communicate this to instruments.
      MessageBuffer::Iterator messages;
    };

    // List of performers.
    std::vector<Performer> performers;
  };

  explicit Musician(int sample_rate) : clock_(sample_rate) {}

  void SetNumBeats(int num_beats) { num_beats_ = num_beats; }

  void SetTempo(double tempo) { clock_.SetTempo(tempo); }

  void Update(int num_samples) {
    const double start_position = clock_.GetPosition();
    clock_.UpdatePosition(num_samples);
    const double end_position = clock_.GetPosition();

    for (double beat = std::ceil(start_position); beat < end_position; ++beat) {
      ProcessBeat(static_cast<int>(beat));
    }

    for (Ensemble::Performer& performer : ensemble_.performers) {
      performer.messages =
          performer.score.GetIterator(start_position, end_position);
    }
  }

  void Process(float* output, int num_channels, int num_frames,
               Ensemble::Performer* performer) {
    int frame = 0;
    // Process notes.
    const int num_samples_per_beat = clock_.GetNumSamplesPerBeat();
    const auto& messages = performer->messages;
    const int offset_samples =
        SamplesFromBeats(messages.timestamp, num_samples_per_beat);
    for (auto it = messages.cbegin; it != messages.cend; ++it) {
      const int message_frame =
          SamplesFromBeats(it->timestamp, num_samples_per_beat) -
          offset_samples;
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
        const double timestamp = static_cast<double>(beat) + note.offset_beats;
        performer.score.Push(
            {NoteOnData{note.index, note.intensity}, timestamp});
        performer.score.Push(
            {NoteOffData{note.index}, timestamp + note.duration_beats});
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

  // TODO: not necessary, use Score instead?
  std::vector<Note> temp_notes_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_MUSICIAN_MUSICIAN_H_
