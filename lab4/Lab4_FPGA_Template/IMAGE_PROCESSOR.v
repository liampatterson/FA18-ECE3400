`define SCREEN_WIDTH 176
`define SCREEN_HEIGHT 144
`define NUM_BARS 3
`define BAR_HEIGHT 48

module IMAGE_PROCESSOR (
	PIXEL_IN,
	CLK,
	VGA_PIXEL_X,
	VGA_PIXEL_Y,
	VGA_VSYNC_NEG,
	RESULT
);


//=======================================================
//  PORT declarations
//=======================================================
input	[7:0]	PIXEL_IN;
input 		CLK;

input [9:0] VGA_PIXEL_X;
input [9:0] VGA_PIXEL_Y;
input			VGA_VSYNC_NEG;

output [8:0] RESULT;

always @ (posedge VGA_VSYNC_NEG) begin
	// average pixel data
	if (PIXEL_IN[7:5]==3'b111) begin //red
		RESULT = 1'b1;
	end
	else if (PIXEL_IN[1:0]==2'b11) begin //blue
		RESULT = 1'b0;
	end
end

endmodule