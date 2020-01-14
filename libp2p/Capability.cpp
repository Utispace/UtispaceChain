#include "Capability.h"

#include <libdevcore/easylog.h>
#include "Session.h"
#include "Host.h"
using namespace std;
using namespace dev;
using namespace dev::p2p;

Capability::Capability(std::shared_ptr<SessionFace> _s, HostCapabilityFace* _h, unsigned _idOffset, uint16_t _protocolID):
	c_protocolID(_protocolID), m_session(_s), m_hostCap(_h), m_idOffset(_idOffset)
{
	LOG(INFO) << "New session for capability" << m_hostCap->name() << "; idOffset:" << m_idOffset << "; protocolID:" << c_protocolID;
}

void Capability::disable(std::string const& _problem)
{
	LOG(WARNING) << "DISABLE: Disabling capability '" << m_hostCap->name() << "'. Reason:" << _problem;
	m_enabled = false;
}

RLPStream& Capability::prep(RLPStream& _s, unsigned _id, unsigned _args)
{
	return _s.appendRaw(bytes(1, _id + m_idOffset)).appendList(_args);
}

void Capability::sealAndSend(RLPStream& _s)
{
	shared_ptr<SessionFace> session = m_session.lock();
	if (session)
		session->sealAndSend(_s, c_protocolID);
}

void Capability::addRating(int _r)
{
	shared_ptr<SessionFace> session = m_session.lock();
	if (session)
		session->addRating(_r);
}
