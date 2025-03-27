#ifndef BIQUAD_DFI_H
#define BIQUAD_DFI_H

#include "../lib/tdm_system_spec.h"
#include "ap_float.h"
#include <ap_fixed.h>
#include <stdint.h>

const smpl_t PCM_MAX_POS_VAL = 0x7FFFFF;
const smpl_t PCM_MAX_NEG_VAL = 0x800000;
typedef ap_fixed<64, 32, AP_RND, AP_SAT> smpl_fix_t;
const smpl_fix_t inv_scale_pos =
    static_cast<smpl_fix_t>(1) / static_cast<smpl_fix_t>(PCM_MAX_POS_VAL >> 8);
const smpl_fix_t inv_scale_neg =
    static_cast<smpl_fix_t>(1) / static_cast<smpl_fix_t>(PCM_MAX_NEG_VAL >> 8);

constexpr int32_t float_to_q2_30(float x) {
  const int64_t Q2_30_SCALE = 1LL << 30;
  return static_cast<int32_t>(
      (x >= 2.0f ? 1.999999f : (x < -2.0f ? -2.0f : x)) * Q2_30_SCALE);
}

struct FilterCoefficients {
  smpl_fix_t b0;
  smpl_fix_t b1;
  smpl_fix_t b2;
  smpl_fix_t a1;
  smpl_fix_t a2;
};

class Biquad_DFI_fix {
private:
  // Memory elements for state
  smpl_fix_t m_b1;
  smpl_fix_t m_b2;
  smpl_fix_t m_a1;
  smpl_fix_t m_a2;
  FilterCoefficients coeff;

public:
  Biquad_DFI_fix() {

    coeff.b0 = 0.003916126660547368;
    coeff.b1 = 0.007832253321094737;
    coeff.b2 = 0.003916126660547368;
    coeff.a1 = -1.8153410827045682;
    coeff.a2 = 0.8310055893467577;

    /*     coeff.b0 = 1.0;
        coeff.b1 = 1.0;
        coeff.b2 = 1.0;
        coeff.a1 = 1.0;
        coeff.a2 = 1.0; */
  }

  smpl_t process(smpl_t in_val) {

    smpl_fix_t in_val_fix;
    smpl_fix_t rb0;
    smpl_fix_t rb1;
    smpl_fix_t rb2;
    smpl_fix_t ra1;
    smpl_fix_t ra2;
    smpl_fix_t out_val_fix;
    smpl_t out_val;

#pragma HLS bind_op variable = in_val_fix op = mul impl = fabric
#pragma HLS bind_op variable = out_val_fix op = add impl = fabric
#pragma HLS allocation operation instances = mul limit = 1
    // Precompute the reciprocal constant (do this once, e.g., in the
    // constructor or as a global constant)

    // Then in your process function:
    if (in_val < 0) {
      in_val_fix = static_cast<smpl_fix_t>(in_val >> 8) * inv_scale_neg;
    } else {
      in_val_fix = static_cast<smpl_fix_t>(in_val >> 8) * inv_scale_pos;
    }

    // smpl_fix_t rb0 = mul_Q2_30(in_val_fix, coeff.b0);
    // Intermediate results

    rb0 = in_val_fix * coeff.b0;
    rb1 = m_b1 * coeff.b1;
    rb2 = m_b2 * coeff.b2;
    ra1 = m_a1 * coeff.a1;
    ra2 = m_a2 * coeff.a2;

    out_val_fix = rb0 + rb1 + rb2 - ra1 - ra2;

    // Update state: shift the previous inputs/outputs.
    m_b2 = m_b1;
    m_b1 = in_val_fix;
    m_a2 = m_a1;
    m_a1 = out_val_fix;

    // Shift output back to INT32
    if (in_val < 0) {
      out_val =
          static_cast<smpl_t>((out_val_fix * PCM_MAX_NEG_VAL)) & (0xFFFFFF00);
    } else {
      out_val =
          static_cast<smpl_t>((out_val_fix * PCM_MAX_POS_VAL)) & (0xFFFFFF00);
    }

    return out_val;
  }
};

void biquad_DFI(smpl_ppln_t &in_stream, smpl_ppln_t &out_stream,
                volatile bit_t &mclk, bit_t &data_out);

#endif