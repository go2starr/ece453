module shift_reg(input in,
                 output reg out,
                 input clk
                 );

   parameter size = 2;
   reg       shift_reg [5:0];
   integer   i;
   
   always @(posedge clk) begin
      for (i = 0; i < size - 1; i = i + 1) begin
         shift_reg[i] <= shift_reg[i+1];
      end

      shift_reg[size-1] <= in;

      out <= (shift_reg[0] ^ shift_reg[1]) ? out : shift_reg[0];
   end
endmodule
