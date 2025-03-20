#include "../lib/tdm_signal_base.h"
#include "tdm_gpio_input.h"

class TDM_signal : public TDM_signal_base {
public:
  bit_t sclk;
  bit_t lrclk;
  bit_t sdata;

  TDM_signal(int start_offset)
      : TDM_signal_base(start_offset), sclk(0), lrclk(0), sdata(0) {}

private:
  void updateClocks(int cur_clk_cycle) {
    if (cur_clk_cycle % MCLKS_PER_BIT == 0) {
      sclk = 0;
    } else if (cur_clk_cycle % MCLKS_PER_BIT == MCLKS_PER_BIT / 2) {
      sclk = 1;
    }

    if (sample_index == NUM_CHANNELS - 1 && bit_index == SIZE_SAMPLE - 1) {
      lrclk = 1;
    } else {
      lrclk = 0;
    }
  };
  void processData(int cur_clk_cycle, smpl_t &data) {
    // Process the data based on the state of data_lock.
    if (!lrclk_seen) {
      sdata = (bit_index + sample_index) % 2;
    } else {
      sdata = data[SIZE_SAMPLE - 1 - bit_index];
    }
  }
  void simulation_data(smpl_t &data_container) {
    static bool rdy_for_next_sample = true;
    if (lrclk_seen) {
      if (!bit_index && rdy_for_next_sample) {
        // Here we can set any new sample
        data_container = (data_container + 1) | (smpl_t)0;
        rdy_for_next_sample = false;
      } else if (bit_index > 0) {
        rdy_for_next_sample = true;
      }
    }
  }

public:
  void master_clk(int cur_clk_cycle, smpl_t &data) override {
    simulation_data(data);
    updateCounters(cur_clk_cycle);
    processData(cur_clk_cycle, data);
    updateClocks(cur_clk_cycle);
  };
};