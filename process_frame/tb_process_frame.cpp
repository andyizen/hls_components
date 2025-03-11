#include "process_frame.h"
#include <iostream>
#include <math.h> // for pow()

// For printing large values.
#include <iomanip>

int main() {
  hls::stream<sample_t> in_stream;
  hls::stream<sample_t> out_stream;

  const int num_frames = 3;
  // We'll create an array to store each test frame (512 bits per frame).
  // Use ap_uint<512> since 16 * 32 = 512.
  ap_uint<CHANNEL_SIZE> test_frames[num_frames][NUM_CHANNELS];

  // Initialize test frames to 0.

  for (int i = 0; i < num_frames; i++) {
    for (int j = 0; j < NUM_CHANNELS; j++) {
      test_frames[i][j] = 0;
    }
  }

  // Create test frames
  for (int i = 0; i < num_frames; i++) {
    for (int j = 0; j < NUM_CHANNELS; j++) {
      ap_uint<CHANNEL_SIZE> channel_value =
          (ap_uint<CHANNEL_SIZE>)pow(j, (i + 1));
      test_frames[i][j] = channel_value;
      in_stream.write(channel_value);
    }
    // Process the in_stream
    process_frame(in_stream, out_stream);

    // Read processed frame
    // Compare frames
    int fail_count = 0;
    std::cout << "Received " << out_stream.size() << " bits in out_stream."
              << std::endl;
    for (int j = 0; j < NUM_CHANNELS; j++) {
      sample_t sample = out_stream.read();

      std::cout << "Frame " << i << " | CH" << j << ":" << std::dec << sample;
      if (sample - 1 == test_frames[i][j])
        std::cout << " PASSED." << std::endl;
      else {
        std::cout << " FAILED: " << test_frames[i][j] << std::endl;
        fail_count++;
      }
    }
  }

  return 0;
}
