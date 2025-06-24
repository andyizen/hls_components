#include "biquad_DFII_float.h"

Biquad_DFII_float::Biquad_DFII_float() {}

smpl_t Biquad_DFII_float::process(smpl_t in_val) {

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

  // Biquad DFII structure
  res[0] = -(dly.m_w1 * coeff.a1);
  res[1] = -(dly.m_w2 * coeff.a2);
  res[2] = in_val_float + res[0] + res[1];
  res[3] = res[2] * coeff.b0;
  res[4] = dly.m_w1 * coeff.b1;
  res[5] = dly.m_w2 * coeff.b2;
  out_val_float = res[3] + res[4] + res[5];

  // Update delays
  dly.m_w2 = dly.m_w1;
  dly.m_w1 = res[2];

  // Cast Output
  quant24 = static_cast<smpl_t>(roundf(out_val_float * (1 << 23)));
  lower24 = static_cast<smpl_u_t>(quant24) & smpl_u_t(0x00FFFFFF);
  out_val = static_cast<smpl_t>(lower24 << 8);

  return out_val;
}

void Biquad_DFII_float::set_dly(dly_t *mem_dly, ch_cntr_t _ch_cnt_) {
  dly.m_w1 = 0;
  dly.m_w2 = 0;

  dly.m_w1 = mem_dly[_ch_cnt_ * NUM_DELAYS + 0];
  dly.m_w2 = mem_dly[_ch_cnt_ * NUM_DELAYS + 1];
}
void Biquad_DFII_float::set_coeff(coeff_t *mem_coeff, ch_cntr_t _ch_cnt_) {
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

void Biquad_DFII_float::update_dly(dly_t *mem_dly, ch_cntr_t _ch_cnt_) {
  mem_dly[_ch_cnt_ * NUM_DELAYS + 0] = dly.m_w1;
  mem_dly[_ch_cnt_ * NUM_DELAYS + 1] = dly.m_w2;
}
