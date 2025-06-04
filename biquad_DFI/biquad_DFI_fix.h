#ifndef BIQUAD_DFI_FIX_H
#define BIQUAD_DFI_FIX_H

#include "../lib/tdm_system_spec.h"
#include <ap_fixed.h>
#include <stdint.h>

// Constants
const int NUM_DELAYS = 4;

// AP_RND for inout seems to fix the silence issue, that when sent nothing the
// output is still something very small negative
// -still takes Takes a couple more resources for LUT and FF
typedef ap_fixed<48, 24, AP_RND, AP_WRAP> smpl_fix_inout_t;
typedef ap_fixed<32, 2, AP_TRN, AP_WRAP> smpl_fix32_t;
typedef ap_fixed<72, 2, AP_TRN, AP_WRAP> smpl_fix72_t;

constexpr int32_t float_to_q2_30(float x) {
  const int64_t Q2_30_SCALE = 1LL << 30;
  return static_cast<int32_t>(
      (x >= 2.0f ? 1.999999f : (x < -2.0f ? -2.0f : x)) * Q2_30_SCALE);
}

struct FilterCoefficients {
  smpl_fix32_t b0;
  smpl_fix32_t b1;
  smpl_fix32_t b2;
  smpl_fix32_t a1;
  smpl_fix32_t a2;
};
struct DelayMemory {
  smpl_fix72_t m_b1;
  smpl_fix72_t m_b2;
  smpl_fix72_t m_a1;
  smpl_fix72_t m_a2;
};

class Biquad_DFI_fix {
private:
  // Memory elements for state
  DelayMemory dly;
  FilterCoefficients coeff;

public:
  Biquad_DFI_fix() {}
  smpl_t process(smpl_t in_val);
  void set_coeff(const FilterCoefficients &new_coeff);
  void set_dly(DelayMemory &new_dly);
  DelayMemory get_dly();
};

#endif