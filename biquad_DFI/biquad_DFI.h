#ifndef BIQUAD_DFI_H
#define BIQUAD_DFI_H

#include "../lib/tdm_system_spec.h"
#include "ap_float.h"

struct FilterFactors {
  ap_uint<16> b0;
  ap_uint<16> b1;
  ap_uint<16> b2;
  ap_uint<16> a1;
  ap_uint<16> a2;
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
  Biquad()
      : factors(FilterFactors{1, 1, 1, 1, 1}), m_b1(0), m_b2(0), m_a1(0),
        m_a2(0) {}

  smpl_t process(smpl_t in_val) {
#pragma HLS INLINE off
    smpl_t out_val = in_val * factors.b0 + m_b1 * factors.b1 +
                     m_b2 * factors.b2 - m_a1 * factors.a1 - m_a2 * factors.a2;
    m_b2 = m_b1;
    m_b1 = in_val;
    m_a2 = m_a1;
    m_a1 = out_val;
    return out_val;
  }
};

void biquad_DFI(smpl_ppln_t &in_stream, smpl_ppln_t &out_stream);

#endif