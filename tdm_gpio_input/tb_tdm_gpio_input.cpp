#include "tb_tdm_gpio_input.h"
#include "ap_int.h"
#include <cmath>
#include <hls_stream.h>
#include <iostream>
#include <stdlib.h>

TDM_signal tdm_in{12};
smpl_ppln_t out_reg;
smpl_t pout_reg;
smpl_t data = 0;

int main() {
  int total_clc_cycles = 550 * MCLKS_PER_BIT;
  for (int i = 0; i < total_clc_cycles; i++) {

    // Set simulation data and CLOCKING the tdm master signal
    tdm_in.master_clk(i, data);

    // Sending the signals into the component
    tdm_gpio_input(tdm_in.sclk, tdm_in.lrclk, tdm_in.sdata, out_reg);
  }
  return 0;
}
