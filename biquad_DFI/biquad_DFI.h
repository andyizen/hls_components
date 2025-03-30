#ifndef BIQUAD_DFI_H
#define BIQUAD_DFI_H

#include "../lib/tdm_system_spec.h"
#include "ap_float.h"

struct FilterFactors {
  ap_fixed<24, 2> b0;
  ap_fixed<24, 2> b1;
  ap_fixed<24, 2> b2;
  ap_fixed<24, 2> a1;
  ap_fixed<24, 2> a2;
};

class Biquad {
private:
  // Memory elements for state
  smpl_t m_b1;
  smpl_t m_b2;
  smpl_t m_a1;
  smpl_t m_a2;
  FilterFactors factors;

public:
  Biquad() {
    factors.b0 = 4.24433681e-05f;
    factors.b1 = 8.48867363e-05f;
    factors.b2 = 4.24433681e-05f;
    factors.a1 = -1.98148851f;
    factors.a2 = 0.98165828f;
    /*     factors.b0 = 1.0f;
        factors.b1 = 1.0f;
        factors.b2 = 1.0f;
        factors.a1 = 1.0f;
        factors.a2 = 1.0f; */
  }

  smpl_t process(smpl_t in_val) {
    smpl_t rb0, rb1, rb2, ra1, ra2 = 0;

    // Intermediate results
    rb0 = (smpl_t)(in_val * (factors.b0 >> 8));
    rb1 = (smpl_t)(m_b1 * (factors.b1 >> 8));
    rb2 = (smpl_t)(m_b2 * (factors.b2 >> 8));
    ra1 = (smpl_t)(m_a1 * (factors.a1 >> 8));
    ra2 = (smpl_t)(m_a2 * (factors.a2 >> 8));

    // Addition
    smpl_t out_val = ((rb0 + rb1 + rb2 - ra1 - ra2) << 8) & (0xFFFFFF00);

    // Updating memory
    m_b2 = m_b1;
    m_b1 = in_val;
    m_a2 = m_a1;
    m_a1 = out_val;
    return out_val;
  }
};

void biquad_DFI(smpl_ppln_t &in_stream, smpl_ppln_t &out_stream);

#endif