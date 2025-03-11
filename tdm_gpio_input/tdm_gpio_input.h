#ifndef TDM_GPIO_INPUT_H
#define TDM_GPIO_INPUT_H

#include <ap_int.h>
#include <hls_stream.h>

#ifndef FRAME_SIZE
#define CHANNEL_SIZE 32
#define NUM_CHANNELS 16
#define FRAME_SIZE CHANNEL_SIZE *NUM_CHANNELS
#endif

typedef ap_uint<1> bit_t;
typedef ap_uint<CHANNEL_SIZE> sample_t;

void tdm_gpio_input(volatile const bit_t *sdata, volatile const bit_t *sclk,
                    volatile const bit_t *lrclk,
                    hls::stream<sample_t> &serial_stream);

#endif
