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
  if (!in_stream.empty() && _prcs_stt_ == IDLE) {
    buf_reg = in_stream.read();
    _prcs_stt_ = READ;
  }
}

void process(smpl_t buf_reg, smpl_t &out_reg, ProcessState &_prcs_stt_) {
#pragma HLS PIPELINE II = 256

  static ch_cntr_t _ch_cnt = 0;
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
#pragma HLS PIPELINE II = 256
  if (_prcs_stt_ == DONE) {
    out_stream.write(out_reg);
    _prcs_stt_ = IDLE;
  }
}

// Group the data path operations in their own function.
void data_path(smpl_ppln_t &in_stream, smpl_ppln_t &out_stream) {
  smpl_t buf_reg = 0;
  smpl_t out_reg = 0;
  ProcessState _prcs_stt_ = IDLE;

  read(in_stream, buf_reg, _prcs_stt_);
  process(buf_reg, out_reg, _prcs_stt_);
  write_stream(out_reg, out_stream, _prcs_stt_);
}

void clk_gen(volatile bit_t &mclk, bit_t &data_out) {
  static ap_uint<8> data = 0xAA;
  static ap_int<3> cnt = 4;
  static bit_t clk = 0;
#pragma HLS PIPELINE II = 1

  clk = !clk;
  mclk = clk;

  if (!clk) {
    data_out = (data >> cnt) | 0x1;
    if (cnt == 7) {
      cnt = 0;
    } else {
      cnt++;
    }
  }
}

// Top-level function: run data_path and clk_gen concurrently.
void biquad_DFI(smpl_ppln_t &in_stream, smpl_ppln_t &out_stream,
                volatile bit_t &mclk, bit_t &data_out) {
#pragma HLS INTERFACE axis port = out_stream
#pragma HLS INTERFACE axis port = in_stream
#pragma HLS INTERFACE ap_ctrl_none port = return
#pragma HLS INTERFACE ap_none port = mclk
#pragma HLS INTERFACE ap_none port = data_out
#pragma HLS DATAFLOW
  data_path(in_stream, out_stream);
  clk_gen(mclk, data_out);
}
