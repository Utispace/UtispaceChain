

#include "Peer.h"
using namespace std;
using namespace dev;
using namespace dev::p2p;

namespace dev
{

namespace p2p
{

bool Peer::shouldReconnect() const
{
	return id && endpoint && chrono::system_clock::now() > m_lastAttempted + chrono::seconds(fallbackSeconds());
}
	
unsigned Peer::fallbackSeconds() const
{
	if (peerType == PeerType::Required)
		return 5;
	switch (m_lastDisconnect)
	{
	case BadProtocol:
		return 30 * (m_failedAttempts + 1);
	case UselessPeer:
	case TooManyPeers:
		return 25 * (m_failedAttempts + 1);
	case ClientQuit:
		return 15 * (m_failedAttempts + 1);
	case NoDisconnect:
	default:
		if (m_failedAttempts < 5)
			return m_failedAttempts ? m_failedAttempts * 5 : 5;
		else if (m_failedAttempts < 15)
			return 25 + (m_failedAttempts - 5) * 10;
		else
			return 25 + 100 + (m_failedAttempts - 15) * 20;
	}
}
	
bool Peer::operator<(Peer const& _p) const
{
	if (isOffline() != _p.isOffline())
		return isOffline();
	else if (isOffline())
		if (m_lastAttempted == _p.m_lastAttempted)
			return m_failedAttempts < _p.m_failedAttempts;
		else
			return m_lastAttempted < _p.m_lastAttempted;
		else
			if (m_score == _p.m_score)
				if (m_rating == _p.m_rating)
					if (m_failedAttempts == _p.m_failedAttempts)
						return id < _p.id;
					else
						return m_failedAttempts < _p.m_failedAttempts;
					else
						return m_rating < _p.m_rating;
					else
						return m_score < _p.m_score;
}

}
}
