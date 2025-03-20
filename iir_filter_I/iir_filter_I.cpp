#include "iir_filter_I.h"

enum ProcessState { IDLE, READY };
struct FilterStt {
  ap_uint<4> prev1; // state from the last iteration
  ap_uint<4> prev2; // state from two iterations ago
};

void process_filter(ap_uint<4> in_val, FilterStt &state, ap_uint<4> &out_val) {
#pragma HLS PIPELINE II = 1
  out_val = in_val + state.prev1 + state.prev2;

  state.prev2 = state.prev1;
  state.prev1 = in_val;

  /* // Calc Output
  const ap_uint<16> b0 = 1;
  const ap_uint<16> b1 = 1;
  const ap_uint<16> b2 = 1;
  const ap_uint<16> a1 = 1;
  const ap_uint<16> a2 = 1;
  out_val = in_val * b0 + all_ch_buffer[0] * b1 + all_ch_buffer[1] * b2 -
            all_ch_buffer[2] * a1 - all_ch_buffer[3] * a2;

  // Update forward track
  all_ch_buffer[1] = all_ch_buffer[0];
  all_ch_buffer[0] = in_val;
  // Update feedback track
  all_ch_buffer[3] = all_ch_buffer[2];
  all_ch_buffer[2] = out_val;
  */
}

void read_frame(smpl_ppln_t &in_stream, smpl_t cur_ch_val[NUM_CHANNELS],
                ProcessState &_prcs_stt_) {

  static ch_cntr_t _ch_cnt_ = 0;
  if (!in_stream.empty()) {
    cur_ch_val[_ch_cnt_] = in_stream.read();
    if (_ch_cnt_ == NUM_CHANNELS - 1) {
      _prcs_stt_ = READY;
    }
    _ch_cnt_++;
  }
}

void iir_filter_I(ap_uint<64> in_stream, ap_uint<64> &out_stream) {
#pragma HLS INTERFACE ap_ctrl_none port = return

  enum CTRL { READ, PROCESS, WRITE };
  // If each channel is 4 bits, then use ap_uint<4>.
  // (If your filter coefficients and process_filter expect 8-bit, adjust
  // accordingly.)
  static FilterStt state_array[NUM_CHANNELS] = {0};
  ap_uint<4> out_reg[NUM_CHANNELS];
  ap_uint<4> in_reg[NUM_CHANNELS];
  CTRL flow = READ;

UNFOLD:
  // Extract each 4-bit channel from the 64-bit input.
  if (flow == READ) {
    for (int i = 0; i < NUM_CHANNELS; i++) {
#pragma HLS PIPELINE rewind
#pragma HLS UNROLL factor = 16
      ap_uint<4> curval = (in_stream >> (4 * i)) & 0xF;
      in_reg[i] = curval; // Use mask 0xF for 4 bits.
    }
    flow = PROCESS;
  }

// Process each channel.
POCESS:
  if (flow == PROCESS) {
    for (int i = 0; i < NUM_CHANNELS; i++) {
#pragma HLS PIPELINE rewind
#pragma HLS UNROLL factor = 16
      process_filter(in_reg[i], state_array[i], out_reg[i]);
    }
    flow = WRITE;
  }

MERGE:
  if (flow == WRITE) {
    // Pack the processed channels into out_stream.
    out_stream = 0;
    for (int i = 0; i < NUM_CHANNELS; i++) {
#pragma HLS PIPELINE rewind
#pragma HLS UNROLL factor = 16
      out_stream = (out_stream << 4) | out_reg[i];
    }
    flow = READ;
  }
}
