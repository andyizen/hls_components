#include "biquad_DFI_fix.h"

Biquad_DFI_fix::Biquad_DFI_fix(const FilterCoefficients &coeff,
                               DelayMemory &dly)
    : coeff(coeff), dly(dly) {}

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
  // integer realm
  out_val.range(31, 8) = out_val_fix.range(31, 8);
  return out_val & 0xFFFFFF00;
}