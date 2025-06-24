#ifndef BIQUAD_DFI_H
#define BIQUAD_DFI_H

#include "../lib/tdm_system_spec.h"
#include "ap_float.h"
#include "biquad_DFI_float.h"

void biquad_DFI_float(smpl_ppln_t &in_stream, smpl_ppln_t &out_stream,
                      coeff_t mem_coeff[NUM_CHANNELS * NUM_COEFFS]);

#endif