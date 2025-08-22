module spi_master_1
  (input  clk,
   input  div_clk,
   input  reset_n,
   input  enable,
   input  cpol,
   input  cpha,
   input  miso,
   input  ss_n,
   input  [7:0] tx,
   output sclk,
   output mosi,
   output busy,
   output [7:0] rx);
  wire state;
  wire receive_transmit;
  wire [4:0] clk_toggles;
  wire [4:0] last_bit;
  reg [7:0] rxbuffer;
  reg [7:0] txbuffer;
  wire int_sclk;
  wire n8_o;
  wire n10_o;
  wire [31:0] n11_o;
  wire [31:0] n13_o;
  wire [31:0] n15_o;
  wire [4:0] n16_o;
  wire n19_o;
  wire n23_o;
  wire n24_o;
  wire [4:0] n26_o;
  wire [4:0] n27_o;
  wire [7:0] n28_o;
  wire n30_o;
  wire n31_o;
  wire [31:0] n32_o;
  wire n34_o;
  wire [31:0] n35_o;
  wire [31:0] n37_o;
  wire [4:0] n38_o;
  wire [4:0] n40_o;
  wire [31:0] n41_o;
  wire n43_o;
  wire n44_o;
  wire n45_o;
  wire n46_o;
  wire [31:0] n47_o;
  wire [31:0] n48_o;
  wire [31:0] n50_o;
  wire n51_o;
  wire n52_o;
  wire [6:0] n53_o;
  wire [7:0] n54_o;
  wire [7:0] n55_o;
  wire [31:0] n56_o;
  wire [31:0] n57_o;
  wire n58_o;
  wire n59_o;
  wire n60_o;
  wire [6:0] n61_o;
  wire [7:0] n63_o;
  wire n64_o;
  wire [7:0] n65_o;
  wire [31:0] n66_o;
  wire n68_o;
  wire n70_o;
  wire n73_o;
  wire [7:0] n75_o;
  wire n78_o;
  wire n79_o;
  wire n81_o;
  wire [1:0] n82_o;
  reg n85_o;
  reg n87_o;
  reg [7:0] n89_o;
  reg n91_o;
  reg n93_o;
  reg [4:0] n95_o;
  reg [4:0] n97_o;
  reg [7:0] n99_o;
  reg [7:0] n101_o;
  reg n103_o;
  reg n129_q;
  wire n130_o;
  wire n131_o;
  reg n132_q;
  wire n133_o;
  wire [4:0] n134_o;
  reg [4:0] n135_q;
  wire n136_o;
  wire [4:0] n137_o;
  reg [4:0] n138_q;
  wire n139_o;
  wire [7:0] n140_o;
  reg [7:0] n141_q;
  wire n142_o;
  wire [7:0] n143_o;
  reg [7:0] n144_q;
  reg n145_q;
  reg n146_q;
  reg n147_q;
  reg [7:0] n148_q;
  assign sclk = int_sclk;
  assign mosi = n146_q;
  assign busy = n147_q;
  assign rx = n148_q;
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:27:10  */
  assign state = n129_q; // (signal)
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:28:10  */
  assign receive_transmit = n132_q; // (signal)
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:29:10  */
  assign clk_toggles = n135_q; // (signal)
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:30:10  */
  assign last_bit = n138_q; // (signal)
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:31:10  */
  always @*
    rxbuffer = n141_q; // (isignal)
  initial
    rxbuffer = 8'b00000000;
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:32:10  */
  always @*
    txbuffer = n144_q; // (isignal)
  initial
    txbuffer = 8'b00000000;
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:33:10  */
  assign int_sclk = n145_q; // (signal)
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:43:16  */
  assign n8_o = ~reset_n;
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:61:33  */
  assign n10_o = ~cpha;
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:64:39  */
  assign n11_o = {31'b0, cpha};  //  uext
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:64:39  */
  assign n13_o = 32'b00000000000000000000000000010000 + n11_o;
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:64:60  */
  assign n15_o = n13_o - 32'b00000000000000000000000000000001;
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:64:25  */
  assign n16_o = n15_o[4:0];  // trunc
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:58:11  */
  assign n19_o = enable ? 1'b1 : 1'b0;
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:58:11  */
  assign n23_o = enable ? 1'b1 : 1'b0;
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:58:11  */
  assign n24_o = enable ? n10_o : receive_transmit;
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:58:11  */
  assign n26_o = enable ? 5'b00000 : clk_toggles;
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:58:11  */
  assign n27_o = enable ? n16_o : last_bit;
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:58:11  */
  assign n28_o = enable ? tx : txbuffer;
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:53:9  */
  assign n30_o = state == 1'b0;
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:73:46  */
  assign n31_o = ~receive_transmit;
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:76:41  */
  assign n32_o = {27'b0, clk_toggles};  //  uext
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:76:41  */
  assign n34_o = n32_o == 32'b00000000000000000000000000010000;
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:79:60  */
  assign n35_o = {27'b0, clk_toggles};  //  uext
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:79:60  */
  assign n37_o = n35_o + 32'b00000000000000000000000000000001;
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:79:48  */
  assign n38_o = n37_o[4:0];  // trunc
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:76:26  */
  assign n40_o = n34_o ? 5'b00000 : n38_o;
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:83:26  */
  assign n41_o = {27'b0, clk_toggles};  //  uext
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:83:26  */
  assign n43_o = $signed(n41_o) <= $signed(32'b00000000000000000000000000010000);
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:84:25  */
  assign n44_o = ~int_sclk;
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:83:11  */
  assign n45_o = n43_o ? n44_o : int_sclk;
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:88:31  */
  assign n46_o = ~receive_transmit;
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:88:53  */
  assign n47_o = {27'b0, clk_toggles};  //  uext
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:88:64  */
  assign n48_o = {27'b0, last_bit};  //  uext
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:88:64  */
  assign n50_o = n48_o + 32'b00000000000000000000000000000001;
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:88:53  */
  assign n51_o = $signed(n47_o) < $signed(n50_o);
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:88:37  */
  assign n52_o = n51_o & n46_o;
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:89:33  */
  assign n53_o = rxbuffer[6:0];
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:89:58  */
  assign n54_o = {n53_o, miso};
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:88:11  */
  assign n55_o = n52_o ? n54_o : rxbuffer;
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:93:53  */
  assign n56_o = {27'b0, clk_toggles};  //  uext
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:93:53  */
  assign n57_o = {27'b0, last_bit};  //  uext
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:93:53  */
  assign n58_o = $signed(n56_o) < $signed(n57_o);
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:93:37  */
  assign n59_o = n58_o & receive_transmit;
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:94:29  */
  assign n60_o = txbuffer[7];
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:95:33  */
  assign n61_o = txbuffer[6:0];
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:95:58  */
  assign n63_o = {n61_o, 1'b0};
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:93:11  */
  assign n64_o = n59_o ? n60_o : n146_q;
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:93:11  */
  assign n65_o = n59_o ? n63_o : txbuffer;
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:99:26  */
  assign n66_o = {27'b0, clk_toggles};  //  uext
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:99:26  */
  assign n68_o = n66_o == 32'b00000000000000000000000000010000;
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:99:11  */
  assign n70_o = n68_o ? 1'bZ : n64_o;
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:99:11  */
  assign n73_o = n68_o ? 1'b0 : 1'b1;
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:99:11  */
  assign n75_o = n68_o ? rxbuffer : n148_q;
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:99:11  */
  assign n78_o = n68_o ? 1'b0 : 1'b1;
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:99:11  */
  assign n79_o = n68_o ? cpol : n45_o;
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:71:9  */
  assign n81_o = state == 1'b1;
  assign n82_o = {n81_o, n30_o};
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:51:7  */
  always @*
    case (n82_o)
      2'b10: n85_o = n70_o;
      2'b01: n85_o = 1'bZ;
      default: n85_o = 1'bX;
    endcase
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:51:7  */
  always @*
    case (n82_o)
      2'b10: n87_o = n73_o;
      2'b01: n87_o = n19_o;
      default: n87_o = 1'bX;
    endcase
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:51:7  */
  always @*
    case (n82_o)
      2'b10: n89_o = n75_o;
      2'b01: n89_o = n148_q;
      default: n89_o = 8'bX;
    endcase
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:51:7  */
  always @*
    case (n82_o)
      2'b10: n91_o = n78_o;
      2'b01: n91_o = n23_o;
      default: n91_o = 1'bX;
    endcase
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:51:7  */
  always @*
    case (n82_o)
      2'b10: n93_o = n31_o;
      2'b01: n93_o = n24_o;
      default: n93_o = 1'bX;
    endcase
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:51:7  */
  always @*
    case (n82_o)
      2'b10: n95_o = n40_o;
      2'b01: n95_o = n26_o;
      default: n95_o = 5'bX;
    endcase
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:51:7  */
  always @*
    case (n82_o)
      2'b10: n97_o = last_bit;
      2'b01: n97_o = n27_o;
      default: n97_o = 5'bX;
    endcase
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:51:7  */
  always @*
    case (n82_o)
      2'b10: n99_o = n55_o;
      2'b01: n99_o = rxbuffer;
      default: n99_o = 8'bX;
    endcase
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:51:7  */
  always @*
    case (n82_o)
      2'b10: n101_o = n65_o;
      2'b01: n101_o = n28_o;
      default: n101_o = 8'bX;
    endcase
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:51:7  */
  always @*
    case (n82_o)
      2'b10: n103_o = n79_o;
      2'b01: n103_o = cpol;
      default: n103_o = 1'bX;
    endcase
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:50:5  */
  always @(negedge clk or posedge n8_o)
    if (n8_o)
      n129_q <= 1'b0;
    else
      n129_q <= n91_o;
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:40:3  */
  assign n130_o = ~n8_o;
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:50:5  */
  assign n131_o = n130_o ? n93_o : receive_transmit;
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:50:5  */
  always @(negedge clk)
    n132_q <= n131_o;
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:40:3  */
  assign n133_o = ~n8_o;
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:50:5  */
  assign n134_o = n133_o ? n95_o : clk_toggles;
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:50:5  */
  always @(negedge clk)
    n135_q <= n134_o;
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:40:3  */
  assign n136_o = ~n8_o;
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:50:5  */
  assign n137_o = n136_o ? n97_o : last_bit;
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:50:5  */
  always @(negedge clk)
    n138_q <= n137_o;
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:40:3  */
  assign n139_o = ~n8_o;
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:50:5  */
  assign n140_o = n139_o ? n99_o : rxbuffer;
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:50:5  */
  always @(negedge clk)
    n141_q <= n140_o;
  initial
    n141_q = 8'b00000000;
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:40:3  */
  assign n142_o = ~n8_o;
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:50:5  */
  assign n143_o = n142_o ? n101_o : txbuffer;
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:50:5  */
  always @(negedge clk)
    n144_q <= n143_o;
  initial
    n144_q = 8'b00000000;
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:50:5  */
  always @(negedge clk or posedge n8_o)
    if (n8_o)
      n145_q <= cpol;
    else
      n145_q <= n103_o;
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:50:5  */
  always @(negedge clk or posedge n8_o)
    if (n8_o)
      n146_q <= 1'bZ;
    else
      n146_q <= n85_o;
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:50:5  */
  always @(negedge clk or posedge n8_o)
    if (n8_o)
      n147_q <= 1'b1;
    else
      n147_q <= n87_o;
  /* /home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/own_periphs/spi/spi_master.vhd:50:5  */
  always @(negedge clk or posedge n8_o)
    if (n8_o)
      n148_q <= 8'b00000000;
    else
      n148_q <= n89_o;
endmodule

