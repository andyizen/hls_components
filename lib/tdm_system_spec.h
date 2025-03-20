#ifndef TDM_SYSTEM_SPECS_H
#define TDM_SYSTEM_SPECS_H

constexpr int bitsNeeded(int n) {
  int bits = 0;
  while ((1 << bits) < n) {
    ++bits;
  }
  return bits;
}

#define HLS_STREAM

#include <ap_int.h>
#ifdef HLS_STREAM
#include <hls_stream.h>
#endif

const int SIZE_CHANNEL = 32;
const int CNT_BIT_DEPTH_SIZE_CHANNEL = bitsNeeded(SIZE_CHANNEL);
const int NUM_CHANNELS = 16;
const int CNT_BIT_DEPTH_NUM_CHANNELS = bitsNeeded(NUM_CHANNELS);
const int MCLKS_PER_BIT = 4;
const int CNT_BIT_DEPTH_MCLKS_PER_BIT = bitsNeeded(MCLKS_PER_BIT);

typedef ap_uint<1> bit_t;
typedef ap_uint<SIZE_CHANNEL> sample_t;

#ifndef HLS_STREAM
typedef sample_t sample_pipe_t;
#else
typedef hls::stream<sample_t> sample_pipe_t;
#endif

#endif