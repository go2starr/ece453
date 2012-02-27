/* shift_reg.v - basic shift register */
module shift_reg(input in,
                 output reg out,
                 input clk
                 );

   parameter SIZE = 3;
   parameter DELAY = 0;
   
   reg       shift_reg [5:0];
   integer   i;
   
   always @(posedge clk) begin
      for (i = 0; i < SIZE+DELAY - 1; i = i + 1) begin
         shift_reg[i] <= shift_reg[i+1];
      end

      shift_reg[SIZE+DELAY-1] <= in;

      out <= (shift_reg[0] ^ shift_reg[1]) ? out : shift_reg[0];
   end
endmodule
