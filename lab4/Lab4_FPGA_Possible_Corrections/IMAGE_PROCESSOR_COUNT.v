`define SCREEN_WIDTH 176
`define SCREEN_HEIGHT 144
`define NUM_BARS 3
`define BAR_HEIGHT 48

module IMAGE_PROCESSOR_COUNT (
	PIXEL_IN,
	CLK,
	RESET,
	VGA_PIXEL_X,
	VGA_PIXEL_Y,
	REDCOUNT,
	BLUECOUNT
);


//=======================================================
//  PORT declarations
//=======================================================
input	[7:0]	PIXEL_IN;
input 		CLK;

input [9:0] VGA_PIXEL_X;
input [9:0] VGA_PIXEL_Y;
input [1:0] RESET;

output reg [9:0] REDCOUNT;
output reg [9:0] BLUECOUNT;


//always @ (posedge CLK) begin
//	if (RESET) begin
//		REDCOUNT <= 9'b0;
//		BLUECOUNT <= 9'b0;
//	end
//	if (VGA_PIXEL_Y > 9'b001000110 && VGA_PIXEL_Y < 001001011) begin
//		if (PIXEL_IN[7:5]==3'b111) begin //red
//			REDCOUNT <= REDCOUNT + 1'b1;
//		end
//		else if (PIXEL_IN[1:0]==2'b11) begin //blue
//			BLUECOUNT <= BLUECOUNT + 1'b1;
//		end
//	end
//end

endmodule