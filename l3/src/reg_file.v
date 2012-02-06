module reg_file(addr,
                data,
                ws_n,
                rs_n,
                be,
                clk,
                as,
					 rst);



   // Inputs/Outputs
   input [23:0]  addr;
   inout [31:0]  data;
   input         ws_n, rs_n, clk, as;
   input [3:0]   be;
	input				rst;
	
	reg [2:0] select;

	always begin
		case (addr)
			24'h0: select = 3'd0;
			24'h4: select = 3'd1;
			24'h8: select = 3'd2;
			24'hc: select = 3'd3;
			24'h10: select = 3'd4;
			default: select = 3'd5;
		endcase
	end
   
	// Reg file
   reg [31:0]    RF[5:0];
	
	// Counts
	reg [15:0]    write_count, read_count;
	
   // Data output
   reg [31:0]    data_out;

   // Data Out is high impedence unless a read cycle
   assign data = (~rs_n && as) ? data_out : 32'bz;
	
	always @(posedge clk or negedge rst) begin
		if (~rst)
		begin
			data_out <= 32'h0;
		end

		else if (as) 
		begin
			if (~rs_n) 
			begin
				if (select == 0) begin
					data_out <= { write_count, read_count };
				end else begin
					data_out <= RF[select];
				end
			end
			else if (~ws_n) 
			begin
				if (select != 0) begin
					case (be)
						4'd0: RF[select] <= data;
						4'd3: RF[select] <= {RF[select][31:16], data[15:0]};
						4'd7: RF[select] <= {RF[select][31:8],  data[7:0] };
					endcase
				end	
			end	
		end
	end
	
	
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
