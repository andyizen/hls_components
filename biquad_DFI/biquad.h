#ifndef BIQUAD_DFI_H
#define BIQUAD_DFI_H

#include "../lib/tdm_system_spec.h"
#include "ap_float.h"
#include "biquad_DFI_fix.h"

const int NUM_COEFFS = 5;

void biquad_DFI(smpl_ppln_t &in_stream, smpl_ppln_t &out_stream,
                const smpl_fix32_t coeff[NUM_CHANNELS * NUM_COEFFS]);

#endif