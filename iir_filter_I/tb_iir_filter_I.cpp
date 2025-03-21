#include "iir_filter_I.h"

ap_uint<64> in_reg = 0x1111111111111111;
ap_uint<64> out_reg = 0;
ap_uint<64> res_reg = 0x0000FFFF0001FFFF;

int main() {
  for (int i = 0; i < 10; i++) {
    iir_filter_I(in_reg, out_reg);
    std::cout << std::hex << out_reg << std::endl;
  }
  return 0;
}