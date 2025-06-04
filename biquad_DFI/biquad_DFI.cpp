#include "biquad_DFI.h"
#include "biquad_DFI_fix.h"
#include "hls_task.h"

enum ProcessState { READ, PROCESS, WRITE };

// FilterCoefficients factors_array[NUM_CHANNELS];
Biquad_DFI_fix biquad = Biquad_DFI_fix();

// Top-level function: run data_path and clk_gen concurrently.
void biquad_DFI(smpl_ppln_t &in_stream, smpl_ppln_t &out_stream,
                const FilterCoefficients coeff[NUM_CHANNELS],
                smpl_fix_t mem[NUM_CHANNELS * NUM_OF_DELAYS]) {
// Kompakteste Speicherung
#pragma HLS INTERFACE m_axi port = coeff offset = slave bundle = MEM
#pragma HLS INTERFACE s_axilite port = coeff bundle = CTRL
#pragma HLS INTERFACE mode = ap_memory port = mem storage_type = ram_1p

#pragma HLS INTERFACE axis port = out_stream
#pragma HLS INTERFACE axis port = in_stream
#pragma HLS INTERFACE mode = ap_ctrl_none port = return

  static ProcessState _prcs_stt_ = READ;

  static ch_cntr_t _ch_cnt_;
  static DelayMemory dly;
  static smpl_t buf_reg;
  static smpl_t out_reg;

  if (_prcs_stt_ == READ) {
    buf_reg = in_stream.read();
    // Prepare Delays
    dly.m_a1 = mem[_ch_cnt_ * NUM_OF_DELAYS + 0];
    dly.m_a2 = mem[_ch_cnt_ * NUM_OF_DELAYS + 1];
    dly.m_b1 = mem[_ch_cnt_ * NUM_OF_DELAYS + 2];
    dly.m_b2 = mem[_ch_cnt_ * NUM_OF_DELAYS + 3];

    biquad.set_coeff(coeff[_ch_cnt_]);
    biquad.set_dly(dly);
    _prcs_stt_ = PROCESS;
  } else if (_prcs_stt_ == PROCESS) {
    out_reg = biquad.process(buf_reg);
    _prcs_stt_ = WRITE;
  } else if (_prcs_stt_ == WRITE) {
#pragma HLS PIPELINE off
    out_stream.write(out_reg);
    dly = biquad.get_dly();
    mem[_ch_cnt_ * NUM_OF_DELAYS + 0] = dly.m_a1;
    mem[_ch_cnt_ * NUM_OF_DELAYS + 1] = dly.m_a2;
    mem[_ch_cnt_ * NUM_OF_DELAYS + 2] = dly.m_b1;
    mem[_ch_cnt_ * NUM_OF_DELAYS + 3] = dly.m_b2;
    _ch_cnt_++;
    _prcs_stt_ = READ;
  }
}
