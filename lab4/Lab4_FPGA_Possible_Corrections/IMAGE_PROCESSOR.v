`define SCREEN_WIDTH 176
`define SCREEN_HEIGHT 144
`define NUM_BARS 3
`define BAR_HEIGHT 48

module IMAGE_PROCESSOR (
	REDCOUNT,
	BLUECOUNT,
	VGA_VSYNC_NEG,
	COLOR,
	RESET
);


//=======================================================
//  PORT declarations
//=======================================================


input [9:0] BLUECOUNT;
input [9:0] REDCOUNT;
input			VGA_VSYNC_NEG;

output reg [1:0] COLOR;
output reg [1:0] RESET;

//always @ (posedge VGA_VSYNC_NEG) begin
//		if (BLUECOUNT > REDCOUNT) begin
//			COLOR <= 1'b1; //when color is 1, blue
//		end
//		else begin
//			COLOR <= 1'b0; //when color is 0, red
//		end
//		RESET <= 1'b1;
//end


endmodule