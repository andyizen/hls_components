#include "tdm_gpio_input.h"
#include <cmath>
#include <hls_stream.h>
#include <ios>
#include <iostream>

// For this testbench, assume the testbench clock runs 4x faster than sclk.
int main() {

  hls::stream<sample_t> out_stream;
  int bit_count = 0;
  int frame_index = 0;
  int frame_size = FRAME_SIZE;
  const int PRE_CHARGES = 10;
  bool data = false;
  const int mclks_per_bit = 8; // Testbench clock is 4x faster than sclk

  // Test parameters.
  const int num_frames = 3; // Number of frames to simulate
  const int transmit_cycles = FRAME_SIZE * num_frames + FRAME_SIZE / 2;

  // Test frames: example patterns.
  frame_t test_frames[num_frames];
  for (int i = 0; i < num_frames; i++) {
    for (int j = 0; j < NUM_CHANNELS; j++) {
      sample_t channel = pow(j, (i + 1));
      test_frames[i] = (test_frames[i] << CHANNEL_SIZE) | channel;
    }
  }

  // Simulated external signals (initial values)
  bit_t sdata_sim = 0;
  bit_t sclk_sim = 0;
  bit_t lrclk_sim = 0;

  // Simulate offset start
  for (int pre_clks = 0; pre_clks < PRE_CHARGES; pre_clks++) {
    if (pre_clks == PRE_CHARGES - 1) {
      lrclk_sim = 1;
    } else {
      lrclk_sim = 0;
    }
    for (int cycle = 0; cycle < mclks_per_bit; cycle++) {
      if (cycle < (mclks_per_bit / 2 - 1)) {
        sclk_sim = 0;
      } else if (cycle >= mclks_per_bit / 2) {
        sclk_sim = 1;
      }
      tdm_gpio_input(&sdata_sim, &sclk_sim, &lrclk_sim, out_stream);
    }
  }

  for (int sclks = 0; sclks < transmit_cycles; sclks++) {
    if (sclks % FRAME_SIZE == FRAME_SIZE - 1) {
      lrclk_sim = 1;
    } else {
      lrclk_sim = 0;
    }

    int shift_index = FRAME_SIZE - 1 - (sclks % FRAME_SIZE);
    sdata_sim = (test_frames[frame_index] >> shift_index) & 1;

    for (int mclk = 0; mclk < mclks_per_bit; mclk++) {
      if (mclk < (mclks_per_bit / 2 - 1)) {
        sclk_sim = 0;
      } else if (mclk >= mclks_per_bit / 2) {
        sclk_sim = 1;
      }
      tdm_gpio_input(&sdata_sim, &sclk_sim, &lrclk_sim, out_stream);
    }
    if (bit_count == FRAME_SIZE - 1) {
      bit_count = 0;
      frame_index++;
    } else {
      bit_count++;
    }
  }

  // After simulation, reconstruct frames from the output stream.
  int fail_count = 0;
  std::cout << "Received " << out_stream.size() << " samples in out_stream."
            << std::endl;

  for (int i = 0; i < num_frames; i++) {
    for (int j = 0; j < NUM_CHANNELS; j++) {
      if (!out_stream.empty()) {
        sample_t channel = out_stream.read();

        int shift_index = (NUM_CHANNELS - 1 - j) * CHANNEL_SIZE;
        sample_t test_ch = test_frames[i] >> shift_index & MASK;

        std::cout << "Frame " << i << " | CH" << j << ":" << std::dec
                  << channel;
        if (channel == test_ch)
          std::cout << " PASSED." << std::endl;
        else {
          std::cout << " FAILED." << channel << " | " << test_ch << std::endl;
          fail_count++;
        }
      }
    }
  }

  // Return fail count, since every return value except 0 leads to a failed test
  return fail_count;
}
