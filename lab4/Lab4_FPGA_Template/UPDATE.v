module UPDATE (
	RESET,
	X_ADDR,
	Y_ADDR,
	HREF,
	PCLK
);

input RESET;
input HREF;
input PCLK;

output reg [14:0] X_ADDR;
output reg [14:0] Y_ADDR;

always @ (posedge PCLK) begin
	if (RESET) begin
		X_ADDR <= 0;
		Y_ADDR <= 0;
	end
	else if (!HREF) begin
		X_ADDR <= 0;
		Y_ADDR <= Y_ADDR + 1;
	end
	else begin
		X_ADDR <= X_ADDR + 1;
		Y_ADDR <= Y_ADDR;
	end
end


endmodule

	