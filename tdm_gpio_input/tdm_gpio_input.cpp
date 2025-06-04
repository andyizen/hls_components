#include "tdm_gpio_input.h"

void tdm_gpio_input(bit_t sclk_in, bit_t lrclk_in, bit_t sdata_in,
                    smpl_ppln_t &sample_out) {

#pragma HLS INTERFACE mode = axis port = sample_out depth = 1
#pragma HLS INTERFACE mode = ap_ctrl_none port = return
#pragma HLS PIPELINE II = 1
  enum TDM_IN_STATUS { WAIT, READ, COUNT };
  // Static variables to preserve state.
  static bit_t old_clk = 0;
  static ap_uint<CNT_BIT_DEPTH_SIZE_SAMPLE> _sclk_cnt_ = 0;
  static TDM_IN_STATUS _cmpnt_stt_ = WAIT;
  static smpl_t _smpl_reg_ = (smpl_t)0;
  static smpl_t _smpl_reg_ltchd_ = (smpl_t)0;

  if (lrclk_in && sclk_in && _cmpnt_stt_ == WAIT) {
    _sclk_cnt_ = SIZE_SAMPLE - 1;
    _cmpnt_stt_ = READ;
  }
  if (_cmpnt_stt_ != WAIT) {
    // Rising edge detection: if previous clk was 0 and current clk is 1.
    if (sclk_in && _cmpnt_stt_ == READ) {
      // Writing out complete Sample
      if (_sclk_cnt_ == 0) {
        _smpl_reg_ltchd_ = _smpl_reg_;
        sample_out.write(_smpl_reg_ltchd_);
      }
      _smpl_reg_ = (_smpl_reg_ << 1) | sdata_in;
      _cmpnt_stt_ = COUNT;
    } else if (!sclk_in && _cmpnt_stt_ == COUNT) {
      _sclk_cnt_++;
      _cmpnt_stt_ = READ;
    }
  }
}