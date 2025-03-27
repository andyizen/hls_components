#include "tb_biquad_DFI.h"
#include <fstream>
#include <iostream>

// Global objects used in the testbench
TDM_signal_dummy tdm_in{12};

smpl_ppln_t in_reg;
smpl_ppln_t out_reg;

bit_t sclk_out;
bit_t lrclk_out;
bit_t sdata_out;
bool lck_new_data = false;
int max_dif = 0;

const std::string test_folder_path =
    "C:/Users/andreas.hettler/Desktop/master_thesis/test";
const std::string f_input_data = test_folder_path + "/ch_data/input.dat";
const std::string f_golden_data =
    test_folder_path + "/results/biquad_DFI_fix/golden.dat";
const std::string f_log_data =
    test_folder_path + "/results/biquad_DFI_fix/result.dat";

int main() {
  // Open the input and golden files from the test folder.
  std::ifstream infile(f_input_data);
  if (!infile) {

    std::cerr << "Error: Could not open test/input.data" << std::endl;

    return 1;
  }

  std::ifstream goldenfile(f_golden_data);
  if (!goldenfile) {
    std::cerr << "Error: Could not open test/golden.dat" << std::endl;
    return 1;
  }

  // Optionally, open an output log file in the test folder.
  std::ofstream outfile(f_log_data);
  if (!outfile) {
    std::cerr << "Error: Could not open test/output.log for writing"
              << std::endl;
    return 1;
  }

  int total_clc_cycles = 48000 * 2 * NUM_CHANNELS;
  int sample_count = 0;
  int error_count = 0;
  smpl_t golden_val;
  smpl_t result;

  for (int i = 0; i < total_clc_cycles; i++) {
    smpl_t data = 0;
    if (!(infile >> data)) {
      infile.clear();
      infile.seekg(0, std::ios::beg);
      infile >> std::hex >> data;
    }
    in_reg.write(data);
    biquad_DFI(in_reg, out_reg, sclk_out, sdata_out);

    // Check and log output.
    if (!out_reg.empty()) {
      result = out_reg.read();
      outfile << result << std::endl;

      // Read the expected (golden) value.
      if (goldenfile >> golden_val) {
        // Compare the result with the golden value.
        if (result != golden_val) {
          if (abs(result - golden_val) > max_dif) {
            max_dif = abs(result - golden_val);
          }
          error_count++;
        }
      }
      sample_count++;
    }
  }

  std::cout << "Total samples processed: " << sample_count << std::endl;
  std::cout << "Total mismatches: " << error_count << std::endl;
  std::cout << "Total mismatches: " << error_count << std::endl;
  std::cout << "Maximum difference: " << max_dif << std::endl;

  infile.close();
  goldenfile.close();
  outfile.close();
  return 0;
}
