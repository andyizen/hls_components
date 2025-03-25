
#include "prod_cons.h"
#include <iostream>
#include <random>

bit_t sclk;
bit_t data_in;
bit_t last_sclk;
ap_uint<16> pout = {0};

int main() {
  // Initialize random number generator

  for (int i = 0; i < 5 * 16; i++) {
    if (i % 16) {
      prod_cons(pout);
    }
  }

  return 0;
}