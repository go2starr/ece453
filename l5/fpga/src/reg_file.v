/* regFile.v - Basic register file */
module reg_file(address,
                data_in,
                data_out,
                ws_n,
                rs_n,
                be,
                clk,
                as,
                rst_n);
   // Inputs
   input [23:0] address;        // Arm address
   input [31:0] data_in;        // Data input
   input        ws_n;           // Write strobe
   input        rs_n;           // Read strobe
   input [3:0]  be;             // Byte enable
   input        clk;            // Clock
   input        as;             // Chip select
   input        rst_n;          // Reset

   // Outputs
   output reg [31:0] data_out;  // Data output
   
   // Register file size
   parameter REG_FILE_SIZE      = 8; // Register file size
   parameter LOG_REG_FILE_SIZE  = 3; // Number of internal select bits

   // Internal regs
   reg [LOG_REG_FILE_SIZE - 1:0] select; // Address decoding   
   reg [31:0]                    registers [REG_FILE_SIZE - 1:0]; // The registers

   // Address locations
   parameter A_RAND = 0;

   // Address decode
   always @(address) begin
      case (address)
        A_RAND:  select = A_RAND;
      endcase
   end
   
   // Clocking registers
   always @(posedge clk or negedge rst_n) begin
      // Reset
      if (~rst_n) begin
         data_out <= 32'hfee1dead;
      end

      // Chip selected
      else if (as) begin
         // Read
         if (~rs_n) begin
            // Random
            if (select == A_RAND) begin
               data_out <= registers[select];
            end
         end

         // Write
         else if (~ws_n) begin
            if (select == A_RAND) begin
               registers[select] <= data_in;
            end
         end
      end // if (as)
   end 
endmodule // regFile

  
       
   
   
   
   
   
