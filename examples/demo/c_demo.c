// NOLINTBEGIN
#include <assert.h>
#include <barelymusician.h>
#include <stdio.h>

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#ifdef _WIN32
#include <windows.h>
#define BarelySleep(seconds) Sleep((DWORD)(seconds * 1000.0))
#else  // #ifdef _WIN32
#include <time.h>
#define BarelySleep(seconds)                                                                \
  do {                                                                                      \
    struct timespec ts = {.tv_sec = (time_t)(seconds),                                      \
                          .tv_nsec = (long)(((seconds) - (double)seconds) * 1000000000.0)}; \
    nanosleep(&ts, NULL);                                                                   \
  } while (0)
#endif  // #ifdef _WIN32

// System audio settings.
static const int kSampleRate = 48000;
static const int kChannelCount = 2;
static const int kFrameCount = 256;

static const double kUpdateInterval = 1.0 / 60.0;
static const double kLookahead = 0.05;

// Instrument settings.
static const float kGain = 0.9f;
static const float kOscShape = 0.75f;
static const float kAttack = 0.005f;
static const float kRelease = 0.2f;

// Playback settings.
static const double kTempo = 99.0;
static const int kLoopCount = 2;

enum {
  kMelodyNoteCount = 7,
};
static const float kMelodyPitches[kMelodyNoteCount] = {0.0f, 0.25f, 0.5f, 1.0f, 0.5f, 0.25f, 0.0f};
static const double kMelodyPositions[kMelodyNoteCount + 1] = {
    0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 8.0,
};

static int g_loop_count = 0;
static uint32_t g_instrument_id = 0;
static volatile double g_timestamp = 0.0;

static void AudioProcessCallback(ma_device* device, void* output, const void* input,
                                 ma_uint32 frame_count) {
  (void)input;  // unused
  assert(device != NULL);
  assert(device->pUserData != NULL);
  BarelyEngine_Process((BarelyEngine*)device->pUserData, (float*)output,
                       (int32_t)device->playback.channels, (int32_t)frame_count, g_timestamp);
  g_timestamp += (double)frame_count / (double)kSampleRate;
}

static void NoteOffCallback(float pitch, void* user_data) {
  (void)user_data;  // unused
  printf("NoteOff(%.1f)\n", pitch);
}

static void NoteOnCallback(float pitch, void* user_data) {
  (void)user_data;  // unused
  printf("NoteOn(%.1f)\n", pitch);
}

static void TriggerCallback(void* user_data) {
  if (g_loop_count++ < kLoopCount) {
    for (int i = 0; i < kMelodyNoteCount; ++i) {
      uint32_t event_id = 0;
      BarelyInstrument_ScheduleNote((BarelyEngine*)user_data, g_instrument_id, kMelodyPitches[i],
                                    kMelodyPositions[i + 1] - kMelodyPositions[i],
                                    kMelodyPositions[i], &event_id);
    }
  }
}

int main() {
  // Initialize the engine.
  BarelyEngine* engine = NULL;
  BarelyEngineConfig config = BARELY_ENGINE_CONFIG_DEFAULT(kSampleRate);
  config.max_frame_count = kFrameCount;
  BarelyEngine_Create(&config, &engine);
  BarelyEngine_SetTempo(engine, kTempo);

  BarelyEngine_CreateInstrument(engine, &g_instrument_id);
  BarelyInstrument_SetControl(engine, g_instrument_id, BarelyInstrumentControlType_kGain, kGain);
  BarelyInstrument_SetControl(engine, g_instrument_id, BarelyInstrumentControlType_kOscMix, 1.0f);
  BarelyInstrument_SetControl(engine, g_instrument_id, BarelyInstrumentControlType_kOscShape,
                              kOscShape);
  BarelyInstrument_SetControl(engine, g_instrument_id, BarelyInstrumentControlType_kAttack,
                              kAttack);
  BarelyInstrument_SetControl(engine, g_instrument_id, BarelyInstrumentControlType_kRelease,
                              kRelease);

  BarelyInstrument_SetNoteOffCallback(engine, g_instrument_id, NoteOffCallback, NULL);
  BarelyInstrument_SetNoteOnCallback(engine, g_instrument_id, NoteOnCallback, NULL);

  uint32_t event_id = 0;
  BarelyEngine_ScheduleTrigger(engine, TriggerCallback, engine, kMelodyPositions[kMelodyNoteCount],
                               kLookahead, &event_id);

  // Initialize the audio device.
  ma_device device;
  ma_device_config device_config = ma_device_config_init(ma_device_type_playback);
  device_config.playback.format = ma_format_f32;
  device_config.playback.channels = (ma_uint32)kChannelCount;
  device_config.periodSizeInFrames = (ma_uint32)kFrameCount;
  device_config.sampleRate = (ma_uint32)kSampleRate;
  device_config.pUserData = engine;
  device_config.dataCallback = AudioProcessCallback;

  const ma_result result = ma_device_init(NULL, &device_config, &device);
  if (result != MA_SUCCESS) {
    printf("Failed to initialize audio device\n");
    return result;
  }
  ma_device_start(&device);

  printf("Playback started\n");

  while (g_loop_count <= kLoopCount) {
    BarelyEngine_Update(engine, g_timestamp + kLookahead);
    BarelySleep(kUpdateInterval);
  }

  printf("Playback stopped\n");

  // Shutdown the audio device.
  ma_device_stop(&device);
  ma_device_uninit(&device);

  // Shutdown the engine.
  BarelyEngine_CancelEvent(engine, event_id);
  BarelyEngine_DestroyInstrument(engine, g_instrument_id);
  BarelyEngine_Destroy(engine);

  return 0;
}
// NOLINTEND
