#include "../lib/tdm_signal_base.h"
#include "tdm_gpio_output.h"

static ap_uint<32> default_data = 0;

class TDM_signal_dummy : public TDM_signal_base {
public:
  TDM_signal_dummy(int start_offset) : TDM_signal_base(start_offset) {}

private:
  ap_uint<32> *_data = 0;

public:
  void master_clk(int cur_clk_cycle,
                  ap_uint<32> &data = default_data) override {
    updateCounters(cur_clk_cycle);
  };
};
