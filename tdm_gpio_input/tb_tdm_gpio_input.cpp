#include "tdm_gpio_input.h"
#include <cmath>
#include <hls_stream.h>
#include <ios>
#include <iostream>

// For this testbench, assume the testbench clock runs 4x faster than sclk.
int main() {

  hls::stream<bit_t> out_stream;
  int bit_count = 0;
  int frame_index = 0;
  int frame_size = FRAME_SIZE;
  int bit_index = 0;
  bool data = false;
  const int cycles_per_bit = 4; // Testbench clock is 4x faster than sclk

  // Test parameters.
  const int num_frames = 3; // Number of frames to simulate
  const int transmit_cycles = (FRAME_SIZE * num_frames * cycles_per_bit) +
                              (frame_size / 2 * cycles_per_bit);

  // Test frames: example patterns.
  ap_uint<FRAME_SIZE> test_frames[num_frames];

  for (int i = 0; i < num_frames; i++) {
    for (int j = 0; j < NUM_CHANNELS; j++) {
      ap_uint<CHANNEL_SIZE> channel_value = pow(j, (i + 1));
      test_frames[i] = (test_frames[i] << CHANNEL_SIZE) | channel_value;
    }
  }

  // Simulated external signals (initial values)
  bit_t sdata_sim = 0;
  bit_t sclk_sim = 0;
  bit_t lrclk_sim = 0;

  // Continuous simulation loop: each iteration is one testbench clock cycle.
  for (int cycle = 0; cycle < transmit_cycles + frame_size; cycle++) {
    switch (cycle % cycles_per_bit) {
    case 0:
      sclk_sim = 0;
      break;
    case 1:
      bit_index = bit_count % frame_size;
      data = (test_frames[frame_index] >> ((frame_size - 1) - bit_index)) & 1;
      sdata_sim = data;

      if (lrclk_sim == 1)
        lrclk_sim = 0;
      else if ((bit_count % frame_size) == (frame_size - 1)) {
        frame_index++;
        lrclk_sim = 1;
      }
      break;
    case 2:
      sclk_sim = 1;
      bit_count++;
      break;
    }

    // End of transmisson of test data, but we want to continue the clk for a
    // while to see if the downstream transmission works correctly
    if (bit_count >= (frame_size * num_frames) + 1)
      sdata_sim = 0;

    // Call the tdm_gpio_input function once per testbench cycle.
    tdm_gpio_input(&sdata_sim, &sclk_sim, &lrclk_sim, out_stream);
  }

  // After simulation, reconstruct frames from the output stream.
  int fail_count = 0;
  std::cout << "Received " << out_stream.size() << " bits in out_stream."
            << std::endl;

  for (int i = 0; i < num_frames; i++) {
    for (int j = 0; j < NUM_CHANNELS; j++) {
      ap_uint<CHANNEL_SIZE> channel = 0;
      for (int k = 0; k < CHANNEL_SIZE; k++) {
        channel = (channel << 1) | out_stream.read();
      }

      int shift_index = (NUM_CHANNELS - 1 - j) * CHANNEL_SIZE;
      ap_uint<CHANNEL_SIZE> test_ch =
          test_frames[i] >> shift_index & 0xFFFFFFFF;

      std::cout << "Frame " << i << " | CH" << j << ":" << std::dec << channel;
      if (channel == test_ch)
        std::cout << " PASSED." << std::endl;
      else {
        std::cout << " FAILED." << std::endl;
        fail_count++;
      }
    }
  }

  // Return fail count, since every return value except 0 leads to a failed test
  return fail_count;
}
