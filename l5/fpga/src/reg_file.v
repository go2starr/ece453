module reg_file(addr,
                din,
		dout,
                ws_n,
                rs_n,
                be,
                clk,
                as,
		rst);

   // Inputs/Outputs
   input [23:0]  addr;
   input [31:0]  din;
   output reg [31:0] dout;
   input             ws_n, rs_n, clk, as;
   input [3:0]       be;
   input             rst;

   // Read/Write state
   reg               rw_cycle;

   // Address decoding
   reg [3:0]         select;
   
   parameter A_RAND     = 4'd0; // r/w
   parameter A_JUNK     = 4'd10;
   
   always begin
      case (addr)
	24'h0:     select = A_RAND;
	default:   select = A_JUNK;
      endcase
   end
   
   // Reg file
   reg [31:0]    RF[10:0];
   
   always @(posedge clk or negedge rst) begin
      //reset
      if (~rst) begin
         dout <= 32'hfee1dead;
         rw_cycle <= 1'b0;
         RF[A_RAND] <= 32'b0;
      end
      
      else if (as) begin
	 // Read
	 if (~rs_n) begin
            if (select == A_RAND) begin
               dout <= RF[A_RAND];
               RF[A_RAND] <= RF[A_RAND] + 1;
               //RF[A_RAND] <= (RF[A_RAND] >> 1) ^ (-(RF[A_RAND] & 1) & 32'h80200003);
            end else begin
               dout <= 32'hbeefeade;
            end // if (~rs_n)
         end
         
	 // Write
         else if (~ws_n) begin
            if (select == A_RAND) begin
               RF[A_RAND] <= din;
            end
	 end // if (as)
         
         // Reset
         else begin
            rw_cycle <= 1'b0;
         end
      end // if (as)
   end // always @ (posedge clk or negedge rst)
endmodule // reg_file
