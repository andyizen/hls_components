#include "tb_tdm_system.h"
#include "ap_int.h"
#include "tdm_system.h"
#include <hls_stream.h>
#include <iostream>
#include <stdlib.h>

TDM_signal tdm_in{12};
sample_t out_reg;
ap_uint<32> pout_reg;
ap_uint<32> data = 0;

bit_t sclk_out;
bit_t lrclk_out;
bit_t sdata_out;
bool lck_new_data = false;

int main() {
  int total_clc_cycles = 1200 * CLKS_PER_BIT;
  for (int i = 0; i < total_clc_cycles; i++) {

    // Set simulation data and CLOCKING the tdm master signal
    tdm_in.master_clk(i, data);

    // Sending the signals into the component
    tdm_system(tdm_in.sclk, tdm_in.lrclk, tdm_in.sdata, sclk_out, lrclk_out,
               sdata_out);
  }
  return 0;
}
