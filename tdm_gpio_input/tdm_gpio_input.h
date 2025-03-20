#ifndef TDM_COUNT_H
#define TDM_COUNT_H

#include "../lib/tdm_system_spec.h"

void tdm_gpio_input(bit_t sclk_in, bit_t lrclk_in, bit_t sdata_in,
                    sample_pipe_t &sample_reg);

#endif