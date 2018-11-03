module CAMERA(DATA, PCLK, VSYNC, HREF, W_EN, PIXEL_COLOR, X, Y);

input [7:0] DATA;
input PCLK;
input VSYNC;
input HREF;

output [7:0] PIXEL_COLOR;
output W_EN;
output X;
output Y;
reg W_EN = 1'b0;

reg CYCLE = 1'b0;
reg [7:0] TEMP;
reg [7:0] PIXEL_COLOR;
reg [9:0] X = 10'b0;
reg [9:0] Y = 10'b0; 
reg INC_Y = 0;

	always @(posedge PCLK) begin 
		if (~VSYNC) begin 
			if (HREF) begin  	
				if (CYCLE == 1'b0) begin
					TEMP <= DATA;
					CYCLE <= 1'b1;
					W_EN <= 1'b0;
					X <= X;
					INC_Y <= 1'b0;
				end 
				else begin

					PIXEL_COLOR[7:5] <= TEMP[7:5];
					PIXEL_COLOR[4:2] <= TEMP[2:0];
					PIXEL_COLOR[1:0] <= DATA[4:3];

//					PIXEL_COLOR[7:5] <= 3'b111;
//					PIXEL_COLOR[4:2] <= 3'b000;
//					PIXEL_COLOR[1:0] <= 2'b00;

//						if(Y < 45) begin
//							PIXEL_COLOR = 8'b111_111_11;
//						end
//						else begin
//							PIXEL_COLOR = 8'b111_000_00;
//						end
				

					W_EN <= 1'b1;
					CYCLE <= 1'b0; 
					X <= X + 10'b0000000001; // inc X count	
					INC_Y <= 1'b0;
					
				end
			end	
			else begin
			if (INC_Y == 1'b0) begin
				Y <= Y + 10'b0000000001; // inc Y count
				INC_Y <= 1'b1;
			end
			else begin
				Y <= Y;
			end
				
				X <= 10'b0; // reset X count 
			end
		end 
		else begin
			X <= 10'b0;
			Y <= 10'b0;
		end
	end 
	
endmodule