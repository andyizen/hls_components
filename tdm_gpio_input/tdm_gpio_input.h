#ifndef _TDM_GPIO_INPUT_H_
#define _TDM_GPIO_INPUT_H_

#include "../lib/tdm_system_spec.h"

void tdm_gpio_input(bit_t sclk_in, bit_t lrclk_in, bit_t sdata_in,
                    smpl_ppln_t &sample_out);

#endif