

#include "PBFTHost.h"
using namespace std;
using namespace dev;
using namespace dev::eth;
using namespace p2p;

void PBFTHost::foreachPeer(std::function<bool(std::shared_ptr<PBFTPeer>)> const& _f) const
{
	//order peers by protocol, rating, connection age
	auto sessions = peerSessions();
	auto sessionLess = [](std::pair<std::shared_ptr<SessionFace>, std::shared_ptr<Peer>> const & _left, std::pair<std::shared_ptr<SessionFace>, std::shared_ptr<Peer>> const & _right)
	{ return _left.first->rating() == _right.first->rating() ? _left.first->connectionTime() < _right.first->connectionTime() : _left.first->rating() > _right.first->rating(); };

	std::sort(sessions.begin(), sessions.end(), sessionLess);
	for (auto s : sessions)
		if (!_f(capabilityFromSession<PBFTPeer>(*s.first)))
			return;

	static const unsigned c_oldProtocolVersion = 62;
	sessions = peerSessions(c_oldProtocolVersion); //TODO: remove once v61+ is common
	std::sort(sessions.begin(), sessions.end(), sessionLess);
	for (auto s : sessions)
		if (!_f(capabilityFromSession<PBFTPeer>(*s.first, c_oldProtocolVersion)))
			return;
}


