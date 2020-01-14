
#include <libp2p/Session.h>
#include <libp2p/Host.h>
#include "PBFTPeer.h"
#include "PBFTHost.h"
using namespace std;
using namespace dev;
using namespace eth;
using namespace p2p;

PBFTPeer::PBFTPeer(std::shared_ptr<SessionFace> _s, HostCapabilityFace* _h, unsigned _i, CapDesc const& _cap, uint16_t _capID):
	Capability(_s, _h, _i, _capID),
	m_peerCapabilityVersion(_cap.second)
{
	//session()->addNote("manners", isRude() ? "RUDE" : "nice");
}

PBFTPeer::~PBFTPeer()
{
}

bool PBFTPeer::interpret(unsigned _id, RLP const& _r)
{
	dynamic_cast<PBFTHost&>(*hostCapability()).onMsg(_id, dynamic_pointer_cast<PBFTPeer>(shared_from_this()), _r);
	return true;
}