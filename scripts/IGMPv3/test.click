//generator::Generator() -> ToDump("test.dump", ENCAP IP) -> Discard;

reporter::Reporter(SRC 192.168.2.1, DST 192.168.1.254 )
	 -> EtherEncap(0x0800, 00:50:BA:85:84:B2, 00:50:BA:85:84:A1)
	 -> ToDump("test.dump")
	 ->Discard;
