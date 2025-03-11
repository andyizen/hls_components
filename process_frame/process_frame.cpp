#include "process_frame.h"

// Dummy processing function: add 1 to the sample.
sample_t process_sample(sample_t sample) {
#pragma HLS INLINE
  return sample + 1;
}

void process_frame(hls::stream<sample_t> &in_stream,
                   hls::stream<sample_t> &out_stream) {

  // Array to hold one frame.
  sample_t frame[NUM_CHANNELS];
#pragma HLS ARRAY_PARTITION variable = frame cyclic factor = NUM_PIPELINES

READ_IN:
  // Read a frame: one sample per channel.
  for (int i = 0; i < NUM_CHANNELS; i++) {
#pragma HLS PIPELINE II = 1
    frame[i] = in_stream.read();
  }

  sample_t processed[NUM_CHANNELS];
#pragma HLS ARRAY_PARTITION variable = processed cyclic factor = NUM_PIPELINES

PROCESS:
  // Process each channel. Unroll with the factor NUM_PIPELINES.
  for (int i = 0; i < NUM_CHANNELS; i++) {
#pragma HLS UNROLL factor = NUM_PIPELINES
    processed[i] = process_sample(frame[i]);
  }

WRITE_OUT:
  // Write out the processed frame.
  for (int i = 0; i < NUM_CHANNELS; i++) {
#pragma HLS PIPELINE II = 1
    out_stream.write(processed[i]);
  }
}
