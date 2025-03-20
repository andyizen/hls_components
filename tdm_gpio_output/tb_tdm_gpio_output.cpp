
#include "tb_tdm_gpio_output.h"
#include "ap_int.h"
#include "tdm_gpio_output.h"
#include <cmath>
#include <hls_stream.h>
#include <iostream>
#include <stdlib.h>

// Assume the following constants and types are defined elsewhere:
// const int SIZE_CHANNEL;
// const int NUM_CHANNELS;
// const int MCLKS_PER_BIT;
// using bit_t = int; // or a specific type

// Base class defining the template method

TDM_signal_dummy tdm_in{12};
sample_t data = 0;
sample_pipe_t in_reg;

bit_t sclk_out;
bit_t lrclk_out;
bit_t sdata_out;
bool lck_new_data = false;

int main() {
  int total_clc_cycles = 1200 * MCLKS_PER_BIT;
  for (int i = 0; i < total_clc_cycles; i++) {

    tdm_in.master_clk(i);
    if (tdm_in.bit_index == 0 && !lck_new_data) {
      data = data + 1;
      std::cout << "size_a: " << in_reg.size();
      in_reg.write(data);
      std::cout << "size_b: " << in_reg.size() << std::endl;
      lck_new_data = true;
    } else if (tdm_in.bit_index > 0) {
      lck_new_data = false;
    }

    // Sending the signals into the component
    tdm_gpio_output(in_reg, sclk_out, lrclk_out, sdata_out);

    // LRCLK CHECK
    if (lrclk_out)
      std::cout << "I'M HERE!" << std::endl;
  }
  return 0;
}
