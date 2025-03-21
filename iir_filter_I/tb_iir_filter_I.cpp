#include "iir_filter_I.h"

ap_uint<4> in_reg[NUM_CHANNELS] = {0x1};
ap_uint<4> out_reg[NUM_CHANNELS];

int main() {
  for (int i = 0; i < 10; i++) {
    iir_filter_I(in_reg, out_reg);
    std::cout << std::hex << out_reg << std::endl;
  }
  return 0;
}