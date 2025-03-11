#ifndef PROCESS_FRAME_H
#define PROCESS_FRAME_H

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

#define NUM_PIPELINES 2


// Top-level function that processes one frame.
// It reads FRAME_SIZE samples from the input stream, processes them in
// parallel, and writes out FRAME_SIZE processed samples to the output stream.
void process_frame(hls::stream<sample_t> &in_stream,
                   hls::stream<frame_t> &out_stream);

#endif
