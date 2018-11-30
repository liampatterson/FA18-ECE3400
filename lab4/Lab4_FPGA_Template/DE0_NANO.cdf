/* Quartus Prime Version 16.1.2 Build 203 01/18/2017 SJ Lite Edition */
JedecChain;
	FileRevision(JESD32A);
	DefaultMfr(6E);

	P ActionCode(Cfg)
		Device PartName(EP4CE22F17) Path("C:/Users/patte/Dropbox/School 2018-2019/FA/ECE 3400/Project/lab4/Lab4_FPGA_Template/") File("DE0_NANO.sof") MfrSpec(OpMask(1));
	P ActionCode(Cfg)
		Device PartName(EP4CE22) Path("C:/Users/patte/Dropbox/School 2018-2019/FA/ECE 3400/Project/lab4/Lab4_FPGA_Template/") File("output_file.jic") MfrSpec(OpMask(1) SEC_Device(EPCS64) Child_OpMask(1 1));

ChainEnd;

AlteraBegin;
	ChainType(JTAG);
AlteraEnd;
