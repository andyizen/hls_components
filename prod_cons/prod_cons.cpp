#include "prod_cons.h"
#include <hls_stream.h>
#include <hls_task.h>

void producer(bit_t data_in, bit_t sclk, hls::stream<ap_uint<16>> &stream_out) {
#pragma HLS INTERFACE mode = ap_vld port = data_in
#pragma HLS INTERFACE mode = ap_vld port = sclk
#pragma HLS INTERFACE axis port = stream_out

  static ap_uint<4> cnt = 0;
  static ap_uint<4> add = 0;
  static ap_uint<16> buf_reg = {0};
  static bool lock = false;

  if (sclk && !lock) {
    buf_reg = (buf_reg << 1) | data_in;

    if (cnt == 15) {
      if (!stream_out.full())
        stream_out.write(buf_reg);
      cnt = 0;
    } else {
      cnt++;
    }
    lock = true;
  } else if (!sclk && lock) {
    lock = false;
  }
}

/* void read(hls::stream<ap_uint<16>> &stream_in, ap_uint<16> &p_out) {
#pragma HLS PIPELINE II = 16
#pragma HLS LATENCY min = 1 max = 2
#pragma HLS INTERFACE axis port = stream_in
  static ap_uint<16> buf_reg = 0;
  if (!stream_in.empty())
    buf_reg = stream_in.read();

  p_out = buf_reg;
} */

void prod_cons(bit_t sclk, bit_t data_in, ap_uint<16> &p_out) {

  hls::stream<ap_uint<16>> inter_stream;
  producer(sclk, data_in, inter_stream);
}
