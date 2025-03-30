#include "tdm_system.h"

void tdm_system(const bit_t sclk_in, const bit_t lrclk_in, const bit_t sdata_in,
                bit_t &sclk_out, bit_t &lrclk_out, bit_t &sdata_out) {

#pragma HLS INTERFACE mode = ap_ctrl_none port = return
#pragma HLS INTERFACE ap_none port = sclk_out
#pragma HLS INTERFACE ap_none port = lrclk_out
#pragma HLS INTERFACE ap_none port = sdata_out
  smpl_ppln_t tdm_out;
  smpl_ppln_t filter_out;

#pragma HLS STREAM variable = tdm_out depth = 2 type = fifo
#pragma HLS STREAM variable = filter_out depth = 2 type = fifo

#pragma HLS DATAFLOW
  tdm_gpio_input(sclk_in, lrclk_in, sdata_in, tdm_out);
  biquad_DFI(tdm_out, filter_out);
  tdm_gpio_output(filter_out, sclk_out, lrclk_out, sdata_out);
}