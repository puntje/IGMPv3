#ifndef CLICK_IGMPROUTERSTATES_HH
#define CLICK_IGMPROUTERSTATES_HH

#include <click/element.hh>
#include <click/vector.hh>
#include <click/hashtable.hh>
#include "../states/routerrecord.hh"
#include "../utils/filtermode.hh"

CLICK_DECLS

struct GroupTimerState;

class IGMPRouterStates : public Element {
public:
	IGMPRouterStates();
	~IGMPRouterStates();

	const char *class_name() const  { return "IGMPRouterStates"; }
	const char *port_count() const  { return "0/0"; }
	const char *processing() const  { return PUSH; }
	int configure(Vector<String>&, ErrorHandler*);

	void push(int, Packet*);

	void updateCurrentState(unsigned int interface, IPAddress groupAddress, unsigned int filter, Vector<IPAddress> sources);
	QUERY_MODE updateFilterChange(unsigned int interface, IPAddress groupAddress, unsigned int filter, Vector<IPAddress> sources);

	bool isMulticastAllowed(unsigned int interface, IPAddress group, IPAddress source);
    double codeToSeconds(unsigned int code);

	void add_handlers();
	/**
	 * read handlers
	 */
	static String recordStates(Element* e, void* thunk);
    static String getQRV(Element* e, void* thunk);
    static String getQIC(Element* e, void* thunk);
    static String getQRI(Element* e, void* thunk);
    static String getGMI(Element* e, void* thunk);
    static String getSQIC(Element* e, void* thunk);
    static String getSQC(Element* e, void* thunk);
    static String getLMQI(Element* e, void* thunk);
    static String getLMQC(Element* e, void* thunk);
    static String getLMQT(Element* e, void* thunk);

	/**
	 * write handlers
	 */
    static int setQRV(const String& conf, Element* e, void* thunk, ErrorHandler* errh);
    static int setQIC(const String& conf, Element* e, void* thunk, ErrorHandler* errh);
    static int setQRI(const String& conf, Element* e, void* thunk, ErrorHandler* errh);
    static int setLMQI(const String& conf, Element* e, void* thunk, ErrorHandler* errh);
    static int setLMQC(const String& conf, Element* e, void* thunk, ErrorHandler* errh);

	/**
	 * data members
	 */
	IPAddress _source;
	IPAddress _destination;
    // per interface, per group
	Vector<HashTable<IPAddress, RouterRecord> > _records;
    Vector<HashTable<IPAddress, Timer*> > _groupTimers;
    Vector<HashTable<IPAddress, GroupTimerState*> > _groupTimerStates;

    unsigned int _qrv; 
    unsigned int _qic;
    unsigned int _qri;
    unsigned int _sqic;
    unsigned int _sqc;
    unsigned int _lmqi;
    unsigned int _lmqc;

private:
	static void handleExpiryGroup(Timer*, void* data);
    void expireGroup(GroupTimerState* timerState);

	// collects all source addresses either from _forwardingSet or _blockingSet depening on given filter for a router record with given interface and groupAddress
	Vector<IPAddress> getSourceAddresses(unsigned int interface, IPAddress groupAddress, FilterMode filter);
	Vector<SourceRecord> transformToSourceRecords(Vector<IPAddress> a);
	void removeSourceRecords(Vector<SourceRecord>& records, Vector<IPAddress> x);

    double computeLMQT();
    int computeGMI();
    int computeRemainingGMI(int interface, IPAddress group);
    void scheduleGMI(int interface, IPAddress groupAddress, int delay);
};

struct GroupTimerState {
    IGMPRouterStates* me;

    int interface;
    IPAddress group;
    FilterMode filter;
};


CLICK_ENDDECLS
#endif
