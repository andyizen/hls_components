#include "tdm_gpio_output.h"

void tdm_gpio_output(volatile bit_t *sclk, volatile bit_t *lrclk,
                     volatile bit_t *sdata, hls::stream<frame_t> &in_stream) {
#pragma HLS INTERFACE mode = ap_none port = sclk
#pragma HLS INTERFACE mode = ap_none port = lrclk
#pragma HLS INTERFACE mode = ap_none port = sdata

  // Static variables to hold state between cycles.
  static frame_t frame_reg = 0; // Holds the current frame.
  static frame_t out_reg = 0;   // Holds the current frame.
  static int bit_index = 0;     // Index for the bit currently being output.
  static int clk_index = 0;
  static bool idle = true;
  static hls::stream<sample_t> sample_stream;

  // Copy incoming data
  if (!in_stream.empty() && idle == true) {
    frame_reg = in_stream.read();
    idle = false;
  }

  // Set data
  if (bit_index == 0) {
    out_reg = frame_reg;
  }

  bit_t out_bit = out_reg[FRAME_SIZE - 1 - bit_index];

  // CLOCKING
  //  SCLK
  if (clk_index < 4) {
    *sclk = 0;
    *sdata = out_bit;
  } else if (clk_index < 8) {
    *sclk = 1;
  }
  // LRCLCK
  if (bit_index == NUM_CHANNELS * CHANNEL_SIZE - 1) {
    *lrclk = 1;
  } else {
    *lrclk = 0;
  }

  // RESET CYCLES
  if (clk_index == 7) {
    // Reset clk
    clk_index = 0;
    // Reset bit index if we are at the end of a frame
    if (bit_index == NUM_CHANNELS * CHANNEL_SIZE - 1) {
      bit_index = 0;
      idle = true;
    } else {
      bit_index++;
    }
  } else {
    clk_index++;
  }
}
