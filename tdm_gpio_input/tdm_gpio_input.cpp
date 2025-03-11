#include "tdm_gpio_input.h"

enum State { CAPTURING, COPYING, TRANSMITTING };

void tdm_gpio_input(volatile const bit_t *sdata, volatile const bit_t *sclk,
                    volatile const bit_t *lrclk,
                    hls::stream<bit_t> &serial_stream) {
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
  static bit_t old_sclk = 0; // for edge detection

  // Read current external signals
  bit_t current_sclk = *sclk;
  bit_t current_lrclk = *lrclk;
  bit_t current_data = *sdata;

  // State machine operation:

  // Capture data on rising edge of sclk
  if (old_sclk == 0 && current_sclk == 1) {
    // Shift in the new bit (MSB-first)
    frame_reg = (frame_reg << 1) | current_data;

    // If lrclk is high on this rising edge, the frame is complete.
    if (current_lrclk == 1) {
      // Transition to TRANSMITTING state
      state = COPYING;
    } else {
      bit_count++;
    }
  }

  if (state == COPYING) {
    if (bit_count == FRAME_SIZE - 1) {
      out_reg = frame_reg;
      state = TRANSMITTING;
    } else {
      state = CAPTURING;
    }
    bit_count = 0; // Reset counter for transmission index
    frame_reg = 0;
  } else if (state == TRANSMITTING) {
    // Transmit one bit per cycle from frame_reg (MSB-first)
    bit_t out_bit = out_reg[FRAME_SIZE - 1 - out_bit_count];
    serial_stream.write(out_bit);
    out_bit_count++;

    if (out_bit_count == FRAME_SIZE) {
      out_reg = 0; // Optional: clear output register.
      out_bit_count = 0;
      state = CAPTURING;
    }
  }

  // Update old_sclk for edge detection.
  old_sclk = current_sclk;
}
