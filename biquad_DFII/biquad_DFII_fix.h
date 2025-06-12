#ifndef Biquad_DFII_fix_H
#define Biquad_DFII_fix_H

#include "../lib/tdm_system_spec.h"
#include <ap_fixed.h>
#include <stdint.h>

// Constants
const int NUM_DELAYS = 2;

// AP_RND for inout seems to fix the silence issue, that when sent nothing the
// output is still something very small negative
// -still takes Takes a couple more resources for LUT and FF
typedef ap_fixed<32, 1, AP_TRN, AP_WRAP> smpl_fix_inout_t;
typedef ap_fixed<36, 8, AP_TRN, AP_WRAP> smpl_fix32_t;
typedef ap_fixed<72, 8, AP_TRN, AP_WRAP> smpl_fix72_t;

constexpr int32_t float_to_q2_30(float x) {
  const int64_t Q2_30_SCALE = 1LL << 30;
  return static_cast<int32_t>(
      (x >= 2.0f ? 1.999999f : (x < -2.0f ? -2.0f : x)) * Q2_30_SCALE);
}

typedef smpl_fix32_t coeff_t;
struct FilterCoefficients {
  coeff_t b0;
  coeff_t b1;
  coeff_t b2;
  coeff_t a1;
  coeff_t a2;
};

typedef smpl_fix32_t dly_t;
struct DelayMemory {
  dly_t w1;
  dly_t w2;
};

class Biquad_DFII_fix {
private:
  // Memory elements for state
  DelayMemory &dly;
  const FilterCoefficients &coeff;

public:
  Biquad_DFII_fix(const FilterCoefficients &coeff, DelayMemory &dly);
  smpl_t process(smpl_t in_val);
};

#endif