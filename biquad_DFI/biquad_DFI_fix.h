#ifndef BIQUAD_DFI_H
#define BIQUAD_DFI_H

#include "../lib/tdm_system_spec.h"
#include "ap_float.h"
#include <ap_fixed.h>
#include <stdint.h>

typedef ap_fixed<48, 24, AP_TRN, AP_WRAP> smpl_fix_inout_t;
typedef ap_fixed<48, 2, AP_TRN, AP_WRAP> smpl_fix_t;

constexpr int32_t float_to_q2_30(float x) {
  const int64_t Q2_30_SCALE = 1LL << 30;
  return static_cast<int32_t>(
      (x >= 2.0f ? 1.999999f : (x < -2.0f ? -2.0f : x)) * Q2_30_SCALE);
}

struct FilterCoefficients {
  const smpl_fix_t b0 = 0.003916126660547368;
  const smpl_fix_t b1 = 0.007832253321094737;
  const smpl_fix_t b2 = 0.003916126660547368;
  const smpl_fix_t a1 = -1.8153410827045682;
  const smpl_fix_t a2 = 0.8310055893467577;
};

struct DelayMemory {
  smpl_fix_t m_b1;
  smpl_fix_t m_b2;
  smpl_fix_t m_a1;
  smpl_fix_t m_a2;
};

class Biquad_DFI_fix {

private:
  // Memory elements for state
  DelayMemory dly;
  FilterCoefficients coeff;

public:
  Biquad_DFI_fix() {}

  smpl_t process(smpl_t in_val) {
#pragma HLS PIPELINE II = 128

    smpl_fix_inout_t in_shifted;
    smpl_fix_inout_t out_shifted;
    smpl_fix_inout_t res[7];
    smpl_fix_t in_val_fix;
    smpl_fix_t out_val_fix;

// Put the addition into the fabric here
// Also save the delays in BRAM to save LUT space
// Allocate only one ressource for adding and multiplying
#pragma HLS bind_op variable = in_val_fix op = mul impl = fabric
#pragma HLS bind_op variable = out_val_fix op = add impl = fabric
#pragma HLS bind_storage variable = dly type = RAM_1P impl = bram
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
    // Add forward and backward circle
    res[5] = res[0] + res[1] + res[2];
    res[6] = res[3] + res[4];
    out_val_fix = res[5] + res[6];

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
};

void biquad_DFI(smpl_ppln_t &in_stream, smpl_ppln_t &out_stream);

#endif