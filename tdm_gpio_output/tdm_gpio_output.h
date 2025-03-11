#ifndef TDM_GPIO_OUTPUT_H
#define TDM_GPIO_OUTPUT_H

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

void tdm_gpio_output(volatile bit_t *sclk, volatile bit_t *lrclk,
                     volatile bit_t *sdata, hls::stream<frame_t> &in_stream);

#endif
