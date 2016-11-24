#ifndef CLICK_REPORTER_HH
#define CLICK_REPORTER_HH

#include <click/element.hh>
#include <set>
#include "igmpclientstates.hh"

using std::set;

CLICK_DECLS

class Reporter: public Element {
public:
	Reporter();
	~Reporter();

	const char *class_name() const	{ return "Reporter"; }
	const char *port_count() const	{ return "1/1"; }
	const char *processing() const	{ return PUSH; }
	int configure(Vector<String>&, ErrorHandler*);

	void push(int, Packet*);

	/**
	 * handlers
	 */
	static int joinGroup(const String &conf, Element* e, void* thunk, ErrorHandler* errh);
	static int leaveGroup(const String &conf, Element* e, void* thunk, ErrorHandler* errh);
	void add_handlers();

private:
	void saveStates(unsigned int port, unsigned int interface, IPAddress groupAddress, FilterMode filter, set<String> sources);
	void reportCurrentState();
	void reportFilterModeChange(unsigned int port, unsigned int interface, IPAddress groupAddress, FilterMode filter, set<String> sources);
	// void reportSourceListChange(unsigned int port, unsigned int interface, IPAddress groupAddress, FilterMode filter, set<String> sources);

	IGMPClientStates* _states;
};


CLICK_ENDDECLS
#endif
