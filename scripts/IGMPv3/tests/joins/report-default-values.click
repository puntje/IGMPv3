igmp_client_states::IGMPClientStates(SRC 192.168.2.1, DST 224.0.0.22);

reporter::Reporter(CLIENT_STATES igmp_client_states)
	 -> EtherEncap(0x0800, 00:50:BA:85:84:B2, 00:50:BA:85:84:A1)
	 -> ToDump("report-default-values.dump")
	 -> Discard;
