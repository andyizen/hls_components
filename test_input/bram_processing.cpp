// bram_processing.cpp
#include "./bram_processing.h"

/*
void bram_processor(volatile data32_t *bram_in, data4_t &led_out,
                    volatile data4_t *ctrl) {
#pragma HLS INTERFACE m_axi depth = 32 port = bram_in offset = off
#pragma HLS INTERFACE s_axilite port = ctrl bundle = control
#pragma HLS INTERFACE ap_none port = led_out
#pragma HLS INTERFACE mode = ap_ctrl_none port = return

#pragma HLS PIPELINE II = 1


  static ap_uint<4> cnt = 0;
  data4_t mem_num = *ctrl;
  data32_t data = bram_in[mem_num];

  led_out = (data >> 28) & 0xF;
  cnt++;
}

*/

struct test {
  data_t a;
  data_t b;
  data_t c;
  data_t d;
};

void bram_processor(test mem[NUM_ELEMENTS], ap_uint<32> addr, data_t dat,
                    ap_uint<4> &led_out) {
#pragma HLS INTERFACE mode = ap_memory port = mem storage_type = ram_t2p
#pragma HLS INTERFACE mode = ap_none port = led_out

#pragma HLS INTERFACE s_axilite port = addr bundle = CTRL
#pragma HLS INTERFACE s_axilite port = dat bundle = CTRL
#pragma HLS INTERFACE ap_ctrl_none port = return

  static int addr_int = 0;
  static data_t dat_int = 0;

  addr_int = addr;
  dat_int = dat;

  mem[addr_int].a = dat_int;
  mem[addr_int].b = dat_int;
  mem[addr_int].c = dat_int;
  mem[addr_int].d = dat_int;

  if (mem[1].a == 0x000A'0000'0005)
    led_out = 0xF;
  else if (mem[15].a == 0x000F'0000'00F2)
    led_out = 0x2;
  else
    led_out = 0x0;
}
