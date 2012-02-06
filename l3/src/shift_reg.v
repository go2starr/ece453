module shift_reg(
                 input in,
                 output reg out,
                 input clk
                 );
   reg                 shift_reg [2:0];

   always @(posedge clk) begin
      shift_reg[0] <= shift_reg[1];
      shift_reg[1] <= shift_reg[2];
      shift_reg[2] <= in;

      out = (shift_reg[0] ^ shift_reg[1]) ? out : shift_reg[0];
   end
   

endmodule
