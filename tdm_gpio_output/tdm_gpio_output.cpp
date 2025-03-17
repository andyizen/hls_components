#include "tdm_gpio_output.h"

void tdm_gpio_output(sample_t &in_stream, bit_t sample_rdy, bit_t frame_rdy,
                     bit_t &sclk, bit_t &lrclk, bit_t &sdata, bit_t &started) {
#pragma HLS INTERFACE mode = ap_none port = sclk
#pragma HLS INTERFACE mode = ap_none port = lrclk
#pragma HLS INTERFACE mode = ap_none port = sdata
#pragma HLS INTERFACE mode = ap_none port = started
#pragma HLS INTERFACE mode = ap_none port = in_stream

#pragma HLS INTERFACE mode = ap_ctrl_none port = return
  enum CountLock { UNLOCKED, LOCKED };
  enum FrameState { WAIT_FOR_NEW, NEW };

  // Static variables to hold state between cycles.
  static sample_t _smpl_reg = 0;   // Holds the current frame.
  static sample_t out_reg = 0;     // Holds the current frame.
  static ap_uint<5> _bit_cnt_ = 0; // Index for the bit currently being output.
  static CountLock _bit_cnt_lck = UNLOCKED;
  static ap_uint<11> _clk_cnt_ = 0;
  static ap_uint<4> _smpl_cnt_ = 0; // Channel counter

  static FrameState _frm_stt_ = WAIT_FOR_NEW;
  static bool _strt_stt_ = false;
  static bool _sclk_stt_ = 0;
  static bool _lrclk_stt_ = 0;
  static bool _sdata_stt_ = 0;

  // Read from the input stream when a new sample is ready
  if (sample_rdy) {
    if (frame_rdy && _frm_stt_ == WAIT_FOR_NEW) {
      _strt_stt_ = true;
      _bit_cnt_ = 0;
      _smpl_cnt_ = 0;
      _clk_cnt_ = 0;
      _frm_stt_ = NEW;
    } else if (!frame_rdy) {
      _frm_stt_ = WAIT_FOR_NEW;
    }

    _smpl_reg = in_stream;
  }

  if (_strt_stt_) {
    // Generate the bit clock (sclk) waveform
    if ((_clk_cnt_ % CLKS_PER_BIT) < (CLKS_PER_BIT / 2) &&
        _bit_cnt_lck == UNLOCKED) {
      _sclk_stt_ = 0;
      _bit_cnt_lck = LOCKED;
      _sdata_stt_ = (_smpl_reg >> (CHANNEL_SIZE - 1 - _bit_cnt_)) & 1;

      // LRCLK
      if (_smpl_cnt_ == NUM_CHANNELS - 1 && _bit_cnt_ == CHANNEL_SIZE - 1) {
        _lrclk_stt_ = 1;
        // Reset channel index when frame completes
      } else {
        _lrclk_stt_ = 0;
      }
    } else if ((_clk_cnt_ % CLKS_PER_BIT) >= (CLKS_PER_BIT / 2) &&
               _bit_cnt_lck == LOCKED) {
      if (_bit_cnt_ == CHANNEL_SIZE - 1) {
        _smpl_cnt_++;
      }
      _sclk_stt_ = 1;
      _bit_cnt_lck = UNLOCKED;
      _bit_cnt_++;
    }

    _clk_cnt_++;
    sclk = _sclk_stt_;
    sdata = _sdata_stt_;
    lrclk = _lrclk_stt_;

  } else {
    // Reset outputs and internal counters when not in active transmission
    sclk = 0;
    lrclk = 0;
    sdata = 0;
    _bit_cnt_ = 0;
    _clk_cnt_ = 0;
    _smpl_cnt_ = 0;
  }
  started = _strt_stt_;
}
