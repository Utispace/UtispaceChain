
#pragma once
#include <libcvcore/Common.h>
#include <libcv/CommonNet.h>
#include <libp2p/Capability.h>
#include <libp2p/HostCapability.h>
#include "Common.h"
#include "PBFTPeer.h"

namespace dev
{
namespace eth
{

class PBFTHost: public p2p::HostCapability<PBFTPeer>
{
public:
	typedef std::function<void(unsigned, std::shared_ptr<p2p::Capability>, RLP const&)> MsgHandler;

	PBFTHost(MsgHandler h): m_msg_handler(h) {}
	virtual ~PBFTHost() {}

	void onMsg(unsigned _id, std::shared_ptr<p2p::Capability> _peer, RLP const& _r) {
		m_msg_handler(_id, _peer, _r);
	}

	void foreachPeer(std::function<bool(std::shared_ptr<PBFTPeer>)> const& _f) const;

private:
	MsgHandler m_msg_handler;
};

}
}
