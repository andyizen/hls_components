#include "iir_filter_I.h"

// Define a structure for the filter coefficients.

FilterFactors factors_array[NUM_CHANNELS];

void read_stream(ap_uint<64> in_stream, ap_uint<4> in_reg[NUM_CHANNELS]) {
#pragma HLS INLINE
  for (int i = 0; i < NUM_CHANNELS; i++) {
#pragma HLS PIPELINE rewind
#pragma HLS UNROLL factor = 16
    in_reg[i] = (in_stream >> (4 * i)) & 0xF;
  }
}

void process_channels(ap_uint<4> in_reg[NUM_CHANNELS],
                      ap_uint<4> out_reg[NUM_CHANNELS],
                      Biquad biquads[NUM_CHANNELS]) {
#pragma HLS INLINE
  for (int i = 0; i < NUM_CHANNELS; i++) {
#pragma HLS PIPELINE rewind
#pragma HLS UNROLL factor = 16
    out_reg[i] = biquads[i].process(in_reg[i]);
  }
}

void write_stream(ap_uint<4> out_reg[NUM_CHANNELS], ap_uint<64> &out_stream) {
#pragma HLS INLINE
  out_stream = 0;
  for (int i = 0; i < NUM_CHANNELS; i++) {
#pragma HLS PIPELINE rewind
#pragma HLS UNROLL factor = 16
    out_stream = (out_stream << 4) | out_reg[i];
  }
}

void iir_filter_I(ap_uint<64> in_stream, ap_uint<64> &out_stream) {
#pragma HLS INTERFACE ap_ctrl_none port = return

  static Biquad biquads[NUM_CHANNELS];

  ap_uint<4> in_reg[NUM_CHANNELS];
  ap_uint<4> out_reg[NUM_CHANNELS];

  // Chopping up into single
  read_stream(in_stream, in_reg);
  process_channels(in_reg, out_reg, biquads);
  write_stream(out_reg, out_stream);
}
