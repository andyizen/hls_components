#include "tdm_system.h"

void tdm_system(const bit_t sclk_in, const bit_t lrclk_in, const bit_t sdata_in,
                bit_t &sclk_out, bit_t &lrclk_out, bit_t &sdata_out) {

#pragma HLS INTERFACE mode = ap_ctrl_none port = return
#pragma HLS INTERFACE ap_none port = sdata_in
#pragma HLS INTERFACE ap_none port = sclk_in
#pragma HLS INTERFACE ap_none port = lrclk_in
#pragma HLS INTERFACE ap_none port = sdata_out
#pragma HLS INTERFACE ap_none port = sclk_out
#pragma HLS INTERFACE ap_none port = lrclk_out

#pragma HLS PIPELINE II = 1
  ap_uint<5> sclk_cnt = 0;
  ap_uint<4> smpl_cnt = 0;
  bit_t smpl_rdy = 0;
  bit_t frame_rdy = 0;
  bit_t started = 0;
  ap_uint<32> data_out = 0;

  tdm_gpio_input(sclk_in, lrclk_in, sdata_in, sclk_cnt, smpl_rdy, smpl_cnt,
                 frame_rdy, data_out);
  tdm_gpio_output(data_out, smpl_rdy, frame_rdy, sclk_out, lrclk_out, sdata_out,
                  started);
}