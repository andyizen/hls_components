#include "biquad_DFI_fix.h"
#include "hls_task.h"

enum ProcessState { IDLE, READ, DONE };

FilterCoefficients factors_array[NUM_CHANNELS];
Biquad_DFI_fix biquads[NUM_CHANNELS] = {
    Biquad_DFI_fix(), Biquad_DFI_fix(), Biquad_DFI_fix(), Biquad_DFI_fix(),
    Biquad_DFI_fix(), Biquad_DFI_fix(), Biquad_DFI_fix(), Biquad_DFI_fix(),
    Biquad_DFI_fix(), Biquad_DFI_fix(), Biquad_DFI_fix(), Biquad_DFI_fix(),
    Biquad_DFI_fix(), Biquad_DFI_fix(), Biquad_DFI_fix(), Biquad_DFI_fix(),
};

// Data path functions
void read(smpl_ppln_t &in_stream, smpl_t &buf_reg, ProcessState &_prcs_stt_) {
#pragma HLS INLINE
  if (_prcs_stt_ == IDLE) {
    buf_reg = in_stream.read();
    _prcs_stt_ = READ;
  }
}

void process(smpl_t buf_reg, smpl_t &out_reg, ProcessState &_prcs_stt_,
             FilterCoefficients coeff[NUM_CHANNELS]) {
#pragma HLS INLINE
  static ch_cntr_t _ch_cnt = 0;
  biquads[_ch_cnt].set_coeff(coeff[_ch_cnt]);
  if (_prcs_stt_ == READ) {
    out_reg = biquads[_ch_cnt].process(buf_reg);
    _prcs_stt_ = DONE;
    if (_ch_cnt == NUM_CHANNELS - 1) {
      _ch_cnt = 0;
    } else {
      _ch_cnt++;
    }
  }
}

void write_stream(smpl_t out_reg, smpl_ppln_t &out_stream,
                  ProcessState &_prcs_stt_) {
#pragma HLS INLINE
  if (_prcs_stt_ == DONE) {
    out_stream.write(out_reg);
    _prcs_stt_ = IDLE;
  }
}

// Top-level function: run data_path and clk_gen concurrently.
void biquad_DFI(smpl_ppln_t &in_stream, smpl_ppln_t &out_stream,
                FilterCoefficients coeff[NUM_CHANNELS]) {
#pragma HLS INTERFACE mode = s_axilite port = coeff storage_impl = bram
#pragma HLS INTERFACE axis port = out_stream
#pragma HLS INTERFACE axis port = in_stream
#pragma HLS INTERFACE mode = ap_ctrl_none port = return

  static smpl_t buf_reg = 0;
  static smpl_t out_reg = 0;
  ProcessState _prcs_stt_ = IDLE;
  read(in_stream, buf_reg, _prcs_stt_);
  process(buf_reg, out_reg, _prcs_stt_, coeff);
  write_stream(out_reg, out_stream, _prcs_stt_);
}
