#include "tdm_gpio_input.h"

void tdm_gpio_input(const bit_t sdata, const bit_t sclk, const bit_t lrclk,
                    bit_t &frame_rdy, bit_t &sample_rdy,
                    ap_uint<4> &sample_count, hls::stream<sample_t> &out) {

#pragma HLS INTERFACE ap_none port = sdata
#pragma HLS INTERFACE ap_none port = sclk
#pragma HLS INTERFACE ap_none port = lrclk
#pragma HLS INTERFACE ap_none port = frame_rdy
#pragma HLS INTERFACE ap_none port = sample_rdy
#pragma HLS INTERFACE ap_none port = sample_count
#pragma HLS INTERFACE mode = ap_ctrl_none port = return

#pragma HLS INTERFACE ap_fifo port = out depth = 1
#pragma HLS PIPELINE II = 1

  static sample_t sample_reg = 0;
  static sample_t out_reg = 0; // This will hold the stable output.
  static ap_uint<6> sclk_counter = 0;
  static ap_uint<5> sample_counter = 0;
  static ap_uint<5> sample_counter_latched = 0;

  static bool new_sample = false;
  static bool new_frame = false;
  static bool data_sampled = false;
  static bool last_bit = false;
  static bool sample_latched = false;
  static bool written = false;

  // Catching the last bit transition.
  if (lrclk && !last_bit) {
    last_bit = true;
  } else if (!lrclk && last_bit) {
    last_bit = false;
    sample_counter = 0;
    sclk_counter = 0;
  }

  // Capture bits on rising edge of sclk.
  if (sclk == 1 && !data_sampled) {
    // Shift in a new bit.
    sample_reg = (sample_reg << 1) | sdata;

    // When we have captured the full channel width:
    if (sclk_counter == CHANNEL_SIZE) {
      new_sample = true;
      written = false; // A complete sample is ready.
      // Latch the value: update out_reg only once.
      if (!sample_latched) {
        out_reg = sample_reg & MASK;
        sample_latched = true;
      }
      sclk_counter = 0;
      sample_counter++;
      sample_counter_latched = sample_counter;
      // Optionally, check if a full frame (NUM_CHANNELS samples) is complete.
      if (sample_counter == NUM_CHANNELS) {
        new_frame = true;
      }
    }
    data_sampled = true;
  } else if (sclk == 0) {
    if (data_sampled)
      sclk_counter++;
    data_sampled = false;
    // On the falling edge, clear new_sample flags and sample_latched so that
    // out_reg won't update until the next complete sample is captured.
    new_sample = false;
    new_frame = false;
    sample_latched = false;
  }

  // Only write to the FIFO when a new sample is ready.
  if (new_sample && sample_latched && !written) {
    out.write(out_reg);
    written = true;
    // new_sample will be cleared on sclk falling edge, so only one write per
    // sample.
  }

  // Drive the ready signals (these remain high only for one cycle per
  // sample/frame)
  sample_count = sample_counter_latched;
  sample_rdy = new_sample;
  frame_rdy = new_frame;
}
