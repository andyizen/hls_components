#include "tdm_gpio_output.h"
#include <hls_stream.h>
#include <iostream>
#include <time.h>

// For our test, we define a simple test frame.
// For example, we pack channel values (here, simply the channel index) into a
// frame.
int main() {
  hls::stream<sample_t> in_stream;
  hls::stream<sample_t> out_stream;
  sample_t out_sample = 0;

  // Write the test frame into the input stream.
  // Variables to simulate external signals.
  bit_t sclk;
  bit_t lrclk;
  bit_t sdata;
  bit_t smpl_rdy;
  bit_t frm_rdy;
  bit_t start;

  int bit_index;
  int test_smpl_counter = 0;
  bool old_lrclk;
  int smpl_cntr = 0;
  int bit_cntr = 0;
  bool sampled = false;
  bool clocked = false;
  bool pre_sync = true;
  int sclks = 0;

  const int NUM_TEST_SAMPLES = 34;
  const int NUM_TEST_MCKLS = NUM_TEST_SAMPLES * CHANNEL_SIZE * CLKS_PER_BIT;
  const int NUM_PRESYNC_MCLKS = NUM_CHANNELS * CHANNEL_SIZE * CLKS_PER_BIT;

  // Take the Presync time 2 to have an afterglow once all samples are through
  const int total_mclks = NUM_TEST_MCKLS + 2 * NUM_PRESYNC_MCLKS;

  // Create test samples
  sample_t test_smpls[NUM_TEST_SAMPLES];
  for (int j = 0; j < NUM_TEST_SAMPLES; j++) {
    test_smpls[j] = j + 1;
  }

  // We'll simulate clock cycles.
  for (int mclks = 0; mclks < total_mclks; mclks++) {

    // CLOCKING SCLK
    if (mclks % CLKS_PER_BIT == 0 && mclks != 0) {
      sclks++;
      clocked = true;
    }

    // SCLK CLOCK ACTIONS
    // WRITE FIFO BEFORE WE START READ
    if (clocked) {
      // Write Stream before we get to an end of a sample
      if ((sclks % CHANNEL_SIZE) == (CHANNEL_SIZE - 3)) {
        if (test_smpl_counter < NUM_TEST_SAMPLES && !pre_sync) {
          in_stream.write(test_smpls[test_smpl_counter]);
          test_smpl_counter++;
        } else {
          in_stream.write((sample_t)0);
        }
      }
      // Write sample ready
      if ((sclks % CHANNEL_SIZE) == 0) {
        smpl_rdy = 1;
        // For the presync
        if (smpl_cntr == NUM_CHANNELS - 2) {
          pre_sync = false;
        }
        if (smpl_cntr == NUM_CHANNELS - 1) {
          smpl_cntr = 0;
        } else {
          smpl_cntr++;
        }
      } else {
        smpl_rdy = 0;
      }

      if (smpl_cntr == 0 && smpl_rdy == 1) {
        frm_rdy = 1;
      } else {
        frm_rdy = 0;
      }

      clocked = false;
    }
    sample_t in_reg = 0;
    if (!pre_sync)
      in_reg = test_smpls[test_smpl_counter];
    tdm_gpio_output(in_reg, smpl_rdy, frm_rdy, sclk, lrclk, sdata, start);

    // CAPTURING PART
    if (sclk & !sampled) {
      out_sample = (out_sample << 1) | sdata;
      sampled = true;

      if (bit_cntr == CHANNEL_SIZE - 1) {
        out_stream.write(out_sample);
        bit_cntr = 0;
      } else {
        bit_cntr++;
      }

    } else if (!sclk) {
      sampled = false;
    }
  }

  // Checking results
  int fail_count = 0;
  int recvd_smpls = out_stream.size();
  std::cout << "Received " << out_stream.size() << " samples in our_reg."
            << std::endl;

  for (int i = 0; i < NUM_TEST_SAMPLES; i++) {
    sample_t sample = out_stream.read();
    sample_t test_smpl = test_smpls[i];

    if (sample == test_smpl) {
      std::cout << "SAMPLE " << std::dec << i << ": okay!" << std::endl;
      std::cout << "READ:" << std::hex << sample << " | TEST: " << std::hex
                << test_smpl << std::endl;
    } else {
      std::cout << "SAMPLE " << std::dec << i << ": NOT okay!" << std::endl;
      std::cout << "READ:" << std::hex << sample << " | TEST: " << std::hex
                << test_smpl << std::endl;

      fail_count++;
    }
  }

  // Close down test, make sure all fifos are drained to avaoid warnings
  while (!out_stream.empty()) {
    sample_t junk = out_stream.read();
  }
  std::cout << "OUTSTREAM size: " << out_stream.size() << std::endl;
  while (!in_stream.empty()) {
    sample_t junk = in_stream.read();
  }
  std::cout << "INSTREAM size: " << in_stream.size() << std::endl;

  // return fail_count;
  return 0;
}
