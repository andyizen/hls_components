#include "biquad.h"

enum ProcessState { READ, PROCESS, WRITE };
enum MemoryState { LOAD, SAVE };

// FilterCoefficients factors_array[NUM_CHANNELS];

// Create internal delay memory
dly_72_t mem_dly[NUM_CHANNELS * NUM_DELAYS];

// Top-level function: run data_path and clk_gen concurrently.
void biquad_DFI(smpl_ppln_t &in_stream, smpl_ppln_t &out_stream,
                const coeff32_t mem_coeff[NUM_CHANNELS * NUM_COEFFS]) {
// Kompakteste Speicherung
#pragma HLS INTERFACE mode = m_axi port = mem_coeff bundle = MEM offset = slave
#pragma HLS INTERFACE s_axilite port = mem_coeff bundle = CTRL

/*#pragma HLS INTERFACE s_axilite port = read_coeffs bundle = COEFF*/
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
  static Biquad_DFI_fix biquad = Biquad_DFI_fix(coeff, dly);

  if (_prcs_stt_ == READ) {
    buf_reg = in_stream.read();

    if (_mem_stt_ == LOAD) {
      // Prepare Delays
      dly.m_a1 = mem_dly[_ch_cnt_ * NUM_DELAYS + 0];
      dly.m_a2 = mem_dly[_ch_cnt_ * NUM_DELAYS + 1];
      dly.m_b1 = mem_dly[_ch_cnt_ * NUM_DELAYS + 2];
      dly.m_b2 = mem_dly[_ch_cnt_ * NUM_DELAYS + 3];

      // Prepare Coeffs
      coeff.b0 = mem_coeff[_ch_cnt_ * NUM_COEFFS + 0];
      coeff.b1 = mem_coeff[_ch_cnt_ * NUM_COEFFS + 1];
      coeff.b2 = mem_coeff[_ch_cnt_ * NUM_COEFFS + 2];
      coeff.a1 = mem_coeff[_ch_cnt_ * NUM_COEFFS + 3];
      coeff.a2 = mem_coeff[_ch_cnt_ * NUM_COEFFS + 4];
      _mem_stt_ = SAVE;
    }

    _prcs_stt_ = PROCESS;
  } else if (_prcs_stt_ == PROCESS) {
    out_reg = biquad.process(buf_reg);
    _prcs_stt_ = WRITE;
  } else if (_prcs_stt_ == WRITE) {
#pragma HLS PIPELINE off
    out_stream.write(out_reg);
    mem_dly[_ch_cnt_ * NUM_DELAYS + 0] = dly.m_a1;
    mem_dly[_ch_cnt_ * NUM_DELAYS + 1] = dly.m_a2;
    mem_dly[_ch_cnt_ * NUM_DELAYS + 2] = dly.m_b1;
    mem_dly[_ch_cnt_ * NUM_DELAYS + 3] = dly.m_b2;
    _ch_cnt_++;
    _prcs_stt_ = READ;
    _mem_stt_ = LOAD;
  }
}
