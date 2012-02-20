module bdu(addr,
           data,
           ws_n,
           rs_n,
           be,
           clk,
           as,
           rst_n
           );

   input [23:0] addr;
   inout [31:0] data;
   input [3:0]  be;
   input        ws_n, rs_n, as;
   input        clk, rst_n;

   // Clocked output from shift registers
   wire [23:0]                            addr_clkd;
   wire [3:0]                             be_clkd;
   wire [31:0]                            d_clkd;
   wire                                   rs_clkd;
   wire                                   ws_clkd;
   wire                                   as_clkd;

   genvar                                 i;

   // ARM_A
   generate 
      for (i = 0; i < 24; i = i + 1) begin :address_gen
         shift_reg sr (.in(addr[i]), .out(addr_clkd[i]), .clk(clk));
      end
   endgenerate

   // ARM_D
   generate
      for (i = 0; i < 32; i = i +1) begin :data_gen
         shift_reg sr (.in(data[i]), .out(d_clkd[i]), .clk(clk));
      end
   endgenerate

   // ARM_BE_B
   generate 
      for (i = 0; i < 4; i = i + 1) begin :be_gen
         shift_reg sr (.in(be[i]), .out(be_clkd[i]), .clk(clk));
      end
   endgenerate

   // CPLD
   shift_reg #(.DELAY(1)) sr_rs  (.in(rs), .out(rs_clkd), .clk(clk));
   shift_reg #(.DELAY(1)) sr_ws  (.in(ws), .out(ws_clkd), .clk(clk));
   shift_reg #(.DELAY(1)) sr_as  (.in(as), .out(as_clkd), .clk(clk));   
   
   // Data INOUT
   wire [31:0] dout;
   assign data = (~rs_clkd && as_clkd) ? dout : 32'bz;
   
   reg_file myRegFile (.addr(addr_clkd), .din(d_clkd), .dout(dout), .ws_n(ws_clkd),
                       .rs_n(rs_clkd), .be(be_clkd), .clk(clk),
                       .as(as_clkd), .rst(rst_n));
   
   
   
endmodule // bdu
