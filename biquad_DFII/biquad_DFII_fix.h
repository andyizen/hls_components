#ifndef Biquad_DFII_fix_H
#define Biquad_DFII_fix_H

#include "../lib/tdm_system_spec.h"
#include <ap_fixed.h>
#include <stdint.h>

// Constants
const int NUM_DELAYS = 2;
const int NUM_COEFFS = 5;

typedef ap_fixed<32, 1, AP_RND, AP_WRAP> smpl_fix_inout_t;
typedef ap_fixed<32, 2, AP_RND, AP_WRAP> smpl_fix32_t;
typedef ap_fixed<36, 8, AP_RND, AP_WRAP> smpl_fix36_t;
typedef ap_fixed<72, 8, AP_RND, AP_WRAP> smpl_fix72_t;

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
  dly_t m_w1;
  dly_t m_w2;
};

class Biquad_DFII_fix {
private:
  // Memory elements for state
  DelayMemory dly;
  FilterCoefficients coeff;

public:
  Biquad_DFII_fix();
  smpl_t process(smpl_t in_val);
  void update_dly(dly_t mem_dly[NUM_CHANNELS * NUM_DELAYS], ch_cntr_t _ch_cnt_);
  void set_dly(dly_t mem_dly[NUM_CHANNELS * NUM_DELAYS], ch_cntr_t _ch_cnt_);
  void set_coeff(coeff_t mem_coeff[NUM_CHANNELS * NUM_COEFFS],
                 ch_cntr_t _ch_cnt_);
};

#endif