
#pragma once

#include "Common.h"
#include "HostCapability.h"

namespace dev
{
namespace p2p
{

class ReputationManager;

class Capability: public std::enable_shared_from_this<Capability>
{
	friend class Session;

public:
	Capability(std::shared_ptr<SessionFace> _s, HostCapabilityFace* _h, unsigned _idOffset, uint16_t _protocolID);
	virtual ~Capability() {}

	// Implement these in the derived class.
	/*	static std::string name() { return ""; }
		static u256 version() { return 0; }
		static unsigned messageCount() { return 0; }
	*/
	std::shared_ptr<SessionFace> session() const { return m_session.lock(); }

	HostCapabilityFace* hostCapability() const { return m_hostCap; }

	void disable(std::string const& _problem);

	RLPStream& prep(RLPStream& _s, unsigned _id, unsigned _args = 0);
	void sealAndSend(RLPStream& _s);
	void addRating(int _r);

protected:
	virtual bool interpret(unsigned _id, RLP const&) = 0;

	uint16_t const c_protocolID;

private:
	std::weak_ptr<SessionFace> m_session;
	HostCapabilityFace* m_hostCap;
	bool m_enabled = true;
	unsigned m_idOffset;
};

}
}
