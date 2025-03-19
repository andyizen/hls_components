#include "tdm_gpio_input.h"

void tdm_gpio_input(bit_t sclk_in, bit_t lrclk_in, bit_t sdata_in,
                    sample_t &sample_reg, ap_uint<32> &out_reg) {

#pragma HLS INTERFACE ap_none port = sclk_in
#pragma HLS INTERFACE ap_none port = lrclk_in
#pragma HLS INTERFACE ap_none port = sdata_in
#pragma HLS INTERFACE mode = axis port = sample_reg depth = 1
#pragma HLS pipeline II = 1
#pragma HLS INTERFACE ap_ctrl_none port = return

  enum LockStatus { LOCKED, UNLOCKED };
  // Static variables to preserve state.
  static bit_t old_clk = 0;
  static ap_uint<5> _sclk_cnt_ = 0;
  static LockStatus _sclk_cnt_stt_ = LOCKED;
  static ap_uint<32> _smpl_reg_ = (ap_uint<32>)0;
  static ap_uint<32> _smpl_reg_ltchd_ = (ap_uint<32>)0;
  static bool _frm_sync_stt_ = false;

  if (lrclk_in && sclk_in && !_frm_sync_stt_) {
    _sclk_cnt_ = CHANNEL_SIZE - 1;
    _frm_sync_stt_ = true;
  }
  if (_frm_sync_stt_) {
    // Rising edge detection: if previous clk was 0 and current clk is 1.
    if (sclk_in && _sclk_cnt_stt_ == UNLOCKED) {
      // Writing out complete Sample
      if (_sclk_cnt_ == 0) {
        _smpl_reg_ltchd_ = _smpl_reg_;
        sample_reg.write(_smpl_reg_ltchd_);
      }
      _smpl_reg_ = (_smpl_reg_ << 1) | sdata_in;
      _sclk_cnt_stt_ = LOCKED;
    } else if (!sclk_in && _sclk_cnt_stt_ == LOCKED) {
      _sclk_cnt_++;
      _sclk_cnt_stt_ = UNLOCKED;
    }
  }
  out_reg = _smpl_reg_ltchd_;
}