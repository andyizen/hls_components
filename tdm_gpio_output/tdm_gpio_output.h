#ifndef TDM_GPIO_OUTPUT_H
#define TDM_GPIO_OUTPUT_H

#include <ap_int.h>
#include <hls_stream.h>
#define HLS_STREAM

#ifndef SYSTEM_SPECS
#define SYSTEM_SPECS
const int CHANNEL_SIZE = 32;
const int NUM_CHANNELS = 16;
const int CLKS_PER_BIT = 4;
const int FRAME_SIZE = CHANNEL_SIZE * NUM_CHANNELS;
typedef ap_uint<1> bit_t;
typedef ap_uint<FRAME_SIZE> frame_t;
#ifndef HLS_STREAM
typedef ap_uint<CHANNEL_SIZE> sample_t;
#else
#include <hls_stream.h>
typedef hls::stream<ap_uint<CHANNEL_SIZE>> sample_t;
#endif
#endif

void tdm_gpio_output(sample_t &in_stream, bit_t &sclk, bit_t &lrclk,
                     bit_t &sdata);

#endif