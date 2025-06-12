#include "biquad_DFII_fix.h"
#include <algorithm>
#include <algorithm> // std::fill_n
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>

Biquad_DFII_fix::Biquad_DFII_fix(const FilterCoefficients &coeff,
                                 DelayMemory &dly)
    : coeff(coeff), dly(dly) {}

smpl_t Biquad_DFII_fix::process(smpl_t in_val) {

  smpl_fix72_t res[6] = {};
  smpl_fix_inout_t in_val_fix;
  smpl_fix_inout_t out_val_fix;
  smpl_t out_val;

#pragma HLS allocation operation instances = mul limit = 1
#pragma HLS allocation operation instances = add limit = 1
#pragma HLS allocation operation instances = sub limit = 1
#pragma HLS PIPELINE off

  // Eingabewert in Fixed-Point-Format
  in_val_fix.range(31, 8) = in_val.range(31, 8);

  /*
  std::string in = in_val_fix.to_string(10, true);
  std::string w1 = dly.w1.to_string(10, true);
  std::string w2 = dly.w2.to_string(10, true);
  static int count = 0;
  const std::string p_test_folder =
      "C:/Users/andreas.hettler/Desktop/master_thesis/test";
  const std::string f_output_data = p_test_folder + "/result/calc.dat";

  std::ofstream outfile(f_output_data);
  if (!outfile) {
    std::cerr << "Error: Could not open " << f_output_data << std::endl;
    return 1;
  }

  if (count % 16 == 0) {

    outfile << "in\t\t" << in << std::endl;
    outfile << "dly.w1\t" << w1 << std::endl;
    outfile << "dly.w2\t" << w2 << std::endl;
  }
  */

  res[0] = dly.w1 * coeff.a1;
  res[1] = dly.w2 * coeff.a2;
  res[2] = in_val_fix - res[0] - res[1];
  res[3] = res[2] * coeff.b0;
  res[4] = dly.w1 * coeff.b1;
  res[5] = dly.w2 * coeff.b2;

  std::string res0 = res[0].to_string(10, true);
  std::string res1 = res[1].to_string(10, true);
  std::string res2 = res[2].to_string(10, true);
  std::string res3 = res[3].to_string(10, true);
  std::string res4 = res[4].to_string(10, true);
  std::string res5 = res[5].to_string(10, true);

  out_val_fix = res[3] + res[4] + res[5];

  // Direct Form II – Zustandstransformation
  dly.w2 = dly.w1;
  dly.w1 = res[2];

  /*
  w1 = dly.w1.to_string(10, true);
  w2 = dly.w2.to_string(10, true);
  std::string out = out_val_fix.to_string(10, true);
  if (count % 16 == 0) {

    outfile << "res[0]\t" << res0 << std::endl;
    outfile << "res[1]\t" << res1 << std::endl;
    outfile << "res[2]\t" << res2 << std::endl;
    outfile << "res[3]\t" << res3 << std::endl;
    outfile << "res[4]\t" << res4 << std::endl;
    outfile << "res[5]\t" << res5 << std::endl;
    outfile << "dly.w1\t" << w1 << std::endl;
    outfile << "dly.w2\t" << w2 << std::endl;
    outfile << "out\t\t" << res5 << std::endl;

    outfile.close();
  }
  count++; */

  // Rückwandlung in 24-bit-Sample
  out_val.range(31, 8) = out_val_fix.range(31, 8);
  return out_val & 0xFFFFFF00;
}
