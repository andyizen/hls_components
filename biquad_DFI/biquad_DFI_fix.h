#ifndef BIQUAD_DFI_FIX_H
#define BIQUAD_DFI_FIX_H

#include "../lib/tdm_system_spec.h"
#include <ap_fixed.h>
#include <stdint.h>

// Constants
const int NUM_DELAYS = 4;

typedef ap_fixed<32, 1, AP_RND, AP_WRAP> smpl_fix_inout_t;
typedef ap_fixed<32, 2, AP_RND, AP_WRAP> smpl_fix32_t;
typedef ap_fixed<36, 2, AP_RND, AP_WRAP> smpl_fix36_t;
typedef ap_fixed<72, 2, AP_RND, AP_WRAP> smpl_fix72_t;

// Optimized for external RAM - 32bit data
typedef smpl_fix32_t coeff_t;
struct FilterCoefficients {
  coeff_t b0;
  coeff_t b1;
  coeff_t b2;
  coeff_t a1;
  coeff_t a2;
};

// Optimized for internal BRAM - 36bit data
typedef smpl_fix36_t dly_t;
struct DelayMemory {
  dly_t m_b1;
  dly_t m_b2;
  dly_t m_a1;
  dly_t m_a2;
};

class Biquad_DFI_fix {
private:
  // Memory elements for state
  DelayMemory &dly;
  const FilterCoefficients &coeff;

public:
  Biquad_DFI_fix(const FilterCoefficients &coeff, DelayMemory &dly);
  smpl_t process(smpl_t in_val);
};

#endif