#ifndef BIQUAD_DFII_H
#define BIQUAD_DFII_H

#include "../lib/tdm_system_spec.h"
#include "biquad_DFII_fix.h"

void biquad_DFII(smpl_ppln_t &in_stream, smpl_ppln_t &out_stream,
                 const coeff_t mem_coeff[NUM_CHANNELS * NUM_COEFFS]);

#endif