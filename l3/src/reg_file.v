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
   

   
   // Address decoding
   reg [3:0]         select;

   parameter A_RW_COUNT = 4'd0;
   parameter A_SCRATCH1 = 4'd1;
   parameter A_SCRATCH2 = 4'd2;
   parameter A_SCRATCH3 = 4'd3;
   parameter A_SCRATCH4 = 4'd4;      
   parameter A_IO_DIR   = 4'd5;
   parameter A_WRITE    = 4'd6;
   parameter A_READ     = 4'd7;
   parameter A_JUNK     = 4'd8;
   
   always begin
      case (addr)
	24'h0: select = A_RW_COUNT;
	24'h4: select = A_SCRATCH1;
	24'h8: select = A_SCRATCH2;
	24'hc: select = A_SCRATCH3;
	24'h10: select = A_SCRATCH4;
        24'h14: select = A_IO_DIR;
        24'h18: select = A_WRITE;
        24'h1C: select = A_READ;        
	default: select = A_JUNK;
      endcase
   end
   
   // Reg file
   reg [31:0]    RF[5:0];
   
   // Counts
   reg [15:0]    write_count, read_count;
   
   always @(posedge clk or negedge rst) begin
      //reset
      if (~rst)
	begin
	   dout <= 32'h0;
	end
      
      else if (as) 
	begin 
	   // Read
	   if (~rs_n) 
	     begin
                case (select)
                  A_RW_COUNT:
		    dout <= { write_count, read_count };
                  A_SCRATCH1:
		    dout <= RF[select];
                  A_SCRATCH2:
		    dout <= RF[select];
                  A_SCRATCH3:
		    dout <= RF[select];
                  A_SCRATCH4:
		    dout <= RF[select];
                  A_IO_DIR:
                    dout <= RF[select];
                  A_READ:
                    dout <= 1+1;        // TODO
                  A_WRITE:
                    dout <= 1+1;        // TODO
                endcase
	     end

	   // Write
	   else if (~ws_n) 
	     begin

                // RW_COUNT is read only
                if (select == A_RW_COUNT) begin
                end

                // SCRATCH registers
                else if (select == A_SCRATCH1 ||
                         select == A_SCRATCH2 ||
                         select == A_SCRATCH3 ||
                         select == A_SCRATCH4) begin
		   case (be)
		     4'd0: RF[select] <= din;
		     4'd3: RF[select] <= {RF[select][31:16], din[15:0]};
		     4'd7: RF[select] <= {RF[select][31:8],  din[7:0] };
		   endcase
		end

                else if (select == A_IO_DIR) begin
                   // TODO
                end

                else if (select == A_WRITE) begin
                   // TODO
                end

                else if (select == A_READ) begin
                   // TODO
                end
	     end // if (~ws_n)
	end // if (as)
   end // always @ (posedge clk or negedge rst)
   
   
   // Read count
   always @(negedge rs_n, negedge rst) begin
      if (~rst) begin
	 read_count = 0;
      end
      // if write/read to scratch
      else if (as && select > 0 && select < 5 && ws_n && be[3]) begin
	 read_count = read_count + 1;
      end		
   end			
   
   
   // Write count
   always @(posedge ws_n, negedge rst) begin
      if (~rst) begin
	 write_count = 0;
      end	
      // if write/read to scratch
      else if (as && select > 0 && select < 5 && ~be[3] && rs_n) begin
	 write_count = write_count + 1;
      end				
   end


endmodule // reg_file
