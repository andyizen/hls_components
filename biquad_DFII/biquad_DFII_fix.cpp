#include "biquad_DFII_fix.h"

Biquad_DFII_fix::Biquad_DFII_fix(const FilterCoefficients &coeff,
                                 DelayMemory &dly)
    : coeff(coeff), dly(dly) {}

smpl_t Biquad_DFII_fix::process(smpl_t in_val) {

  smpl_fix72_t res[7] = {0, 0, 0, 0, 0, 0, 0};
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

  // biquad DFII structure
  res[0] = -(dly.w1 * coeff.a1);
  res[1] = -(dly.w2 * coeff.a2);
  res[2] = in_val_fix + res[0] + res[1];
  res[3] = res[2] * coeff.b0;
  res[4] = dly.w1 * coeff.b1;
  res[5] = dly.w2 * coeff.b2;
  res[6] = res[3] + res[4] + res[5];

  // Update delay blocks
  dly.w2 = dly.w1;
  dly.w1 = res[2];

  // Cast results
  out_val_fix = res[6];

  // Rückwandlung in 24-bit-Sample
  out_val.range(31, 8) = out_val_fix.range(31, 8);
  return out_val & 0xFFFFFF00;
}
