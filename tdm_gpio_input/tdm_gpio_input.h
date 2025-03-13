#ifndef TDM_GPIO_INPUT_H
#define TDM_GPIO_INPUT_H

#include <ap_int.h>
#include <hls_stream.h>

#ifndef SYSTEM_SPECS
#define SYSTEM_SPECS
const int CHANNEL_SIZE = 32;
const int NUM_CHANNELS = 16;
const int FRAME_SIZE = CHANNEL_SIZE * NUM_CHANNELS;
typedef ap_uint<1> bit_t;
typedef ap_uint<CHANNEL_SIZE> sample_t;
typedef ap_uint<FRAME_SIZE> frame_t;
#endif

// Sets all bits to one
const ap_uint<CHANNEL_SIZE> MASK = ~((ap_uint<CHANNEL_SIZE>)0);

void tdm_gpio_input(const bit_t sdata, const bit_t sclk, const bit_t lrclk,
                    bit_t &frame_rdy, bit_t &sample_rdy,
                    ap_uint<4> &sample_count, hls::stream<sample_t> &out);
#endif
