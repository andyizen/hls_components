#include "tb_biquad_DFI.h"

TDM_signal_dummy tdm_in{12};
smpl_t data = 0;
smpl_ppln_t in_reg;
smpl_ppln_t out_reg;

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
      in_reg.write(data);
      std::cout << "size(in_reg): " << in_reg.size() << "\t";
      std::cout << "in:" << std::hex << data << "\t";
      std::cout << "mclk: " << std::dec << i << std::endl;
      lck_new_data = true;
    } else if (tdm_in.bit_index > 0) {
      lck_new_data = false;
    }

    // Sending the signals into the component
    biquad_DFI(in_reg, out_reg);

    // LRCLK CHECK
    if (!out_reg.empty()) {
      std::cout << "size(out_reg):" << out_reg.size() << "\t";
      std::cout << "out:" << std::hex << out_reg.read() << "\t";
      std::cout << "mclk: " << std::dec << i << std::endl;
    }
  }
  return 0;
}