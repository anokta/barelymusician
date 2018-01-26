#include "barelymusician/instrument/envelope.h"

#include <iostream>

#include "test_utils.h"

// TODO(anokta): Refactor the tests to have their own classes with a template.
using barelyapi::Envelope;

const int kSampleRate = 100;
const float kEpsilon = 1e-5f;

void TestEnvelope() {
  bool pass = true;
  Envelope envelope(1.0f / static_cast<float>(kSampleRate));
  envelope.SetAttack(0.1f);
  envelope.SetSustain(0.5f);
  envelope.SetDecay(0.1f);
  envelope.Start();
  for (int i = 0; i < 30; ++i) {
    const float expected =
        i < 10 ? 0.1f * i : (i < 20 ? 1.0f - 0.05f * (i - 10) : 0.5f);
    const float actual = envelope.Next();
    const bool sample_pass = ExpectNear(expected, actual, kEpsilon);
    pass &= sample_pass;
    if (!sample_pass) {
      std::cout << i << ": " << expected << " vs " << actual << std::endl;
    }
  }
  std::cout << (pass ? "PASSED" : "FAILED") << std::endl;
}

int main() {
  TestEnvelope();

  system("pause");
  return 0;
}
