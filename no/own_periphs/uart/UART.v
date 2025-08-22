module uart_clk_div_16_1
  (input  clk,
   input  rst,
   input  clear,
   input  enable,
   output div_mark);
  wire [3:0] clk_div_cnt;
  wire clk_div_cnt_mark;
  wire n343_o;
  wire [3:0] n345_o;
  wire [3:0] n347_o;
  wire [3:0] n348_o;
  wire [3:0] n350_o;
  wire n355_o;
  wire n356_o;
  wire n360_o;
  reg [3:0] n363_q;
  reg n364_q;
  assign div_mark = n364_q;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_clk_div.vhd:33:12  */
  assign clk_div_cnt = n363_q; // (signal)
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_clk_div.vhd:34:12  */
  assign clk_div_cnt_mark = n356_o; // (signal)
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_clk_div.vhd:44:33  */
  assign n343_o = clk_div_cnt == 4'b1111;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_clk_div.vhd:47:48  */
  assign n345_o = clk_div_cnt + 4'b0001;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_clk_div.vhd:44:17  */
  assign n347_o = n343_o ? 4'b0000 : n345_o;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_clk_div.vhd:43:13  */
  assign n348_o = enable ? n347_o : clk_div_cnt;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_clk_div.vhd:41:13  */
  assign n350_o = clear ? 4'b0000 : n348_o;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_clk_div.vhd:53:47  */
  assign n355_o = clk_div_cnt == 4'b0001;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_clk_div.vhd:53:29  */
  assign n356_o = n355_o ? 1'b1 : 1'b0;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_clk_div.vhd:58:32  */
  assign n360_o = enable & clk_div_cnt_mark;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_clk_div.vhd:40:9  */
  always @(posedge clk)
    n363_q <= n350_o;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_clk_div.vhd:57:9  */
  always @(posedge clk)
    n364_q <= n360_o;
endmodule

module uart_clk_div_16_3
  (input  clk,
   input  rst,
   input  clear,
   input  enable,
   output div_mark);
  wire [3:0] clk_div_cnt;
  wire clk_div_cnt_mark;
  wire n317_o;
  wire [3:0] n319_o;
  wire [3:0] n321_o;
  wire [3:0] n322_o;
  wire [3:0] n324_o;
  wire n329_o;
  wire n330_o;
  wire n334_o;
  reg [3:0] n337_q;
  reg n338_q;
  assign div_mark = n338_q;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_clk_div.vhd:33:12  */
  assign clk_div_cnt = n337_q; // (signal)
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_clk_div.vhd:34:12  */
  assign clk_div_cnt_mark = n330_o; // (signal)
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_clk_div.vhd:44:33  */
  assign n317_o = clk_div_cnt == 4'b1111;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_clk_div.vhd:47:48  */
  assign n319_o = clk_div_cnt + 4'b0001;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_clk_div.vhd:44:17  */
  assign n321_o = n317_o ? 4'b0000 : n319_o;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_clk_div.vhd:43:13  */
  assign n322_o = enable ? n321_o : clk_div_cnt;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_clk_div.vhd:41:13  */
  assign n324_o = clear ? 4'b0000 : n322_o;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_clk_div.vhd:53:47  */
  assign n329_o = clk_div_cnt == 4'b0011;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_clk_div.vhd:53:29  */
  assign n330_o = n329_o ? 1'b1 : 1'b0;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_clk_div.vhd:58:32  */
  assign n334_o = enable & clk_div_cnt_mark;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_clk_div.vhd:40:9  */
  always @(posedge clk)
    n337_q <= n324_o;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_clk_div.vhd:57:9  */
  always @(posedge clk)
    n338_q <= n334_o;
endmodule

module uart_tx_16_71f8e7976e4cbc4561c9d62fb283e7f788202acb
  (input  clk,
   input  rst,
   input  uart_clk_en,
   input  [7:0] din,
   input  din_vld,
   output uart_txd,
   output din_rdy);
  wire tx_clk_en;
  wire tx_clk_div_clr;
  wire [7:0] tx_data;
  wire [2:0] tx_bit_count;
  wire tx_bit_count_en;
  wire tx_ready;
  wire tx_parity_bit;
  wire [1:0] tx_data_out_sel;
  wire [2:0] tx_pstate;
  wire [2:0] tx_nstate;
  wire tx_clk_divider_i_div_mark;
  wire n176_o;
  wire n182_o;
  wire n184_o;
  wire [2:0] n186_o;
  wire [2:0] n188_o;
  wire [2:0] n189_o;
  wire [2:0] n191_o;
  wire n198_o;
  wire n203_o;
  wire n205_o;
  wire [2:0] n206_o;
  reg n209_o;
  wire n211_o;
  wire [2:0] n218_o;
  wire [2:0] n225_o;
  wire n227_o;
  wire [2:0] n230_o;
  wire n232_o;
  wire [2:0] n235_o;
  wire n237_o;
  wire n239_o;
  wire n240_o;
  wire [2:0] n243_o;
  wire n245_o;
  wire [2:0] n248_o;
  wire n250_o;
  wire [2:0] n253_o;
  wire [2:0] n255_o;
  wire n257_o;
  wire [5:0] n258_o;
  reg n266_o;
  reg n274_o;
  reg n282_o;
  reg [1:0] n290_o;
  reg [2:0] n292_o;
  wire [7:0] n294_o;
  reg [7:0] n295_q;
  reg [2:0] n296_q;
  reg [2:0] n297_q;
  reg n298_q;
  wire n299_o;
  wire n300_o;
  wire n301_o;
  wire n302_o;
  wire n303_o;
  wire n304_o;
  wire n305_o;
  wire n306_o;
  wire [1:0] n307_o;
  reg n308_o;
  wire [1:0] n309_o;
  reg n310_o;
  wire n311_o;
  wire n312_o;
  assign uart_txd = n298_q;
  assign din_rdy = tx_ready;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_tx.vhd:33:12  */
  assign tx_clk_en = tx_clk_divider_i_div_mark; // (signal)
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_tx.vhd:34:12  */
  assign tx_clk_div_clr = n266_o; // (signal)
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_tx.vhd:35:12  */
  assign tx_data = n295_q; // (signal)
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_tx.vhd:36:12  */
  assign tx_bit_count = n296_q; // (signal)
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_tx.vhd:37:12  */
  assign tx_bit_count_en = n274_o; // (signal)
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_tx.vhd:38:12  */
  assign tx_ready = n282_o; // (signal)
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_tx.vhd:39:12  */
  assign tx_parity_bit = 1'b0; // (signal)
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_tx.vhd:40:12  */
  assign tx_data_out_sel = n290_o; // (signal)
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_tx.vhd:43:12  */
  assign tx_pstate = n297_q; // (signal)
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_tx.vhd:44:12  */
  assign tx_nstate = n292_o; // (signal)
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_tx.vhd:54:5  */
  uart_clk_div_16_1 tx_clk_divider_i (
    .clk(clk),
    .rst(rst),
    .clear(tx_clk_div_clr),
    .enable(uart_clk_en),
    .div_mark(tx_clk_divider_i_div_mark));
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_tx.vhd:74:31  */
  assign n176_o = tx_ready & din_vld;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_tx.vhd:89:42  */
  assign n182_o = tx_clk_en & tx_bit_count_en;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_tx.vhd:90:34  */
  assign n184_o = tx_bit_count == 3'b111;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_tx.vhd:93:50  */
  assign n186_o = tx_bit_count + 3'b001;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_tx.vhd:90:17  */
  assign n188_o = n184_o ? 3'b000 : n186_o;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_tx.vhd:89:13  */
  assign n189_o = n182_o ? n188_o : tx_bit_count;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_tx.vhd:87:13  */
  assign n191_o = rst ? 3'b000 : n189_o;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_tx.vhd:130:21  */
  assign n198_o = tx_data_out_sel == 2'b01;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_tx.vhd:132:21  */
  assign n203_o = tx_data_out_sel == 2'b10;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_tx.vhd:134:21  */
  assign n205_o = tx_data_out_sel == 2'b11;
  assign n206_o = {n205_o, n203_o, n198_o};
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_tx.vhd:129:17  */
  always @*
    case (n206_o)
      3'b100: n209_o = tx_parity_bit;
      3'b010: n209_o = n312_o;
      3'b001: n209_o = 1'b0;
      default: n209_o = 1'b1;
    endcase
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_tx.vhd:126:13  */
  assign n211_o = rst ? 1'b1 : n209_o;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_tx.vhd:151:13  */
  assign n218_o = rst ? 3'b000 : tx_nstate;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_tx.vhd:171:17  */
  assign n225_o = din_vld ? 3'b001 : 3'b000;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_tx.vhd:165:13  */
  assign n227_o = tx_pstate == 3'b000;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_tx.vhd:183:17  */
  assign n230_o = tx_clk_en ? 3'b010 : 3'b001;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_tx.vhd:177:13  */
  assign n232_o = tx_pstate == 3'b001;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_tx.vhd:195:17  */
  assign n235_o = tx_clk_en ? 3'b011 : 3'b010;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_tx.vhd:189:13  */
  assign n237_o = tx_pstate == 3'b010;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_tx.vhd:207:57  */
  assign n239_o = tx_bit_count == 3'b111;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_tx.vhd:207:39  */
  assign n240_o = n239_o & tx_clk_en;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_tx.vhd:207:17  */
  assign n243_o = n240_o ? 3'b101 : 3'b011;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_tx.vhd:201:13  */
  assign n245_o = tx_pstate == 3'b011;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_tx.vhd:223:17  */
  assign n248_o = tx_clk_en ? 3'b101 : 3'b100;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_tx.vhd:217:13  */
  assign n250_o = tx_pstate == 3'b100;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_tx.vhd:237:17  */
  assign n253_o = tx_clk_en ? 3'b000 : 3'b101;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_tx.vhd:235:17  */
  assign n255_o = din_vld ? 3'b001 : n253_o;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_tx.vhd:229:13  */
  assign n257_o = tx_pstate == 3'b101;
  assign n258_o = {n257_o, n250_o, n245_o, n237_o, n232_o, n227_o};
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_tx.vhd:163:9  */
  always @*
    case (n258_o)
      6'b100000: n266_o = 1'b0;
      6'b010000: n266_o = 1'b0;
      6'b001000: n266_o = 1'b0;
      6'b000100: n266_o = 1'b0;
      6'b000010: n266_o = 1'b0;
      6'b000001: n266_o = 1'b1;
      default: n266_o = 1'b0;
    endcase
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_tx.vhd:163:9  */
  always @*
    case (n258_o)
      6'b100000: n274_o = 1'b0;
      6'b010000: n274_o = 1'b0;
      6'b001000: n274_o = 1'b1;
      6'b000100: n274_o = 1'b0;
      6'b000010: n274_o = 1'b0;
      6'b000001: n274_o = 1'b0;
      default: n274_o = 1'b0;
    endcase
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_tx.vhd:163:9  */
  always @*
    case (n258_o)
      6'b100000: n282_o = 1'b1;
      6'b010000: n282_o = 1'b0;
      6'b001000: n282_o = 1'b0;
      6'b000100: n282_o = 1'b0;
      6'b000010: n282_o = 1'b0;
      6'b000001: n282_o = 1'b1;
      default: n282_o = 1'b0;
    endcase
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_tx.vhd:163:9  */
  always @*
    case (n258_o)
      6'b100000: n290_o = 2'b00;
      6'b010000: n290_o = 2'b11;
      6'b001000: n290_o = 2'b10;
      6'b000100: n290_o = 2'b01;
      6'b000010: n290_o = 2'b00;
      6'b000001: n290_o = 2'b00;
      default: n290_o = 2'b00;
    endcase
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_tx.vhd:163:9  */
  always @*
    case (n258_o)
      6'b100000: n292_o = n255_o;
      6'b010000: n292_o = n248_o;
      6'b001000: n292_o = n243_o;
      6'b000100: n292_o = n235_o;
      6'b000010: n292_o = n230_o;
      6'b000001: n292_o = n225_o;
      default: n292_o = 3'b000;
    endcase
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_tx.vhd:73:9  */
  assign n294_o = n176_o ? din : tx_data;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_tx.vhd:73:9  */
  always @(posedge clk)
    n295_q <= n294_o;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_tx.vhd:86:9  */
  always @(posedge clk)
    n296_q <= n191_o;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_tx.vhd:150:9  */
  always @(posedge clk)
    n297_q <= n218_o;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_tx.vhd:125:9  */
  always @(posedge clk)
    n298_q <= n211_o;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_tx.vhd:64:21  */
  assign n299_o = tx_data[0];
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_tx.vhd:27:9  */
  assign n300_o = tx_data[1];
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_tx.vhd:23:9  */
  assign n301_o = tx_data[2];
  assign n302_o = tx_data[3];
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_tx.vhd:160:5  */
  assign n303_o = tx_data[4];
  assign n304_o = tx_data[5];
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_tx.vhd:148:5  */
  assign n305_o = tx_data[6];
  assign n306_o = tx_data[7];
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_tx.vhd:133:44  */
  assign n307_o = tx_bit_count[1:0];
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_tx.vhd:133:44  */
  always @*
    case (n307_o)
      2'b00: n308_o = n299_o;
      2'b01: n308_o = n300_o;
      2'b10: n308_o = n301_o;
      2'b11: n308_o = n302_o;
    endcase
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_tx.vhd:133:44  */
  assign n309_o = tx_bit_count[1:0];
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_tx.vhd:133:44  */
  always @*
    case (n309_o)
      2'b00: n310_o = n303_o;
      2'b01: n310_o = n304_o;
      2'b10: n310_o = n305_o;
      2'b11: n310_o = n306_o;
    endcase
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_tx.vhd:133:44  */
  assign n311_o = tx_bit_count[2];
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_tx.vhd:133:44  */
  assign n312_o = n311_o ? n310_o : n308_o;
endmodule

module uart_rx_16_71f8e7976e4cbc4561c9d62fb283e7f788202acb
  (input  clk,
   input  rst,
   input  uart_clk_en,
   input  uart_rxd,
   output [7:0] dout,
   output dout_vld,
   output frame_error,
   output parity_error);
  wire rx_clk_en;
  wire [7:0] rx_data;
  wire [2:0] rx_bit_count;
  wire rx_parity_error;
  wire rx_done;
  wire fsm_idle;
  wire fsm_databits;
  wire fsm_stopbit;
  wire [2:0] fsm_pstate;
  wire [2:0] fsm_nstate;
  wire rx_clk_divider_i_div_mark;
  wire n58_o;
  wire n60_o;
  wire [2:0] n62_o;
  wire [2:0] n64_o;
  wire [2:0] n65_o;
  wire [2:0] n67_o;
  wire n72_o;
  wire [6:0] n73_o;
  wire [7:0] n74_o;
  wire n79_o;
  wire n82_o;
  wire n83_o;
  wire n84_o;
  wire n85_o;
  wire n86_o;
  wire n87_o;
  wire n89_o;
  wire n91_o;
  wire n93_o;
  wire [2:0] n102_o;
  wire n107_o;
  wire [2:0] n110_o;
  wire n112_o;
  wire [2:0] n115_o;
  wire n117_o;
  wire n119_o;
  wire n120_o;
  wire [2:0] n123_o;
  wire n125_o;
  wire [2:0] n128_o;
  wire n130_o;
  wire [2:0] n133_o;
  wire n135_o;
  wire [4:0] n136_o;
  reg n143_o;
  reg n150_o;
  reg n157_o;
  reg [2:0] n159_o;
  wire [7:0] n161_o;
  reg [7:0] n162_q;
  reg [2:0] n163_q;
  reg [2:0] n167_q;
  reg n168_q;
  reg n169_q;
  reg n170_q;
  assign dout = rx_data;
  assign dout_vld = n168_q;
  assign frame_error = n169_q;
  assign parity_error = n170_q;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_rx.vhd:34:12  */
  assign rx_clk_en = rx_clk_divider_i_div_mark; // (signal)
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_rx.vhd:35:12  */
  assign rx_data = n162_q; // (signal)
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_rx.vhd:36:12  */
  assign rx_bit_count = n163_q; // (signal)
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_rx.vhd:38:12  */
  assign rx_parity_error = 1'b0; // (signal)
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_rx.vhd:40:12  */
  assign rx_done = n79_o; // (signal)
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_rx.vhd:41:12  */
  assign fsm_idle = n143_o; // (signal)
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_rx.vhd:42:12  */
  assign fsm_databits = n150_o; // (signal)
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_rx.vhd:43:12  */
  assign fsm_stopbit = n157_o; // (signal)
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_rx.vhd:48:12  */
  assign fsm_pstate = n167_q; // (signal)
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_rx.vhd:49:12  */
  assign fsm_nstate = n159_o; // (signal)
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_rx.vhd:57:5  */
  uart_clk_div_16_3 rx_clk_divider_i (
    .clk(clk),
    .rst(rst),
    .clear(fsm_idle),
    .enable(uart_clk_en),
    .div_mark(rx_clk_divider_i_div_mark));
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_rx.vhd:79:36  */
  assign n58_o = fsm_databits & rx_clk_en;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_rx.vhd:80:34  */
  assign n60_o = rx_bit_count == 3'b111;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_rx.vhd:83:50  */
  assign n62_o = rx_bit_count + 3'b001;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_rx.vhd:80:17  */
  assign n64_o = n60_o ? 3'b000 : n62_o;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_rx.vhd:79:13  */
  assign n65_o = n58_o ? n64_o : rx_bit_count;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_rx.vhd:77:13  */
  assign n67_o = rst ? 3'b000 : n65_o;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_rx.vhd:96:33  */
  assign n72_o = fsm_databits & rx_clk_en;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_rx.vhd:97:46  */
  assign n73_o = rx_data[7:1];
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_rx.vhd:97:37  */
  assign n74_o = {uart_rxd, n73_o};
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_rx.vhd:137:26  */
  assign n79_o = rx_clk_en & fsm_stopbit;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_rx.vhd:147:45  */
  assign n82_o = ~rx_parity_error;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_rx.vhd:147:41  */
  assign n83_o = rx_done & n82_o;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_rx.vhd:147:65  */
  assign n84_o = n83_o & uart_rxd;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_rx.vhd:148:45  */
  assign n85_o = ~uart_rxd;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_rx.vhd:148:41  */
  assign n86_o = rx_done & n85_o;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_rx.vhd:149:41  */
  assign n87_o = rx_done & rx_parity_error;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_rx.vhd:142:13  */
  assign n89_o = rst ? 1'b0 : n84_o;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_rx.vhd:142:13  */
  assign n91_o = rst ? 1'b0 : n86_o;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_rx.vhd:142:13  */
  assign n93_o = rst ? 1'b0 : n87_o;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_rx.vhd:162:13  */
  assign n102_o = rst ? 3'b000 : fsm_nstate;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_rx.vhd:180:30  */
  assign n107_o = ~uart_rxd;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_rx.vhd:180:17  */
  assign n110_o = n107_o ? 3'b001 : 3'b000;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_rx.vhd:175:13  */
  assign n112_o = fsm_pstate == 3'b000;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_rx.vhd:191:17  */
  assign n115_o = rx_clk_en ? 3'b010 : 3'b001;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_rx.vhd:186:13  */
  assign n117_o = fsm_pstate == 3'b001;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_rx.vhd:202:57  */
  assign n119_o = rx_bit_count == 3'b111;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_rx.vhd:202:39  */
  assign n120_o = n119_o & rx_clk_en;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_rx.vhd:202:17  */
  assign n123_o = n120_o ? 3'b100 : 3'b010;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_rx.vhd:197:13  */
  assign n125_o = fsm_pstate == 3'b010;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_rx.vhd:217:17  */
  assign n128_o = rx_clk_en ? 3'b100 : 3'b011;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_rx.vhd:212:13  */
  assign n130_o = fsm_pstate == 3'b011;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_rx.vhd:228:17  */
  assign n133_o = rx_clk_en ? 3'b000 : 3'b100;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_rx.vhd:223:13  */
  assign n135_o = fsm_pstate == 3'b100;
  assign n136_o = {n135_o, n130_o, n125_o, n117_o, n112_o};
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_rx.vhd:173:9  */
  always @*
    case (n136_o)
      5'b10000: n143_o = 1'b0;
      5'b01000: n143_o = 1'b0;
      5'b00100: n143_o = 1'b0;
      5'b00010: n143_o = 1'b0;
      5'b00001: n143_o = 1'b1;
      default: n143_o = 1'b0;
    endcase
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_rx.vhd:173:9  */
  always @*
    case (n136_o)
      5'b10000: n150_o = 1'b0;
      5'b01000: n150_o = 1'b0;
      5'b00100: n150_o = 1'b1;
      5'b00010: n150_o = 1'b0;
      5'b00001: n150_o = 1'b0;
      default: n150_o = 1'b0;
    endcase
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_rx.vhd:173:9  */
  always @*
    case (n136_o)
      5'b10000: n157_o = 1'b1;
      5'b01000: n157_o = 1'b0;
      5'b00100: n157_o = 1'b0;
      5'b00010: n157_o = 1'b0;
      5'b00001: n157_o = 1'b0;
      default: n157_o = 1'b0;
    endcase
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_rx.vhd:173:9  */
  always @*
    case (n136_o)
      5'b10000: n159_o = n133_o;
      5'b01000: n159_o = n128_o;
      5'b00100: n159_o = n123_o;
      5'b00010: n159_o = n115_o;
      5'b00001: n159_o = n110_o;
      default: n159_o = 3'b000;
    endcase
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_rx.vhd:95:9  */
  assign n161_o = n72_o ? n74_o : rx_data;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_rx.vhd:95:9  */
  always @(posedge clk)
    n162_q <= n161_o;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_rx.vhd:76:9  */
  always @(posedge clk)
    n163_q <= n67_o;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_rx.vhd:161:9  */
  always @(posedge clk)
    n167_q <= n102_o;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_rx.vhd:141:9  */
  always @(posedge clk)
    n168_q <= n89_o;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_rx.vhd:141:9  */
  always @(posedge clk)
    n169_q <= n91_o;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_rx.vhd:141:9  */
  always @(posedge clk)
    n170_q <= n93_o;
endmodule

module uart_clk_div_13_12
  (input  clk,
   input  rst,
   input  clear,
   input  enable,
   output div_mark);
  wire [3:0] clk_div_cnt;
  wire clk_div_cnt_mark;
  wire n29_o;
  wire [3:0] n31_o;
  wire [3:0] n33_o;
  wire [3:0] n34_o;
  wire [3:0] n36_o;
  wire n41_o;
  wire n42_o;
  wire n46_o;
  reg [3:0] n49_q;
  reg n50_q;
  assign div_mark = n50_q;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_clk_div.vhd:33:12  */
  assign clk_div_cnt = n49_q; // (signal)
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_clk_div.vhd:34:12  */
  assign clk_div_cnt_mark = n42_o; // (signal)
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_clk_div.vhd:44:33  */
  assign n29_o = clk_div_cnt == 4'b1100;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_clk_div.vhd:47:48  */
  assign n31_o = clk_div_cnt + 4'b0001;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_clk_div.vhd:44:17  */
  assign n33_o = n29_o ? 4'b0000 : n31_o;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_clk_div.vhd:43:13  */
  assign n34_o = enable ? n33_o : clk_div_cnt;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_clk_div.vhd:41:13  */
  assign n36_o = clear ? 4'b0000 : n34_o;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_clk_div.vhd:53:47  */
  assign n41_o = clk_div_cnt == 4'b1100;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_clk_div.vhd:53:29  */
  assign n42_o = n41_o ? 1'b1 : 1'b0;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_clk_div.vhd:58:32  */
  assign n46_o = enable & clk_div_cnt_mark;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_clk_div.vhd:40:9  */
  always @(posedge clk)
    n49_q <= n36_o;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart_clk_div.vhd:57:9  */
  always @(posedge clk)
    n50_q <= n46_o;
endmodule

module UART
  (input  CLK,
   input  RST,
   input  UART_RXD,
   input  [7:0] DIN,
   input  DIN_VLD,
   output UART_TXD,
   output DIN_RDY,
   output [7:0] DOUT,
   output DOUT_VLD,
   output RXD_VLD,
   output FRAME_ERROR,
   output PARITY_ERROR);
  wire os_clk_en;
  wire uart_rxd_meta_n;
  wire uart_rxd_synced_n;
  wire uart_rxd_debounced_n;
  wire uart_rxd_debounced;
  wire os_clk_divider_ias_div_mark;
  localparam n7_o = 1'b1;
  wire n11_o;
  wire n15_o;
  wire [7:0] uart_rx_i_dout;
  wire uart_rx_i_dout_vld;
  wire uart_rx_i_frame_error;
  wire uart_rx_i_parity_error;
  wire uart_tx_i_uart_txd;
  wire uart_tx_i_din_rdy;
  reg n22_q;
  reg n23_q;
  localparam n24_o = 1'bZ;
  assign UART_TXD = uart_tx_i_uart_txd;
  assign DIN_RDY = uart_tx_i_din_rdy;
  assign DOUT = uart_rx_i_dout;
  assign DOUT_VLD = uart_rx_i_dout_vld;
  assign RXD_VLD = n24_o;
  assign FRAME_ERROR = uart_rx_i_frame_error;
  assign PARITY_ERROR = uart_rx_i_parity_error;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart.vhd:51:12  */
  assign os_clk_en = os_clk_divider_ias_div_mark; // (signal)
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart.vhd:52:12  */
  assign uart_rxd_meta_n = n22_q; // (signal)
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart.vhd:53:12  */
  assign uart_rxd_synced_n = n23_q; // (signal)
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart.vhd:54:12  */
  assign uart_rxd_debounced_n = uart_rxd_synced_n; // (signal)
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart.vhd:55:12  */
  assign uart_rxd_debounced = n15_o; // (signal)
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart.vhd:63:5  */
  uart_clk_div_13_12 os_clk_divider_ias (
    .clk(CLK),
    .rst(RST),
    .clear(RST),
    .enable(n7_o),
    .div_mark(os_clk_divider_ias_div_mark));
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart.vhd:83:34  */
  assign n11_o = ~UART_RXD;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart.vhd:108:27  */
  assign n15_o = ~uart_rxd_debounced_n;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart.vhd:114:5  */
  uart_rx_16_71f8e7976e4cbc4561c9d62fb283e7f788202acb uart_rx_i (
    .clk(CLK),
    .rst(RST),
    .uart_clk_en(os_clk_en),
    .uart_rxd(uart_rxd_debounced),
    .dout(uart_rx_i_dout),
    .dout_vld(uart_rx_i_dout_vld),
    .frame_error(uart_rx_i_frame_error),
    .parity_error(uart_rx_i_parity_error));
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart.vhd:136:5  */
  uart_tx_16_71f8e7976e4cbc4561c9d62fb283e7f788202acb uart_tx_i (
    .clk(CLK),
    .rst(RST),
    .uart_clk_en(os_clk_en),
    .din(DIN),
    .din_vld(DIN_VLD),
    .uart_txd(uart_tx_i_uart_txd),
    .din_rdy(uart_tx_i_din_rdy));
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart.vhd:82:9  */
  always @(posedge CLK)
    n22_q <= n11_o;
  /* /home/haron/Documents/gits/risa-payload-gatemate-risc-v/no/own_periphs/uart/comp/uart.vhd:82:9  */
  always @(posedge CLK)
    n23_q <= uart_rxd_meta_n;
endmodule

