#include "tdm_gpio_input.h"

enum State { CAPTURING, COPYING, TRANSMITTING };

void tdm_gpio_input(volatile const bit_t *sdata, volatile const bit_t *sclk,
                    volatile const bit_t *lrclk,
                    hls::stream<sample_t> &serial_stream) {
#pragma HLS INTERFACE ap_none port = sdata
#pragma HLS INTERFACE ap_none port = sclk
#pragma HLS INTERFACE ap_none port = lrclk
#pragma HLS PIPELINE II = 1

  // Static state and register variables persist across clock cycles.
  static State state = CAPTURING;
  static ap_uint<FRAME_SIZE> frame_reg = 0; // register to hold incoming frame
  static ap_uint<FRAME_SIZE> out_reg = 0;   // register to hold incoming frame
  static int bit_count = 0;                 // number of bits captured s
  static int out_bit_count = 0;
  static bit_t start = false;
  static bit_t old_sclk = 0;  // for edge detection
  static bit_t old_lrclk = 0; // for edge detection

  // Read current external signals
  bit_t current_sclk = *sclk;
  bit_t current_lrclk = *lrclk;
  bit_t current_data = *sdata;

  // State machine operation:
  // Waiting for lrclk to sync for last bit of old frame
  if (old_lrclk == 1 && current_lrclk == 0) {
    start = 1;
  }

  // Once we saw lrclk we can start capturing
  if (start) {
    // Capture data on rising edge of sclk
    if (old_sclk == 0 && current_sclk == 1) {
      // Shift in the new bit (MSB-first)
      frame_reg = (frame_reg << 1) | current_data;

      // If lrclk is high on this rising edge, the frame is complete.
      if (current_lrclk == 1) {
        // Transition to TRANSMITTING state
        state = COPYING;
      }
      bit_count++;
    }

    // Extra state of copiying just to make sure nothing gets lost in the
    // parallelism
    if (state == COPYING) {
      out_reg = frame_reg;
      bit_count = 0; // Reset counter for transmission index
      frame_reg = 0;
      state = TRANSMITTING;
    } else if (state == TRANSMITTING) {
      // Shifting out the channels
      for (int i = 0; i < NUM_CHANNELS; i++) {
        sample_t out_channel =
            (out_reg >> (CHANNEL_SIZE * (NUM_CHANNELS - 1 - i))) & MASK;
        serial_stream.write(out_channel);
      }
      state = CAPTURING;
    }

    // Update old_sclk for edge detection.
  }
  old_sclk = current_sclk;
  old_lrclk = current_lrclk;
}
