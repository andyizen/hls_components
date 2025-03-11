#include "process_frame.h"
#include <iomanip>
#include <iostream>
#include <math.h> // for pow()

int main() {
  // Create the input stream of samples (each sample is one channel, 32 bits)
  hls::stream<sample_t> in_stream;
  // Create the output stream of frames (each frame packs NUM_CHANNELS samples,
  // i.e., 512 bits)
  hls::stream<frame_t> out_stream;

  const int num_frames = 3;
  // We'll store test data per frame per channel.
  sample_t test_data[num_frames][NUM_CHANNELS];

  // Generate test data using the provided loop:
  // For each frame i and for each channel j, compute:
  // channel_value = pow(j, (i + 1))
  for (int i = 0; i < num_frames; i++) {
    for (int j = 0; j < NUM_CHANNELS; j++) {
      test_data[i][j] = (sample_t)pow(j, (i + 1));
    }
  }

  // Process each frame.
  for (int i = 0; i < num_frames; i++) {
    // Write one frame's worth of samples into the input stream.
    for (int j = 0; j < NUM_CHANNELS; j++) {
      in_stream.write(test_data[i][j]);
    }

    // Call process_frame to read NUM_CHANNELS samples from in_stream,
    // process them, and pack them into one frame (512 bits) in out_stream.
    process_frame(in_stream, out_stream);

    // Read the packed frame from the output stream.
    frame_t out_frame = out_stream.read();

    // Unpack the frame into individual channels.
    sample_t processed_channels[NUM_CHANNELS];
    for (int j = 0; j < NUM_CHANNELS; j++) {
      // Extract channel j (MSB-first packing):
      // Channel 0 is in the most-significant bits.
      processed_channels[j] =
          out_frame >> ((NUM_CHANNELS - 1 - j) * CHANNEL_SIZE);
    }

    // Compare each channel's processed value to the expected value.
    int fail_count = 0;
    std::cout << "Frame " << i << ":\n";
    for (int j = 0; j < NUM_CHANNELS; j++) {
      sample_t expected = test_data[i][j] + 1; // Dummy processing adds 1.
      std::cout << "  CH" << std::setw(2) << j << ": " << std::dec
                << processed_channels[j] << " (expected: " << expected << ")";
      if (processed_channels[j] == expected) {
        std::cout << " PASSED." << std::endl;
      } else {
        std::cout << " FAILED." << std::endl;
        fail_count++;
      }
    }
    if (fail_count == 0)
      std::cout << "Frame " << i << " PASSED." << std::endl;
    else
      std::cout << "Frame " << i << " FAILED with " << fail_count << " errors."
                << std::endl;
  }

  return 0;
}
