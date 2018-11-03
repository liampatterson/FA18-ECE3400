`define SCREEN_WIDTH 176
`define SCREEN_HEIGHT 144

module DE0_NANO(
   CLOCK_50,
	GPIO_0_D,
	GPIO_1_D,
	KEY 

);





//=======================================================
//  PARAMETER declarations
//=======================================================
localparam RED = 8'b111_000_00;
localparam GREEN = 8'b000_111_00;
localparam BLUE = 8'b000_000_11;

//=======================================================
//  PORT declarations
//=======================================================

//////////// CLOCK //////////
input CLOCK_50;
wire PCLK = GPIO_1_D[32];
wire HREF = GPIO_1_D[30];
wire VSYNC = GPIO_1_D[31];

wire c0_sig;
wire c1_sig;
wire c2_sig;

sweetPLL	sweetPLL_inst (
	.inclk0 ( CLOCK_50 ),
	.c0 ( c0_sig ), //24
	.c1 ( c1_sig ), //25
	.c2 ( c2_sig )  //50 - phase synchronized with c0 and c1  
	);





//////////// GPIO_0, GPIO_0 connect to GPIO Default //////////
output 		    [33:0]		GPIO_0_D;
//////////// GPIO_0, GPIO_1 connect to GPIO Default //////////
input 		    [33:0]		GPIO_1_D;
input 		     [1:0]		KEY;

///// PIXEL DATA /////
reg [7:0]	pixel_data_RGB332 = 8'd0;

///// READ/WRITE ADDRESS /////
wire [14:0] X_ADDR;
wire [14:0] Y_ADDR;
wire [14:0] WRITE_ADDRESS;
reg [14:0] READ_ADDRESS; 

assign WRITE_ADDRESS = X_ADDR + Y_ADDR*(`SCREEN_WIDTH);

///// VGA INPUTS/OUTPUTS /////
wire 			VGA_RESET;
wire [7:0]	VGA_COLOR_IN;
wire [9:0]	VGA_PIXEL_X;
wire [9:0]	VGA_PIXEL_Y;
wire [7:0]	MEM_OUTPUT;
wire			VGA_VSYNC_NEG;
wire			VGA_HSYNC_NEG;
reg			VGA_READ_MEM_EN;

assign GPIO_0_D[5] = VGA_VSYNC_NEG;
assign VGA_RESET = ~KEY[0];
assign GPIO_0_D[1] = c0_sig;


///// I/O for Img Proc /////
wire [1:0] COLOR;
wire [1:0] RESET;

/* WRITE ENABLE */
reg W_EN;

wire [9:0] REDCOUNT;
wire [9:0] BLUECOUNT;


///////* M9K Module *///////
Dual_Port_RAM_M9K mem(
	.input_data(pixel_data_RGB332),
	.w_addr(WRITE_ADDRESS),
	.r_addr(READ_ADDRESS),
	.w_en(W_EN),
	.clk_W(c2_sig),
	.clk_R(c1_sig), // DO WE NEED TO READ SLOWER THAN WRITE??
	.output_data(MEM_OUTPUT)
);

///////* VGA Module *///////
VGA_DRIVER driver (
	.RESET(VGA_RESET),
	.CLOCK(c1_sig),
	.PIXEL_COLOR_IN(VGA_READ_MEM_EN ? MEM_OUTPUT : BLUE),
	.PIXEL_X(VGA_PIXEL_X),
	.PIXEL_Y(VGA_PIXEL_Y),
	.PIXEL_COLOR_OUT({GPIO_0_D[9],GPIO_0_D[11],GPIO_0_D[13],GPIO_0_D[15],GPIO_0_D[17],GPIO_0_D[19],GPIO_0_D[21],GPIO_0_D[23]}),
   .H_SYNC_NEG(GPIO_0_D[7]),
   .V_SYNC_NEG(VGA_VSYNC_NEG)
);




///////* Image Processor *///////
IMAGE_PROCESSOR proc(
	.REDCOUNT(REDCOUNT),
	.BLUECOUNT(BLUECOUNT),
	.VGA_VSYNC_NEG(VGA_VSYNC_NEG),
	.COLOR(COLOR),
	.RESET(RESET)
);

IMAGE_PROCESSOR_COUNT count (
	.PIXEL_IN (MEM_OUTPUT),
	.CLK(c1_sig),
	.RESET(RESET),
	.VGA_PIXEL_X(VGA_PIXEL_X),
	.VGA_PIXEL_Y(VGA_PIXEL_Y),
	.REDCOUNT(REDCOUNT),
	.BLUECOUNT(BLUECOUNT)
);

UPDATE up (
	.RESET(VSYNC),
	.X_ADDR(X_ADDR),
	.Y_ADDR(Y_ADDR),
	.HREF(HREF),
	.PCLK(PCLK)
);


///////* Update Read Address *///////
//buffer reader
always @ (*) begin
		READ_ADDRESS = (VGA_PIXEL_X + VGA_PIXEL_Y*`SCREEN_WIDTH);
		if((VGA_PIXEL_X>`SCREEN_WIDTH-1) || VGA_PIXEL_Y>(`SCREEN_HEIGHT-1)) begin 
				VGA_READ_MEM_EN = 1'b0;
		end
		else begin
//				if (VGA_PIXEL_X==VGA_PIXEL_Y) begin
//					pixel_data_RGB332 = RED;
//				end
//				else begin
//					pixel_data_RGB332 = GREEN;
//				end
				VGA_READ_MEM_EN = 1'b1;
		end
end


//downsampler
reg cycle = 1'b0;
reg [15:0] cameradata;


always @ (posedge PCLK) begin 
	//if (~VSYNC) begin 
		if (HREF) begin
			if (!cycle ) begin
				cameradata[15:8] = {GPIO_1_D[28], GPIO_1_D[29], GPIO_1_D[22], GPIO_1_D[23], GPIO_1_D[24], GPIO_1_D[25], GPIO_1_D[26], GPIO_1_D[27]};
				cycle = 1'b1;
				W_EN = 0;
			end
			else if (cycle) begin
				cameradata[7:0] = {GPIO_1_D[28], GPIO_1_D[29], GPIO_1_D[22], GPIO_1_D[23], GPIO_1_D[24], GPIO_1_D[25], GPIO_1_D[26], GPIO_1_D[27]};
				pixel_data_RGB332[7:0] = {cameradata[15:13], cameradata[10:8], cameradata[4:3]};
				cycle = 1'b0;
				W_EN = 1;
			end
		end
	//end
end
	
endmodule 