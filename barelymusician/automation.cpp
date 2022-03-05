#ifndef BARELYMUSICIAN_AUTOMATION_H_
#define BARELYMUSICIAN_AUTOMATION_H_

#include <deque>
#include <iostream>
#include <vector>

struct AutomationPoint {
  double timestamp;

  float value;
};

struct AutomationSegment {
  // float begin_value;

  // float end_value;

  float value;

  // float slope;
  float increment;

  double timestamp;
};

// float Lerp(const AutomationSegment& segment, float t) {
//   return segment.begin_value + t * (segment.end_value - segment.begin_value);
// }

void Process(float* output, int num_frames, const AutomationSegment* segment) {
  int frame = 0;

  float value = segment->value;
  while (frame < num_frames) {
    output[frame++] = value;
    value += segment->increment;
  }
}

int main() {
  // float value = 4.0f;

  std::vector<AutomationPoint> points;
  // points.push_back({0.0, 4.0f});
  points.push_back({1.0, 5.0f});
  points.push_back({1.5, 6.0f});
  points.push_back({2.0, 10.0f});
  points.push_back({4.0, 11.0f});
  points.push_back({5.0, 11.0f});
  points.push_back({5.0, 15.0f});
  points.push_back({6.0, 20.0f});
  points.push_back({11.0, 10.0f});

  const int kNumFrames = 10;
  std::vector<float> buffer;
  buffer.resize(kNumFrames, 0.0f);

  std::deque<AutomationSegment> segments;
  // segments.push_back({value, 0.0f, 0.0});
  AutomationSegment current_segment{4.0f, 0.0f, 0.0};

  int total_frame = 0;
  const auto process = [&](float* output, int num_frames,
                           const AutomationSegment& segment) {
    Process(output, num_frames, &segment);

    for (auto frame = 0; frame < num_frames; ++frame) {
      std::cout << "\tFrame " << total_frame++ << ":\t" << output[frame]
                << std::endl;
    }
  };

  int index = 0;
  for (double ts = 0.0; ts < 15.0; ++ts) {
    std::cout << std::endl << "Timestamp = " << ts << std::endl;

    int frame = 0;

    while (index < static_cast<int>(points.size())) {
      auto point = points[index++];
      std::cout << "Current "
                << " = " << current_segment.timestamp << ", "
                << current_segment.value << std::endl;
      std::cout << "Point " << index << " = " << point.timestamp << ", "
                << point.value << std::endl;
      double x = point.timestamp - current_segment.timestamp;
      if (x > 0.0) {
        float y = point.value - current_segment.value;
        double slope = static_cast<double>(y) / x;
        std::cout << "Slope = " << slope << std::endl;
        current_segment.increment =
            static_cast<float>(slope / static_cast<double>(kNumFrames));
        const double target_timestamp = std::min(point.timestamp, ts + 1.0);
        const int target_frame = static_cast<int>(
            (target_timestamp - ts) * static_cast<double>(kNumFrames));
        if (frame < target_frame) {
          process(&buffer[frame], target_frame - frame, current_segment);
          frame = target_frame;
        }
        current_segment.timestamp = target_timestamp;
        if (point.timestamp >= ts + 1.0) {
          std::cout << "Begin value = " << current_segment.value << std::endl;
          current_segment.value +=
              y * static_cast<float>(1.0 / (point.timestamp - ts));
          std::cout << "End value = " << current_segment.value << std::endl;
          --index;
          break;
        }
        current_segment.value = point.value;
      } else {
        const int target_frame = static_cast<int>(
            (current_segment.timestamp - ts) * static_cast<double>(kNumFrames));
        if (frame < target_frame) {
          process(&buffer[frame], target_frame - frame, current_segment);
          frame = target_frame;
        }
        current_segment.value = point.value;
        current_segment.increment = 0;
      }
    }

    if (frame < kNumFrames) {
      process(&buffer[frame], kNumFrames - frame, current_segment);
    }
  }

  return 0;
}

#endif  // BARELYMUSICIAN_AUTOMATION_H_
