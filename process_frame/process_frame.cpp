#include "process_frame.h"

// Dummy processing function: add 1 to the sample.
sample_t process_sample(sample_t sample) {
#pragma HLS INLINE
  return sample + 1;
}

void process_frame(hls::stream<sample_t> &in_stream,
                   hls::stream<frame_t> &out_stream) {

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
  // Pack the processed samples into one frame and write it out.
  frame_t output_frame = 0;
  for (int i = 0; i < NUM_CHANNELS; i++) {
    // Pack each sample MSB-first.
    // The first sample becomes the most-significant 32 bits.
#pragma HLS UNROLL
    output_frame = (output_frame << CHANNEL_SIZE) | processed[i];
  }
  out_stream.write(output_frame);
}
