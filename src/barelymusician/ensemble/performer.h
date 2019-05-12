#ifndef BARELYMUSICIAN_ENSEMBLE_PERFORMER_H_
#define BARELYMUSICIAN_ENSEMBLE_PERFORMER_H_

#include <list>

#include "barelymusician/base/logging.h"
#include "barelymusician/base/module.h"
#include "barelymusician/composition/beat_composer.h"
#include "barelymusician/composition/note.h"
#include "barelymusician/instrument/instrument.h"
#include "barelymusician/message/message_utils.h"
#include "barelymusician/sequencer/transport.h"

namespace barelyapi {

namespace {

// Unique message IDs per message type.
const int kNoteOnId = 0;
const int kNoteOffId = 1;

// |NoteOn| message data.
struct NoteOnData {
  float index;
  float intensity;
};

// |NoteOff| message data.
struct NoteOffData {
  float index;
};

}  // namespace

class Performer : public Module {
 public:
  Performer(Instrument* instrument, BeatComposer* composer);

  // Implements |Module|.
  void Reset() override;

  void PerformBeat(const Transport& transport, int section_type, int harmonic,
                   int sample_offset, int num_samples_per_beat);

  void Process(float* output, int num_channels, int num_frames);

 private:
  void PlayNote(float index, float intensity, int start_sample,
                int duration_samples);

  void ProcessMessage(const Message& message);

  void PushMessage(const Message& message);

  Instrument* const instrument_;  // not owned.

  BeatComposer* const composer_;  // not owned.

  // Note messages.
  std::list<Message> messages_;
};

Performer::Performer(Instrument* instrument, BeatComposer* composer)
    : instrument_(instrument), composer_(composer) {
  DCHECK(instrument_);
  DCHECK(composer_);
}

void Performer::Reset() {
  composer_->Reset();
  messages_.clear();
  instrument_->Reset();
}

void Performer::PerformBeat(const Transport& transport, int section_type,
                            int harmonic, int sample_offset,
                            int num_samples_per_beat) {
  const float num_samples_per_beat_float =
      static_cast<float>(num_samples_per_beat);
  const auto notes = composer_->GetNotes(transport, section_type, harmonic);
  for (const auto& note : notes) {
    const float index = note.index;
    const float intensity = note.intensity;
    const int start_sample =
        sample_offset +
        static_cast<int>(note.start_beat * num_samples_per_beat_float);
    const int duration_samples =
        static_cast<int>(note.duration_beats * num_samples_per_beat_float);
    PlayNote(index, intensity, start_sample, duration_samples);
  }
}

void Performer::Process(float* output, int num_channels, int num_frames) {
  // Process frames within message events range.
  int frame = 0;
  const auto begin = messages_.begin();
  const auto end =
      std::lower_bound(begin, messages_.end(), num_frames, &CompareTimestamp);
  if (begin != end) {
    for (auto it = begin; it != end; ++it) {
      const int num_frames_to_process = it->timestamp - frame;
      if (num_frames_to_process > 0) {
        instrument_->Process(&output[frame * num_channels], num_channels,
                             num_frames_to_process);
        frame += num_frames_to_process;
      }
      ProcessMessage(*it);
    }
    messages_.erase(begin, end);
  }
  // Process remaining frames.
  instrument_->Process(&output[frame * num_channels], num_channels,
                       num_frames - frame);

  // Update message timestamps.
  for (auto& message : messages_) {
    message.timestamp -= num_frames;
  }
}

void Performer::PlayNote(float index, float intensity, int start_sample,
                         int duration_samples) {
  PushMessage(
      BuildMessage<NoteOnData>(kNoteOnId, {index, intensity}, start_sample));
  PushMessage(BuildMessage<NoteOffData>(kNoteOffId, {index},
                                        start_sample + duration_samples));
}

void Performer::ProcessMessage(const Message& message) {
  switch (message.id) {
    case kNoteOnId: {
      const auto note_on = ReadMessageData<NoteOnData>(message.data);
      instrument_->NoteOn(note_on.index, note_on.intensity);
    } break;
    case kNoteOffId: {
      const auto note_off = ReadMessageData<NoteOffData>(message.data);
      instrument_->NoteOff(note_off.index);
    } break;
    default:
      DLOG(ERROR) << "Unknown message ID: " << message.id;
      break;
  }
}

void Performer::PushMessage(const Message& message) {
  DCHECK_GE(message.timestamp, 0);
  const auto it = std::upper_bound(messages_.begin(), messages_.end(), message,
                                   &CompareMessage);
  messages_.insert(it, message);
}

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENSEMBLE_PERFORMER_H_
