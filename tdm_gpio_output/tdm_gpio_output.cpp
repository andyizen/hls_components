#include "tdm_gpio_output.h"

void tdm_gpio_output(sample_t &in_stream, bit_t &sclk, bit_t &lrclk,
                     bit_t &sdata) {

#pragma HLS INTERFACE mode = axis port = in_stream depth = 1
#pragma HLS INTERFACE mode = ap_none port = sclk
#pragma HLS INTERFACE mode = ap_none port = lrclk
#pragma HLS INTERFACE mode = ap_none port = sdata
#pragma HLS INTERFACE ap_ctrl_none port = return

  enum LockStatus { UNLOCKED, LOCKED };

  // Static variables to hold state between cycles.
  static ap_uint<32> _smpl_reg = 0;
  static ap_uint<5> _bit_cnt_ = CHANNEL_SIZE - 1;
  static ap_uint<2> _clk_cnt_ = 0;
  static ap_uint<4> _smpl_cnt_ = NUM_CHANNELS - 2;

  static bool _sclk_stt_ = 0;
  static bool _lrclk_stt_ = 0;
  static bool _sdata_stt_ = 0;
  static bool _first_smpl_read = false;
  static LockStatus _read_stt_ = UNLOCKED;

  // Read from the input stream when a new sample is ready
  if (!in_stream.empty() && _read_stt_ == UNLOCKED) {
    _read_stt_ = LOCKED;
    _smpl_reg = in_stream.read();
    _first_smpl_read = true;
  }

  if (_first_smpl_read) {
    // Generate the bit clock (sclk) waveform
    if (!(_clk_cnt_ % CLKS_PER_BIT)) {
      // SCLK
      _sclk_stt_ = 0;

      // CNTR
      _bit_cnt_++;
      if (_bit_cnt_ == 0) {
        _smpl_cnt_++;
      }

      // LRCLK
      if (_smpl_cnt_ == NUM_CHANNELS - 1 && _bit_cnt_ == CHANNEL_SIZE - 1) {
        _lrclk_stt_ = 1;
      } else {
        _lrclk_stt_ = 0;
      }

      // SDATA
      _sdata_stt_ = (_smpl_reg >> (CHANNEL_SIZE - 1 - _bit_cnt_)) & 1;

    } else if ((_clk_cnt_ % CLKS_PER_BIT) == (CLKS_PER_BIT / 2)) {
      _sclk_stt_ = 1;
      // Reset LOCKS
      if (_bit_cnt_ == CHANNEL_SIZE - 1) {
        _read_stt_ = UNLOCKED;
      }
    }
  } else {
    _sclk_stt_ = 0;
    _sdata_stt_ = 0;
    _lrclk_stt_ = 0;
  }

  _clk_cnt_++;
  sclk = _sclk_stt_;
  sdata = _sdata_stt_;
  lrclk = _lrclk_stt_;
}
