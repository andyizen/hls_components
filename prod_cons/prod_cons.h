#ifndef PROD_CONS_H
#define PROD_CONS_H
#include "ap_int.h"
#include "hls_stream.h"

typedef ap_uint<1> bit_t;
typedef hls::stream<ap_uint<16>> stream16;
typedef ap_uint<16> sample16;
void prod_cons(ap_uint<16> &p_out);

#endif