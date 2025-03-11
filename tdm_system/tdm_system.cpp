#include "tdm_system.h"

void tdm_system(volatile const bit_t *sclk_in, volatile const bit_t *lrclk_in,
                volatile const bit_t *sdata_in,
                hls::stream<frame_t> &frame_stream) {

  // Internal streams connecting the modules.
  static hls::stream<sample_t>
      sample_stream; // from tdm_gpio_input to process_frame
  // hls::stream<frame_t> frame_stream;   // from process_frame to
  // tdm_gpio_output

  // Call each module. Note: these functions are assumed to be designed for
  // cycle‑accurate operation.

#pragma HLS PIPELINE II = 1
  tdm_gpio_input(sclk_in, lrclk_in, sdata_in, sample_stream);
  process_frame(sample_stream, frame_stream);
}
