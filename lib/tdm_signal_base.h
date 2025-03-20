#include "ap_int.h"

#ifndef TDM_SIGNAL_BASE_H
#define TDM_SIGNAL_BASE_H

#include "../lib/tdm_system_spec.h"

class TDM_signal_base {
public:
  int start_offset;
  int bit_index;
  int sample_index;
  bool lrclk_seen;

  TDM_signal_base(int start_offset)
      : start_offset(start_offset), bit_index(start_offset % SIZE_CHANNEL),
        sample_index(start_offset % NUM_CHANNELS), lrclk_seen(false) {}

public:
  virtual void master_clk(int cur_clk_cycle, sample_t &data) = 0;

protected:
  void updateCounters(int cur_clk_cycle) {
    if (cur_clk_cycle % MCLKS_PER_BIT == 0) {
      if (bit_index == SIZE_CHANNEL - 1)
        bit_index = 0;
      else
        ++bit_index;

      // When a channel completes, update sample index.
      if (bit_index == 0) {
        if (sample_index == NUM_CHANNELS - 1) {
          sample_index = 0;
          lrclk_seen = true;
        } else {
          ++sample_index;
        }
      }
    }
  }
};

#endif