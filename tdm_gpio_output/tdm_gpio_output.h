#ifndef _TDM_GPIO_OUTPUT_H_
#define _TDM_GPIO_OUTPUT_H_

#include "../lib/tdm_system_spec.h"

void tdm_gpio_output(smpl_ppln_t &sample_in, bit_t &sclk, bit_t &lrclk,
                     bit_t &sdata);

#endif