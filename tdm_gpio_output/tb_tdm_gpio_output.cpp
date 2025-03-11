#include "tdm_gpio_output.h"
#include <hls_stream.h>
#include <iostream>

// For our test, we define a simple test frame.
// For example, we pack channel values (here, simply the channel index) into a
// frame.
int main() {
  hls::stream<frame_t> in_stream;
  frame_t test_frame = 0;
  frame_t check_frame = 0;

  // Create a test frame with 16 channels, each 32 bits.
  // For simplicity, let channel j be equal to j.
  for (int j = 0; j < NUM_CHANNELS; j++) {
    ap_uint<CHANNEL_SIZE> channel_value = j;
    test_frame = (test_frame << CHANNEL_SIZE) | channel_value;
  }
  // Write the test frame into the input stream.

  // Variables to simulate external signals.
  bit_t sclk;
  bit_t lrclk;
  bit_t sdata;

  int bit_index;
  bool old_lrclk;
  bool cur_lrclk;

  // We'll simulate clock cycles.
  const int total_cycles = (FRAME_SIZE * 8) * 2.5;
  for (int cycle = 0; cycle < total_cycles; cycle++) {

    if (cycle == 50)
      in_stream.write(test_frame);

    if (cycle == 500)
      in_stream.write(test_frame);

    tdm_gpio_output(&sclk, &lrclk, &sdata, in_stream);

    if (bit_index % 8 == 0) {
      check_frame = (check_frame << 1) | sdata;
    }

    cur_lrclk = lrclk;
    if (cur_lrclk == false && old_lrclk == true) {
      bit_index = 0;
      std::cout << "FRAME " << std::hex << test_frame << std::endl;
      std::cout << "CHECK " << std::hex << check_frame << std::endl;
    } else {
      bit_index++;
    }

    old_lrclk = cur_lrclk;
  }

  return 0;
}
