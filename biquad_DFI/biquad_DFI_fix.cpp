#include "biquad_DFI_fix.h"

Biquad_DFI_fix::Biquad_DFI_fix(const FilterCoefficients &coeff,
                               DelayMemory &dly)
    : coeff(coeff), dly(dly) {}

smpl_t Biquad_DFI_fix::process(smpl_t in_val) {
#pragma HLS PIPELINE II = 128

  smpl_fix_inout_t in_shifted;
  smpl_fix_inout_t out_shifted;
  smpl_fix_inout_t res[5];
  smpl_fix32_t in_val_fix;
  smpl_fix32_t out_val_fix;

// Put the addition into the fabric here
// Also save the delays in BRAM to save LUT space
// Allocate only one ressource for adding and multiplying
#pragma HLS bind_op variable = in_val_fix op = mul impl = fabric
#pragma HLS bind_op variable = out_val_fix op = add impl = fabric
#pragma HLS bind_storage variable = dly type = RAM_1P impl = bram latency = 3
#pragma HLS allocation operation instances = mul limit = 1
#pragma HLS allocation operation instances = add limit = 1

  // Shift down the 24Bit audio data and then shift this into fix point realm
  in_shifted = in_val >> 8;
  in_val_fix = in_shifted >> 23;

  // Classic biquad DFI structure
  res[0] = in_val_fix * coeff.b0;
  res[1] = dly.m_b1 * coeff.b1;
  res[2] = dly.m_b2 * coeff.b2;
  res[3] = -(dly.m_a1 * coeff.a1);
  res[4] = -(dly.m_a2 * coeff.a2);

  out_val_fix = res[0] + res[1] + res[2] + res[3] + res[4];

  // Update delay blocks
  dly.m_b2 = dly.m_b1;
  dly.m_b1 = in_val_fix;
  dly.m_a2 = dly.m_a1;
  dly.m_a1 = out_val_fix;

  // Put the fixed value into a out shift containerm, then shift back into
  // integer realm
  out_shifted = out_val_fix;
  out_shifted = out_shifted << 23;
  // Truncate and shift the 24Bit value back up to MSBs of audio data
  // container
  return (out_shifted.to_int() << 8);
}

// DelayMemory Biquad_DFI_fix::get_dly() { return dly; }
