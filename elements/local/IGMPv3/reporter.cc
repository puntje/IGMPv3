#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>

#include "reporter.hh"
#include "messages.hh"

#include <clicknet/ether.h>
#include <clicknet/ip.h>

CLICK_DECLS

Reporter::Reporter()
{
}

Reporter::~ Reporter()
{
}

int Reporter::configure(Vector<String> &conf, ErrorHandler *errh)
{
	if (cp_va_kparse(conf, this, errh, "CLIENT_STATES", cpkM, cpElementCast, "IGMPClientStates", &_states, cpEnd) < 0) return -1;
	return 0;
}

void Reporter::push(int, Packet *p)
{
	click_chatter("Report sent of size %d",p->length());
	output(0).push(p);
}

Packet* Reporter::createJoinReport(unsigned int port, unsigned int interface, IPAddress groupAddress, FilterMode filter, set<String> sources)
{
	int headroom = sizeof(click_ether);
	int headerSize = sizeof(click_ip);
	int messageSize = sizeof(struct Report) + sizeof(struct GroupRecord);
	int packetSize = headerSize + messageSize;

	WritablePacket* q = Packet::make(headroom, 0, packetSize, 0);

	if (!q) {
		return 0;
	}

	memset(q->data(), '\0', packetSize);

	click_ip* iph = (click_ip*) q->data();
	iph->ip_v = 4;
	iph->ip_hl = sizeof(click_ip) >> 2;
	iph->ip_len = htons(q->length());
	iph->ip_p = IP_PROTO_IGMP;
	iph->ip_ttl = 1;
	iph->ip_src = _states->_source;
	iph->ip_dst = _states->_destination;
	iph->ip_sum = click_in_cksum((unsigned char*) iph, sizeof(click_ip));

    Report* report = (Report *) (iph + 1);
    report->type = 0x22;
    report->checksum = htons(0);
    report->number_of_group_records = htons(1);  // TODO check for fragmentation needs

	_states->saveSocketState(port, interface, groupAddress, filter, sources);
	_states->saveInterfaceState(port, interface, groupAddress, filter, sources);

    GroupRecord* groupRecord = (GroupRecord* ) (report + 1);
    groupRecord->type = 2;  // TODO
    groupRecord->aux_data_len = 0;
    groupRecord->number_of_sources = htons(0);  // TODO
    groupRecord->multicast_address = groupAddress;

//	groupRecord->source_addresses = _states->_interfaceStates.at(interface).at(0)._sources;
	// TODO source list
	// TODO packetsize
	
	// TODO
	// igmpclientstates::getAmountOfSources
	//_states->_interfaceState.at(interface)

	report->checksum = click_in_cksum((unsigned char*) report, messageSize);

    q->set_dst_ip_anno(_states->_destination);

	return q;
}

int Reporter::leaveGroup(const String &conf, Element* e, void* thunk, ErrorHandler* errh)
{
	Reporter* me = (Reporter *) e;

	// default values for arguments
	unsigned int port = 1234;
	unsigned int interface = 0;
	IPAddress groupAddress = IPAddress("225.1.1.1");
	FilterMode filter = MODE_IS_INCLUDE;
	set<String> sources;

	// overwrite given arguments
	if (cp_va_kparse(conf, me, errh,
			"PORT", cpkN, cpUnsigned, &port,
			"INTERFACE", cpkN, cpUnsigned, &interface,
			"GROUP", cpkN, cpIPAddress, &groupAddress,
			cpEnd) < 0)
		return -1;

	/* // TEST DISABLED TO TEST WHETHER CLIENT IGNORES MULTICAST PACKETS ON OTHER INTERFACES
	if (interface != 0) {
		errh->error("[ERROR IGMPReporter]: invalid INTERFACE value (%u) provided for client with address %s, expected 0", interface, me->_states->_source.unparse().c_str());
		return -1;
	}
	*/

	// TODO verify group address is a valid mcast address

	me->push(0, me->createJoinReport(port, interface, groupAddress, filter, sources));
	return 0;
}

int Reporter::joinGroup(const String &conf, Element* e, void* thunk, ErrorHandler* errh)
{
	Reporter* me = (Reporter *) e;

	// default values for arguments
	unsigned int port = 1234;
	unsigned int interface = 0;
	IPAddress groupAddress = IPAddress("225.1.1.1");
	String sFilter;
	FilterMode filter = MODE_IS_EXCLUDE;
	Vector<String> vSources;
	set<String> sources;

	// overwrite given arguments
	if (cp_va_kparse(conf, me, errh,
			"PORT", cpkN, cpUnsigned, &port,
			"INTERFACE", cpkN, cpUnsigned, &interface,
			"GROUP", cpkN, cpIPAddress, &groupAddress,
			"FILTER", cpkN, cpString, &sFilter,
			"SRC", cpkN, cpArguments, &vSources,
			cpEnd) < 0)
		return -1;

	/* // TEST DISABLED TO TEST WHETHER CLIENT IGNORES MULTICAST PACKETS ON OTHER INTERFACES
	if (interface != 0) {
		errh->error("[ERROR IGMPReporter]: invalid INTERFACE value (%u) provided for client with address %s, expected 0", interface, me->_states->_source.unparse().c_str());
		return -1;
	}
	*/

	// TODO verify group address is a valid mcast address

	if (sFilter == "INCLUDE") {
		filter = MODE_IS_INCLUDE;
	} else if(sFilter != "EXCLUDE" && sFilter != "") {
		errh->error("[ERROR IGMPReporter]: invalid FILTER mode (%s) provided, expected either EXCLUDE or INCLUDE", sFilter.c_str());
		return -1;
	}

	for (int i = 0; i < vSources.size(); i++) {
		sources.insert(vSources.at(i));
	}

	me->push(0, me->createJoinReport(port, interface, groupAddress, filter, sources));
	return 0;
}

void Reporter::add_handlers()
{
	add_write_handler("join_group", &joinGroup, (void *) 0);
	add_write_handler("leave_group", &leaveGroup, (void *) 0);
}


CLICK_ENDDECLS
EXPORT_ELEMENT(Reporter)
