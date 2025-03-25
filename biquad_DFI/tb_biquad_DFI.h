#ifndef TB_BIQUAD_DFI_H
#define TB_BIQUAD_DFI_H

#include "../lib/tdm_signal_base.h"
#include "biquad_DFI.h"

static smpl_t default_data = 0;

class TDM_signal_dummy : public TDM_signal_base {
public:
  TDM_signal_dummy(int start_offset) : TDM_signal_base(start_offset) {}

private:
  smpl_t *_data = 0;

public:
  void master_clk(int cur_clk_cycle, smpl_t &data = default_data) override {
    updateCounters(cur_clk_cycle);
  };
};
#endif
