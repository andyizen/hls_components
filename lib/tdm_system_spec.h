#ifndef TDM_SYSTEM_SPECS_H
#define TDM_SYSTEM_SPECS_H

#define HLS_STREAM

#include <ap_int.h>
#ifdef HLS_STREAM
#include <hls_stream.h>
#endif

constexpr int bitsNeeded(int n) {
  int bits = 0;
  while ((1 << bits) < n) {
    ++bits;
  }
  return bits;
}

const int SIZE_SAMPLE = 32;
typedef ap_uint<SIZE_SAMPLE> smpl_t;
const int CNT_BIT_DEPTH_SIZE_SAMPLE = bitsNeeded(SIZE_SAMPLE);
typedef ap_uint<CNT_BIT_DEPTH_SIZE_SAMPLE> smpl_bts_cntr_t;

const int NUM_CHANNELS = 16;
const int CNT_BIT_DEPTH_NUM_CHANNELS = bitsNeeded(NUM_CHANNELS);
typedef ap_uint<CNT_BIT_DEPTH_NUM_CHANNELS> ch_cntr_t;

const int MCLKS_PER_BIT = 4;
const int CNT_BIT_DEPTH_MCLKS_PER_BIT = bitsNeeded(MCLKS_PER_BIT);
typedef ap_uint<CNT_BIT_DEPTH_MCLKS_PER_BIT> mclk_cntr_t;

typedef ap_uint<1> bit_t;

typedef ap_uint<SIZE_SAMPLE> smpl_t;

#ifndef HLS_STREAM
typedef smpl_t smpl_ppln_t;
#else
typedef hls::stream<smpl_t> smpl_ppln_t;
#endif

#endif