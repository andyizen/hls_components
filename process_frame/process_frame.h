#ifndef PROCESS_FRAME_H
#define PROCESS_FRAME_H

#include <ap_int.h>
#include <hls_stream.h>

// Define frame/channel parameters.
#define NUM_CHANNELS 16
#define CHANNEL_SIZE 32
// FRAME_SIZE here represents the number of channels in a frame.
#define FRAME_SIZE NUM_CHANNELS

// Configurable parameter for parallelism.
#ifndef NUM_PIPELINES
#define NUM_PIPELINES 1
#endif

// Type for a single channel (32 bits).
typedef ap_uint<CHANNEL_SIZE> sample_t;

// Top-level function that processes one frame.
// It reads FRAME_SIZE samples from the input stream, processes them in
// parallel, and writes out FRAME_SIZE processed samples to the output stream.
void process_frame(hls::stream<sample_t> &in_stream,
                   hls::stream<sample_t> &out_stream);

#endif
