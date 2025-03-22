#include "tdm_system.h"

void tdm_system(const bit_t sclk_in, const bit_t lrclk_in, const bit_t sdata_in,
                bit_t &sclk_out, bit_t &lrclk_out, bit_t &sdata_out) {

#pragma HLS INTERFACE ap_ctrl_none port = return
#pragma HLS INTERFACE ap_none port = sdata_in
#pragma HLS INTERFACE ap_none port = sclk_in
#pragma HLS INTERFACE ap_none port = lrclk_in
#pragma HLS INTERFACE ap_none port = sdata_out
#pragma HLS INTERFACE ap_none port = sclk_out
#pragma HLS INTERFACE ap_none port = lrclk_out

  static smpl_ppln_t tdm_out;
  static smpl_ppln_t filter_out;

#pragma HLS DATAFLOW
  tdm_gpio_input(sclk_in, lrclk_in, sdata_in, tdm_out);
  iir_filter_I(tdm_out, filter_out);
  tdm_gpio_output(filter_out, sclk_out, lrclk_out, sdata_out);
}