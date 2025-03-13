#include "tdm_gpio_output.h"

void tdm_gpio_output(hls::stream<sample_t> &in_stream, bit_t sample_rdy,
                     bit_t frame_rdy, bit_t &sclk, bit_t &lrclk, bit_t &sdata,
                     bit_t &started) {
#pragma HLS INTERFACE mode = ap_none port = sclk
#pragma HLS INTERFACE mode = ap_none port = lrclk
#pragma HLS INTERFACE mode = ap_none port = sdata
#pragma HLS INTERFACE mode = ap_none port = started

#pragma HLS INTERFACE mode = ap_ctrl_none port = return
#pragma HLS INTERFACE ap_fifo port = in_stream depth = 1

  // Static variables to hold state between cycles.
  static sample_t frame_reg = 0;   // Holds the current frame.
  static sample_t out_reg = 0;     // Holds the current frame.
  static ap_uint<5> bit_index = 0; // Index for the bit currently being output.
  static ap_uint<12> clk_index = 0;
  static ap_uint<4> ch_index = 0; // Channel counter

  static bool start_output = false;
  static bool clocked = false;
  static bool sample_read = false;
  static bool sclk_state = 0;
  static bool lrclk_state = 0;
  static bool sdata_state = 0;

  // TODO??
  // I still need to shift the 24Bit output
  // how do i handle this constant read ??

  // Read from the input stream when a new frame is ready
  if (sample_rdy && frame_rdy && !sample_read) {
    frame_reg = in_stream.read();
    sample_read = true;
    start_output = true;
  } else if (sample_rdy && !sample_read) {
    frame_reg = in_stream.read();
    sample_read = true;
  } else if (!sample_rdy && sample_read) {
    sample_read = false;
  }

  if (start_output) {
    // Generate the bit clock (sclk) waveform
    if ((clk_index % CLKS_PER_BIT) == 0) {
      sclk_state = 0;
      sdata_state = (frame_reg >> (CHANNEL_SIZE - 1 - bit_index)) & 1;
    } else if ((clk_index % CLKS_PER_BIT) == CLKS_PER_BIT - 1) {
      clocked = true;
    } else if ((clk_index % CLKS_PER_BIT) >= (CLKS_PER_BIT / 2)) {
      sclk_state = 1;
    }

    // Update bit index on the falling edge of sclk
    if (clocked) {
      if (bit_index < CHANNEL_SIZE - 1) {
        bit_index++;
      } else {
        bit_index = 0; // Reset bit index
        ch_index++;    // Increment channel index when a full word is sent
      }
      clocked = false;
    }

    // Set lrclk at the last bit of the last channel
    if (ch_index == NUM_CHANNELS - 1 && bit_index == CHANNEL_SIZE - 1) {
      lrclk_state = 1;
      // Reset channel index when frame completes
    } else {
      lrclk_state = 0;
    }

    clk_index++;
    sclk = sclk_state;
    sdata = sdata_state;
    lrclk = lrclk_state;

  } else {
    // Reset outputs and internal counters when not in active transmission
    sclk = 0;
    lrclk = 0;
    sdata = 0;
    bit_index = 0;
    clk_index = 0;
    ch_index = 0;
  }
  started = start_output;
}
