#include "biquad_DFI.h"

enum ProcessState { IDLE, DONE };

// Define a structure for the filter coefficients.

FilterFactors factors_array[NUM_CHANNELS];

void read_and_process(smpl_ppln_t &in_stream, Biquad &biquads, smpl_t &out_reg,
                      ProcessState &_prcs_stt_) {

  if (!in_stream.empty() && _prcs_stt_ == IDLE) {
    smpl_t in_reg = in_stream.read();
    out_reg = biquads.process(in_reg);
    _prcs_stt_ = DONE;
  }
}
void write_stream(smpl_t out_reg, smpl_ppln_t &out_stream,
                  ProcessState &_prcs_stt_) {
  if (_prcs_stt_ == DONE) {
    out_stream.write(out_reg);
    _prcs_stt_ = IDLE;
  }
}

void biquad_DFI(smpl_ppln_t &in_stream, smpl_ppln_t &out_stream) {
#pragma HLS INTERFACE ap_ctrl_none port = return
#pragma HLS INTERFACE mode = axis port = in_stream depth = 1
#pragma HLS INTERFACE mode = axis port = out_stream depth = 1
#pragma HLS pipeline II = 1

  static Biquad biquads;
  static smpl_t out_reg = 0;
  static ProcessState _prcs_stt_ = IDLE;

  read_and_process(in_stream, biquads, out_reg, _prcs_stt_);
// PIPELINE directive essential here, otherwise II > 1
#pragma HLS PIPELINE
  write_stream(out_reg, out_stream, _prcs_stt_);
}
