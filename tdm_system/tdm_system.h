#ifndef TDM_SYSTEM_H
#define TDM_SYSTEM_H

#include "../process_frame/process_frame.h"
#include "../tdm_gpio_input/tdm_gpio_input.h" // Provides bit_t, sample_t definitions and tdm_gpio_input()
#include "../tdm_gpio_output/tdm_gpio_output.h" // Provides tdm_gpio_output()

// Top-level integrated system function.
// It takes the external TDM signals for the input side (sdata_in, sclk_in,
// lrclk_in) and for the output side (sclk_out, lrclk_out, sdata_out).
// Internally, it creates streams that connect the TDM input to the frame
// processing and then to the TDM output.
void tdm_system(volatile const bit_t *sclk_in, volatile const bit_t *lrclk_in,
                volatile const bit_t *sdata_in,
                hls::stream<frame_t> &frame_stream);

#endif
