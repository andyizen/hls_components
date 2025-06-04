// bram_processing.h
#ifndef BRAM_PROCESSING_H_
#define BRAM_PROCESSING_H_

#include <ap_fixed.h>
#include <ap_int.h>
#include <hls_stream.h>

// Define data type - 48-bit unsigned integer
typedef ap_uint<32> data32_t;
typedef ap_uint<4> data4_t;

// Define the number of elements
#define NUM_ELEMENTS 16

typedef ap_uint<48> data_t;
typedef ap_fixed<48, 2, AP_TRN, AP_WRAP> smpl_fix_t;
typedef ap_uint<256> packed_filter_t;

struct FilterCoefficients {
  smpl_fix_t b0;
  smpl_fix_t b1;
  smpl_fix_t b2;
  smpl_fix_t a1;
  smpl_fix_t a2;
};

struct DelayMemory {
  data_t m_b1;
  data_t m_b2;
  data_t m_a1;
  data_t m_a2;
};

// Top function prototype
// void bram_processor(volatile data32_t *bram_in, data4_t *led_out,
//                    ap_uint<1> start);

// Top function prototype
void bram_processor(data_t mem[NUM_ELEMENTS], data_t addr, ap_uint<4> &led_out);

#endif