#include "biquad_DFII_fix.h"

Biquad_DFII_fix::Biquad_DFII_fix(const FilterCoefficients &coeff,
                                 DelayMemory &dly)
    : coeff(coeff), dly(dly) {}

smpl_t Biquad_DFII_fix::process(smpl_t in_val) {

  smpl_fix72_t res[6] = {};
  smpl_fix32_t in_val_fix;
  smpl_fix32_t out_val_fix;
  smpl_t out_val;

#pragma HLS allocation operation instances = mul limit = 1
#pragma HLS allocation operation instances = add limit = 1
#pragma HLS allocation operation instances = sub limit = 1
#pragma HLS PIPELINE off

  // Eingabewert in Fixed-Point-Format
  in_val_fix = (smpl_fix32_t)in_val;

  res[0] = dly.w1 * coeff.a1;
  res[1] = dly.w2 * coeff.a2;
  res[2] = in_val_fix - res[0] - res[1];
  res[3] = res[2] * coeff.b0;
  res[4] = dly.w1 * coeff.b1;
  res[5] = dly.w2 * coeff.b2;

  out_val_fix = res[3] + res[4] + res[5];

  // Direct Form II – Zustandstransformation
  dly.w2 = dly.w1;
  dly.w1 = res[2];

  // Rückwandlung in 24-bit-Sample
  out_val.range(31, 8) = out_val_fix.range(31, 8);

  return out_val;
}
