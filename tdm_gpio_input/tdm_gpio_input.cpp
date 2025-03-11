#include "tdm_gpio_input.h"

// Capture process: Continuously read serial bits into a frame.
void tdm_gpio_input(volatile const bit_t *sdata, volatile const bit_t *sclk,
                    volatile const bit_t *lrclk,
                    hls::stream<sample_t> &frame_fifo) {
#pragma HLS PIPELINE II = 1
  static sample_t sample_reg = 0;
  static int bit_count = 0;
  static bit_t old_sclk = 0;
  static bit_t old_lrclk = 0;
  static bool start = false;
  bit_t current_sclk = *sclk;
  bit_t current_lrclk = *lrclk;
  bit_t current_data = *sdata;

  if (old_lrclk == 1 && current_lrclk == 0) {
    start = true;
  }

  if (start) {
    // Detect rising edge of sclk.
    if (old_sclk == 0 && current_sclk == 1) {
      sample_reg = (sample_reg << 1) | current_data;
      bit_count++;
      // If a complete channel has been captured.
      if (bit_count == CHANNEL_SIZE) {
        frame_fifo.write(sample_reg);
        sample_reg = 0;
        bit_count = 0;
      }
    }
  }
  old_sclk = current_sclk;
  old_lrclk = current_lrclk;
}
