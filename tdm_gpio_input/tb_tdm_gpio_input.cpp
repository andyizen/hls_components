#include "tdm_gpio_input.h"
#include <cmath>
#include <hls_stream.h>
#include <iostream>

// Helper function: run through mclks_per_bit cycles.
// The lambda parameter 'custom_action' is called each mclk cycle so that you
// can update sdata_sim, lrclk_sim, etc. as needed.

void simulate_mclk(int mclks_per_bit, bit_t &sdata_sim, bit_t &sclk_sim,
                   bit_t &lrclk_sim, bit_t &frame_rdy, bit_t &sample_rdy,
                   ap_uint<4> sample_cout, hls::stream<sample_t> &our_reg) {
  for (int mclk = 0; mclk < mclks_per_bit; mclk++) {
    // Set sclk: For example, assume sclk is low for the first half minus one
    // cycle, and high for the remaining cycles.
    if (mclk < (mclks_per_bit / 2))
      sclk_sim = 0;
    else
      sclk_sim = 1;

    // Call the tdm_gpio_input component for this clock cycle.
    tdm_gpio_input(sdata_sim, sclk_sim, lrclk_sim, frame_rdy, sample_rdy,
                   sample_cout, our_reg);
  }
}

int main() {

  hls::stream<sample_t> our_reg;
  int bit_count = 0;
  int frame_index = 0;

  // frame_size is defined by FRAME_SIZE (e.g. 512 for 16x32-bit channels)
  const int frame_size = FRAME_SIZE;
  const int PRE_CHARGES = 10;
  const int mclks_per_bit = 8;

  // Test parameters.
  const int num_frames = 3; // Number of frames to simulate
  // transmit_cycles can be defined as total bits to send.
  const int transmit_cycles = frame_size * num_frames + frame_size / 2;

  // Test frames: example patterns.
  // Each frame is built by packing NUM_CHANNELS channels (each CHANNEL_SIZE
  // bits).
  frame_t test_frames[num_frames] = {0};
  for (int i = 0; i < num_frames; i++) {
    for (int j = 0; j < NUM_CHANNELS; j++) {
      sample_t channel = j + 1;
      test_frames[i] = (test_frames[i] << CHANNEL_SIZE) | channel;
    }
  }

  // Simulated external signals (initial values)
  bit_t sdata_sim = 0;
  bit_t sclk_sim = 0;
  bit_t lrclk_sim = 0;
  bit_t frame_rdy = 0;
  bit_t sample_rdy = 0;
  ap_uint<4> sample_counter = 0;

  // ---- Offset Start Phase ----
  // Use simulate_mclk() to run a fixed number of cycles to pre-charge the
  // design.
  for (int pre_clks = 0; pre_clks < PRE_CHARGES; pre_clks++) {
    // For each precharge cycle, set lrclk_sim:
    if (pre_clks == PRE_CHARGES - 1)
      lrclk_sim = 1;
    else
      lrclk_sim = 0;

    simulate_mclk(mclks_per_bit, sdata_sim, sclk_sim, lrclk_sim, frame_rdy,
                  sample_rdy, sample_counter, our_reg);
  }

  // ---- Transmit Phase ----
  for (int sclks = 0; sclks < transmit_cycles; sclks++) {
    // Set lrclk_sim based on the current bit position within a frame.
    if (sclks % frame_size == frame_size - 1)
      lrclk_sim = 1;
    else
      lrclk_sim = 0;

    // Compute shift index to drive sdata_sim.
    int shift_index = frame_size - 1 - (sclks % frame_size);
    if (sclks < frame_size * num_frames)
      sdata_sim = (test_frames[frame_index] >> shift_index) & 1;
    else
      sdata_sim = 0;

    // Use simulate_mclk to run mclks_per_bit cycles for each sclk event.
    simulate_mclk(mclks_per_bit, sdata_sim, sclk_sim, lrclk_sim, frame_rdy,
                  sample_rdy, sample_counter, our_reg);

    // Update bit_count and frame_index.
    if (bit_count == frame_size - 1) {
      bit_count = 0;
      frame_index++;
    } else {
      bit_count++;
    }
  }

  // After simulation, reconstruct frames from the output FIFO.
  int fail_count = 0;
  std::cout << "Received " << our_reg.size() << " samples in our_reg."
            << std::endl;
  for (int i = 0; i < num_frames; i++) {
    for (int j = 0; j < NUM_CHANNELS; j++) {
      if (!our_reg.empty()) {
        sample_t channel = our_reg.read();

        int shift_index = (NUM_CHANNELS - 1 - j) * CHANNEL_SIZE;
        sample_t test_ch = (test_frames[i] >> shift_index) & MASK;

        std::cout << "Frame " << i << " | CH" << j << ": " << std::dec
                  << channel;
        if (channel == test_ch)
          std::cout << " PASSED." << std::endl;
        else {
          std::cout << " FAILED. Got " << channel << " expected " << test_ch
                    << std::endl;
          fail_count++;
        }
      }
    }
  }

  return fail_count;
}
