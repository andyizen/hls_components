#include "tdm_gpio_input.h"

void tdm_gpio_input(const bit_t clk_in, const bit_t lrclk_in,
                    const bit_t sdata_in, ap_uint<5> &sclk_cnt, bit_t &sclk_rdy,
                    ap_uint<4> &sample_cnt, bit_t &sample_rdy,
                    ap_uint<32> &sample_reg) {
#pragma HLS PIPELINE II = 1
#pragma HLS INTERFACE ap_none port = clk_in
#pragma HLS INTERFACE ap_none port = lrclk_in
#pragma HLS INTERFACE ap_none port = sdata_in
#pragma HLS INTERFACE ap_none port = sclk_cnt
#pragma HLS INTERFACE ap_none port = sclk_rdy
#pragma HLS INTERFACE ap_none port = sample_cnt
#pragma HLS INTERFACE ap_none port = sample_rdy
#pragma HLS INTERFACE ap_none port = sample_reg
#pragma HLS INTERFACE mode = ap_ctrl_none port = return

  enum Edge { WAS_HI, WAS_LO };
  // Static variables to preserve state.
  static bit_t old_clk = 0;
  static ap_uint<5> _sclk_cnt = 0;
  static ap_uint<4> _smpl_cnt = 0;
  static ap_uint<32> _smpl_reg = 0;
  static ap_uint<32> _smpl_out_reg_ = 0;

  static Edge _sclk_edge_stt;
  static bool _smpl_rdy_stt;
  static bool _sclk_rdy_stt;

  // Rising edge detection: if previous clk was 0 and current clk is 1.
  if (clk_in && _sclk_edge_stt == WAS_LO) {
    _smpl_reg = (_smpl_reg << 1) | sdata_in;
    if (lrclk_in == 1) {
      _sclk_cnt = 31;
      _smpl_cnt = 15;
    } else {
      _sclk_cnt++;
    }

    if (_sclk_cnt == 0) {
      _smpl_cnt++;
    }

    if (_sclk_cnt > 0 && _sclk_cnt <= 2) {
      _sclk_rdy_stt = 1;
    } else {
      _sclk_rdy_stt = 0;
    }

    // we set sample_ready HIGH if the first sample is through
    // therefore at _smpl_cnt == 1
    if (_sclk_cnt > 0 && _sclk_cnt <= 2 && _smpl_cnt == 1) {
      _smpl_rdy_stt = 1;
    } else {
      _smpl_rdy_stt = 0;
    }

    _sclk_edge_stt = WAS_HI;

  } else if (!clk_in && _sclk_edge_stt == WAS_HI) {
    if (_sclk_cnt == 31) {
      _smpl_out_reg_ = _smpl_reg;
    }
    _sclk_edge_stt = WAS_LO;
  }

  // Output the current sclk_count value.
  sclk_rdy = _sclk_rdy_stt;
  sample_rdy = _smpl_rdy_stt;
  sample_reg = _smpl_out_reg_;
  // Update old clock state.
  sclk_cnt = _sclk_cnt;
  sample_cnt = _smpl_cnt;
}
