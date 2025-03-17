#ifndef TDM_COUNT_H
#define TDM_COUNT_H

#include <ap_int.h>
#include <hls_stream.h>

typedef ap_uint<1> bit_t;

// tdm_count:
//   - Monitors the input clock signal (clk_in).
//   - Increments a 4-bit counter on each rising edge of clk_in.
//   - When the counter reaches 16, it resets to 0.
// The current count is output via sample_count.
void tdm_gpio_input(const bit_t clk_in, const bit_t lrclk_in,
                    const bit_t sdata_in, ap_uint<5> &sclk_cnt, bit_t &sclk_rdy,
                    ap_uint<4> &sample_cnt, bit_t &sample_rdy,
                    ap_uint<32> &sample_reg);

#endif // TDM_COUNT_H
