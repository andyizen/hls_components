#ifndef IIR_FILTER_I
#define IIR_FILTER_I

#include "../lib/tdm_system_spec.h"
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
  ap_uint<4> m_b1;
  ap_uint<4> m_b2;
  ap_uint<4> m_a1;
  ap_uint<4> m_a2;
  FilterFactors factors;

public:
  Biquad() : factors({1, 1, 1, 1, 1}), m_b1(0), m_b2(0), m_a1(0), m_a2(0) {}

  ap_uint<4> process(ap_uint<4> in_val) {
#pragma HLS INLINE off
    ap_uint<4> out_val = in_val * factors.b0 + m_b1 * factors.b1 +
                         m_b2 * factors.b2 - m_a1 * factors.a1 -
                         m_a2 * factors.a2;
    m_b2 = m_b1;
    m_b1 = in_val;
    m_a2 = m_a1;
    m_a1 = out_val;
    return out_val;
  }
};

void iir_filter_I(ap_uint<64> in_stream, ap_uint<64> &out_stream);

#endif