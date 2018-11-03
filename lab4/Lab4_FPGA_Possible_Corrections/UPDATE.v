module UPDATE (
	RESET,
	Y_ADDR,
	HREF,
	PCLK
);

input RESET;
input HREF;
input PCLK;

output reg [14:0] Y_ADDR;

always @ (posedge PCLK) begin
	if (RESET) begin
		Y_ADDR <= 0;
	end
	else if (!HREF) begin
		Y_ADDR <= Y_ADDR + 1;
	end
end

endmodule

	