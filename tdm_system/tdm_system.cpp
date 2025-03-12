#include "tdm_system.h"

void gather(hls::stream<sample_t> &sample_in, hls::stream<frame_t> &frame_out) {

#pragma HLS PIPELINE II = 1

  // Buffer to store incoming samples until we have a full frame.
  static sample_t buffer[NUM_CHANNELS];
#pragma HLS ARRAY_PARTITION variable = buffer complete

  // Counter for number of samples accumulated.
  static int count = 0;

  // If there is data available, read one sample and store it.
  if (!sample_in.empty()) {
    buffer[count] = sample_in.read();
    count++;
  }

  // Once we have NUM_CHANNELS samples, pack them into one frame.
  if (count == NUM_CHANNELS) {
    frame_t frame = 0;
    // Pack samples in MSB-first order: channel 0 becomes the most-significant
    // bits.
    for (int i = 0; i < NUM_CHANNELS; i++) {
#pragma HLS UNROLL
      frame = (frame << CHANNEL_SIZE) | buffer[i];
    }
    frame_out.write(frame);
    count = 0; // Reset for the next frame.
  }
}

void tdm_system(volatile const bit_t *sclk_in, volatile const bit_t *lrclk_in,
                volatile const bit_t *sdata_in, volatile bit_t *sclk_out,
                volatile bit_t *lrclk_out, volatile bit_t *sdata_out) {

#pragma HLS INTERFACE mode = ap_ctrl_none port = return
#pragma HLS INTERFACE ap_none port = sdata_in
#pragma HLS INTERFACE ap_none port = sclk_in
#pragma HLS INTERFACE ap_none port = lrclk_in

#pragma HLS DATAFLOW

  // Internal streams connecting the modules.
  hls::stream<sample_t> sample_stream;
  hls::stream<frame_t> out;

  tdm_gpio_input(sdata_in, sclk_in, lrclk_in, sample_stream);
  gather(sample_stream, out);
  tdm_gpio_output(sclk_out, lrclk_out, sdata_out, out);
}