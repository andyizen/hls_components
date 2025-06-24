#include "biquad_DFI_float.h"

Biquad_DFI_float::Biquad_DFI_float(){};

smpl_t Biquad_DFI_float::process(smpl_t in_val) {

  smpl_t in_val_shifted = 0;
  smpl_float_t in_val_float = 0.0f;
  smpl_float_t res[6] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
  smpl_float_t out_val_float = 0.0f;
  smpl_t quant24 = 0;
  smpl_u_t lower24 = 0u;
  smpl_t out_val = 0;

  // Cast input
  in_val_shifted = static_cast<smpl_t>(in_val) >> 8;
  in_val_float = static_cast<smpl_float_t>(in_val_shifted) /
                 static_cast<smpl_float_t>(1 << 23);

#pragma HLS allocation operation instances = mul limit = 1
#pragma HLS allocation operation instances = add limit = 1
#pragma HLS allocation operation instances = sub limit = 1
#pragma HLS PIPELINE II = 128

  // Biquad DFI structure
  res[0] = in_val_float * coeff.b0;
  res[1] = dly.m_b1 * coeff.b1;
  res[2] = dly.m_b2 * coeff.b2;
  res[3] = -(dly.m_a1 * coeff.a1);
  res[4] = -(dly.m_a2 * coeff.a2);
  out_val_float = res[0] + res[1] + res[2] + res[3] + res[4];

  // Update delays
  dly.m_b2 = dly.m_b1;
  dly.m_b1 = in_val_float;
  dly.m_a2 = dly.m_a1;
  dly.m_a1 = out_val_float;

  // Cast Output
  smpl_float_t scaled = out_val_float * (1 << 23);
  quant24 = static_cast<smpl_t>(roundf(scaled));
  lower24 = static_cast<smpl_u_t>(quant24) & 0x00FFFFFFu;
  out_val = static_cast<smpl_t>(lower24 << 8);

  return out_val;
}

void Biquad_DFI_float::set_dly(dly_t *mem_dly, ch_cntr_t _ch_cnt_) {
  dly.m_a1 = 0;
  dly.m_a2 = 0;
  dly.m_b2 = 0;
  dly.m_b1 = 0;

  dly.m_a1 = mem_dly[_ch_cnt_ * NUM_DELAYS + 0];
  dly.m_a2 = mem_dly[_ch_cnt_ * NUM_DELAYS + 1];
  dly.m_b1 = mem_dly[_ch_cnt_ * NUM_DELAYS + 2];
  dly.m_b2 = mem_dly[_ch_cnt_ * NUM_DELAYS + 3];
}
void Biquad_DFI_float::set_coeff(coeff_t *mem_coeff, ch_cntr_t _ch_cnt_) {
  coeff.b0 = 0;
  coeff.b1 = 0;
  coeff.b2 = 0;
  coeff.a1 = 0;
  coeff.a2 = 0;

  coeff.b0 = mem_coeff[_ch_cnt_ * NUM_COEFFS + 0];
  coeff.b1 = mem_coeff[_ch_cnt_ * NUM_COEFFS + 1];
  coeff.b2 = mem_coeff[_ch_cnt_ * NUM_COEFFS + 2];
  coeff.a1 = mem_coeff[_ch_cnt_ * NUM_COEFFS + 3];
  coeff.a2 = mem_coeff[_ch_cnt_ * NUM_COEFFS + 4];
}

void Biquad_DFI_float::update_dly(dly_t *mem_dly, ch_cntr_t _ch_cnt_) {
  mem_dly[_ch_cnt_ * NUM_DELAYS + 0] = dly.m_a1;
  mem_dly[_ch_cnt_ * NUM_DELAYS + 1] = dly.m_a2;
  mem_dly[_ch_cnt_ * NUM_DELAYS + 2] = dly.m_b1;
  mem_dly[_ch_cnt_ * NUM_DELAYS + 3] = dly.m_b2;
}
