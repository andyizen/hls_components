#include "tdm_system.h"

void tdm_system(const bit_t sclk_in, const bit_t lrclk_in, const bit_t sdata_in,
                bit_t &sclk_out, bit_t &lrclk_out, bit_t &sdata_out) {

  smpl_ppln_t tdm_out;
  smpl_ppln_t filter_out;

#pragma HLS PIPELINE II = 1
#pragma HLS LATENCY max = 64

  tdm_gpio_input(sclk_in, lrclk_in, sdata_in, tdm_out);
  biquad_DFI(tdm_out, filter_out);
  tdm_gpio_output(filter_out, sclk_out, lrclk_out, sdata_out);
}