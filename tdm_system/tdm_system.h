#ifndef TDM_SYSTEM_H
#define TDM_SYSTEM_H

#include "../biquad_DFI/biquad_DFI_fix.h"
#include "../lib/tdm_system_spec.h"
#include "../tdm_gpio_input/tdm_gpio_input.h"
#include "../tdm_gpio_output/tdm_gpio_output.h"

void tdm_system(bit_t sclk_in, bit_t lrclk_in, bit_t sdata_in, bit_t &sclk_out,
                bit_t &lrclk_out, bit_t &sdata_out);

#endif
