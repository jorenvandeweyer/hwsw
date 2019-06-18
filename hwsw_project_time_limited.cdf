/* Quartus Prime Version 16.1.0 Build 196 10/24/2016 SJ Lite Edition */
JedecChain;
	FileRevision(JESD32A);
	DefaultMfr(6E);

	P ActionCode(Cfg)
		Device PartName(5CSEMA5F31) Path("Z:/repos/HWSW/project/") File("hwsw_project_time_limited.sof") MfrSpec(OpMask(1));
	P ActionCode(Ign)
		Device PartName(SOCVHPS) MfrSpec(OpMask(0));

ChainEnd;

AlteraBegin;
	ChainType(JTAG);
AlteraEnd;
