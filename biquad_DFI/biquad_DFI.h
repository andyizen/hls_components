#ifndef BIQUAD_DFI_H
#define BIQUAD_DFI_H

#include "../lib/tdm_system_spec.h"
#include "ap_float.h"
#include "biquad_DFI_fix.h"

const int NUM_OF_DELAYS = 4;

void biquad_DFI(smpl_ppln_t &in_stream, smpl_ppln_t &out_stream,
                const FilterCoefficients coeff[NUM_CHANNELS],
                smpl_fix_t mem[NUM_CHANNELS * NUM_OF_DELAYS]);

#endif