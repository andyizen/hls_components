#include "tdm_gpio_input.h"

void tdm_gpio_input(volatile const bit_t *sdata, volatile const bit_t *sclk,
                    volatile const bit_t *lrclk, hls::stream<sample_t> &out) {
#pragma HLS INTERFACE ap_none port = sdata
#pragma HLS INTERFACE ap_none port = sclk
#pragma HLS INTERFACE ap_none port = lrclk
#pragma HLS PIPELINE II = 1

  static sample_t sample_reg = 0;
  static ap_uint<5> sclk_count = 0;
  static bit_t old_sclk = 0;
  static bit_t old_lrclk = 0;
  static bool new_sample = false; // Flag to indicate a new sample is available.

  bit_t current_sclk = *sclk;
  bit_t current_lrclk = *lrclk;
  bit_t current_data = *sdata;

  // On rising edge of sclk, shift in new bit.
  if (!old_sclk && current_sclk) {
    sample_reg = (sample_reg << 1) | current_data;
  }

  // On falling edge of sclk, update counter.

  if (old_sclk && !current_sclk) {
    if (sclk_count == 31) { // Assuming a 32-bit channel (CHANNEL_SIZE==32)
      new_sample = true;    // Mark that a new sample is available.
    }
    sclk_count++;
  }

  // Reset sclk_count if lrclk falling edge detected.
  if (old_lrclk && !current_lrclk) {
    sclk_count = 0;
  }

  // Only write to the FIFO if a new sample has been captured.
  if (new_sample) {
    if (!out.full())
      out.write(sample_reg & MASK);
    new_sample = false; // Clear the flag so we don't write repeatedly.
  }

  old_sclk = current_sclk;
  old_lrclk = current_lrclk;
}
