#ifndef TDM_GPIO_OUTPUT_H
#define TDM_GPIO_OUTPUT_H

#include "../lib/tdm_system_spec.h"

void tdm_gpio_output(smpl_ppln_t &in_stream, bit_t &sclk, bit_t &lrclk,
                     bit_t &sdata);

#endif