#include "biquad_DFI_fix.h"

Biquad_DFI_fix::Biquad_DFI_fix() {}
smpl_t Biquad_DFI_fix::process(smpl_t in_val) {

  smpl_fix72_t res[6] = {0, 0, 0, 0, 0, 0};
  smpl_fix_inout_t in_val_fix = 0;
  smpl_fix_inout_t out_val_fix = 0;
  smpl_t out_val = 0;

// Limit ressource usage
#pragma HLS allocation operation instances = mul limit = 1
#pragma HLS allocation operation instances = add limit = 1
#pragma HLS allocation operation instances = sub limit = 1
#pragma HLS PIPELINE off

  // Eingabewert in Fixed-Point-Format
  in_val_fix.range(31, 8) = in_val.range(31, 8);

  // biquad DFI structure
  res[0] = in_val_fix * coeff.b0;
  res[1] = dly.m_b1 * coeff.b1;
  res[2] = dly.m_b2 * coeff.b2;
  res[3] = -(dly.m_a1 * coeff.a1);
  res[4] = -(dly.m_a2 * coeff.a2);
  res[5] = res[0] + res[1] + res[2] + res[3] + res[4];

  // Update delay blocks
  dly.m_b2 = dly.m_b1;
  dly.m_b1 = in_val_fix;
  dly.m_a2 = dly.m_a1;
  dly.m_a1 = res[5];

  // Cast result
  out_val_fix = res[5];

  // Put the fixed value into a out shift containerm, then shift back into
  // Unsigned cast so the sign does not get shifted to the MSB
  out_val = static_cast<smpl_u_t>(out_val_fix.range(31, 8)) & 0x00FFFFFFu;
  return out_val << 8;
}

void Biquad_DFI_fix::set_dly(dly_t *mem_dly, ch_cntr_t _ch_cnt_) {
  dly.m_a1 = 0;
  dly.m_a2 = 0;
  dly.m_b2 = 0;
  dly.m_b1 = 0;

  dly.m_a1 = mem_dly[_ch_cnt_ * NUM_DELAYS + 0];
  dly.m_a2 = mem_dly[_ch_cnt_ * NUM_DELAYS + 1];
  dly.m_b1 = mem_dly[_ch_cnt_ * NUM_DELAYS + 2];
  dly.m_b2 = mem_dly[_ch_cnt_ * NUM_DELAYS + 3];
}
void Biquad_DFI_fix::set_coeff(coeff_t *mem_coeff, ch_cntr_t _ch_cnt_) {
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

void Biquad_DFI_fix::update_dly(dly_t *mem_dly, ch_cntr_t _ch_cnt_) {
  mem_dly[_ch_cnt_ * NUM_DELAYS + 0] = dly.m_a1;
  mem_dly[_ch_cnt_ * NUM_DELAYS + 1] = dly.m_a2;
  mem_dly[_ch_cnt_ * NUM_DELAYS + 2] = dly.m_b1;
  mem_dly[_ch_cnt_ * NUM_DELAYS + 3] = dly.m_b2;
}