#include "biquad.h"

enum ProcessState { READ, PROCESS, WRITE };
enum MemoryState { LOAD, SAVE };

// FilterCoefficients factors_array[NUM_CHANNELS];

// Create internal delay memory
dly_t mem_dly[NUM_CHANNELS * NUM_DELAYS];

// Top-level function: run data_path and clk_gen concurrently.
void biquad_DFII_float(smpl_ppln_t &in_stream, smpl_ppln_t &out_stream,
                       coeff_t mem_coeff[NUM_CHANNELS * NUM_COEFFS]) {

// Kompakteste Speicherung
#pragma HLS INTERFACE mode = m_axi port = mem_coeff bundle = MEM offset = slave
#pragma HLS INTERFACE s_axilite port = mem_coeff bundle = CTRL
#pragma HLS BIND_STORAGE variable = mem_dly type = ram_1p

#pragma HLS INTERFACE axis port = out_stream
#pragma HLS INTERFACE axis port = in_stream
#pragma HLS INTERFACE mode = ap_ctrl_none port = return

  static ProcessState _prcs_stt_ = READ;
  static MemoryState _mem_stt_ = LOAD;
  static ch_cntr_t _ch_cnt_;
  static smpl_t buf_reg;
  static smpl_t out_reg;

  static DelayMemory dly;
  static FilterCoefficients coeff;
  static Biquad_DFII_float biquad = Biquad_DFII_float();

  if (_prcs_stt_ == READ) {
    buf_reg = in_stream.read();

    if (_mem_stt_ == LOAD) {
      // Prepare Filter
      biquad.set_dly(mem_dly, _ch_cnt_);
      biquad.set_coeff(mem_coeff, _ch_cnt_);
      _mem_stt_ = SAVE;
    }

    _prcs_stt_ = PROCESS;
  } else if (_prcs_stt_ == PROCESS) {
    out_reg = biquad.process(buf_reg);
    _prcs_stt_ = WRITE;
  } else if (_prcs_stt_ == WRITE) {
    // Write Delays & Output
    out_stream.write(out_reg);
    biquad.update_dly(mem_dly, _ch_cnt_);
    _ch_cnt_++;
    _prcs_stt_ = READ;
    _mem_stt_ = LOAD;
  }
}
