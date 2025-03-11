#ifndef TDM_GPIO_OUTPUT_H
#define TDM_GPIO_OUTPUT_H

#include <ap_int.h>
#include <hls_stream.h>

#ifndef CHANNEL_SIZE
#define CHANNEL_SIZE 32
#endif

#ifndef NUM_CHANNELS
#define NUM_CHANNELS 16
#endif

#ifndef FRAME_SIZE
#define FRAME_SIZE (CHANNEL_SIZE * NUM_CHANNELS)
#endif

typedef ap_uint<1> bit_t;
typedef ap_uint<FRAME_SIZE> frame_t;

void tdm_gpio_output(volatile bit_t *sclk, volatile bit_t *lrclk,
                     volatile bit_t *sdata, hls::stream<frame_t> &in_stream);

#endif
