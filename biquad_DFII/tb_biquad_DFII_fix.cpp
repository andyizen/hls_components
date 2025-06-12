#include "biquad.h"

#include <algorithm>
#include <algorithm> // std::fill_n
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>

std::unordered_map<std::string, std::string>
parseConfig(const std::string &filename) {
  std::unordered_map<std::string, std::string> config;
  std::ifstream file(filename);
  std::string line;

  while (std::getline(file, line)) {
    // Ignore comments and empty lines
    if (line.empty() || line[0] == '#' || line[0] == ';')
      continue;

    std::istringstream is_line(line);
    std::string key;
    if (std::getline(is_line, key, '=')) {
      std::string value;
      if (std::getline(is_line, value)) {
        config[key] = value;
      }
    }
  }
  return config;
}

const std::string p_test_folder =
    "C:/Users/andreas.hettler/Desktop/master_thesis/test";
const std::string f_cfg_data = p_test_folder + "/input/filter.cfg";

enum ReadState { LOCKED, UNLOCKED };

smpl_ppln_t in_reg;
smpl_ppln_t out_reg;
ReadState read_stt = UNLOCKED;

// Open filter cfg and set coeffs
auto config = parseConfig(f_cfg_data);
double b0 = std::stof(config["b_0"]);
double b1 = std::stof(config["b_1"]);
double b2 = std::stof(config["b_2"]);
double a1 = std::stof(config["a_1"]);
double a2 = std::stof(config["a_2"]);
FilterCoefficients factors = {b0, b1, b2, a1, a2};
smpl_fix32_t coeff[NUM_CHANNELS * NUM_COEFFS] = {
    b0, b1, b2, a1, a2, b0, b1, b2, a1, a2, b0, b1, b2, a1, a2, b0,
    b1, b2, a1, a2, b0, b1, b2, a1, a2, b0, b1, b2, a1, a2, b0, b1,
    b2, a1, a2, b0, b1, b2, a1, a2, b0, b1, b2, a1, a2, b0, b1, b2,
    a1, a2, b0, b1, b2, a1, a2, b0, b1, b2, a1, a2, b0, b1, b2, a1,
    a2, b0, b1, b2, a1, a2, b0, b1, b2, a1, a2, b0, b1, b2, a1, a2};

bit_t sclk_out;
bit_t lrclk_out;
bit_t sdata_out;
bool lck_new_data = false;
int max_dif = 0;

int main(int argc, char *argv[]) {
  std::string test_signals[4] = {"impulse", "sinus", "step", "rms"};

  for (auto &signal : test_signals) {

    // Set input & result files
    const std::string f_input_data =
        p_test_folder + "/input/input_" + signal + "_multi.dat";
    const std::string f_output_data =
        p_test_folder + "/result/biquad_DFII/result_" + signal + ".dat";

    // Opening files
    std::ifstream infile(f_input_data);
    if (!infile) {
      std::cerr << "Error: Could not open " << f_input_data << std::endl;
      return 1;
    }
    std::ifstream cfgfile(f_cfg_data);
    if (!cfgfile) {
      std::cerr << "Error: Could not open " << f_input_data << std::endl;
      return 1;
    }
    std::ofstream outfile(f_output_data);
    if (!outfile) {
      std::cerr << "Error: Could not open " << f_output_data << std::endl;
      return 1;
    }

    int total_clc_cycles = 5000;
    int sample_in_count = 0;
    int sample_out_count = 0;
    int error_count = 0;
    smpl_t golden_val;
    smpl_t result;
    smpl_t data = 0;
    double data_float;
    while (true) {
      if (read_stt == UNLOCKED) {
        // Break if there is no input line anymore
        if (!(infile >> data_float))
          break;
        /* Scale the input to 24Bit signed int and then shift it to the right to
          satisfy the 32Bit container that is needed for the biquad input */
        double scaled = data_float < 0
                            ? data_float * static_cast<double>(1 << 23)
                            : data_float * (static_cast<double>(1 << 23) - 1);
        int32_t data_int24 = static_cast<int32_t>(scaled);
        uint32_t lower24 = static_cast<uint32_t>(data_int24) & 0x00FFFFFF;
        data = (lower24 << 8);
        in_reg.write(data);
        sample_in_count++;
        read_stt = LOCKED;
      }
      /* We got to consider that even the c simulation takes three calls to
        process one sample and deliver an output. That means we got to lock the
        input until there is an output. After that we can read the next line */

      biquad_DFII(in_reg, out_reg, coeff);

      if (!out_reg.empty() && read_stt == LOCKED) {
        // Check and log output.
        result = out_reg.read();
        outfile << result << std::endl;
        sample_out_count++;
        read_stt = UNLOCKED;
      }
    }
    std::cout << "###############################################" << std::endl;
    std::cout << "Total samples read: " << sample_in_count << std::endl;
    std::cout << "Total samples written: " << sample_out_count << std::endl;
    std::cout << "###############################################" << std::endl;

    infile.close();
    outfile.close();
    cfgfile.close();
  }
  return 0;
}
