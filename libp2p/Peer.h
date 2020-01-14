

#pragma once

#include "Common.h"

namespace dev
{

namespace p2p
{

class Peer: public Node
{
	friend class Session;		/// Allows Session to update score and rating.
	friend class Host;		/// For Host: saveNetwork(), restoreNetwork()

	friend class RLPXHandshake;

public:
	/// Construct Peer from Node.
	Peer(Node const& _node): Node(_node) {}
	
	bool isOffline() const { return !m_session.lock(); }

	virtual bool operator<(Peer const& _p) const;
	
	/// WIP: Returns current peer rating.
	int rating() const { return m_rating; }
	
	/// Return true if connection attempt should be made to this peer or false if
	bool shouldReconnect() const;
	
	/// Number of times connection has been attempted to peer.
	int failedAttempts() const { return m_failedAttempts; }

	/// Reason peer was previously disconnected.
	DisconnectReason lastDisconnect() const { return m_lastDisconnect; }
	
	/// Peer session is noted as useful.
	void noteSessionGood() { m_failedAttempts = 0; }
	
protected:
	/// Returns number of seconds to wait until attempting connection, based on attempted connection history.
	unsigned fallbackSeconds() const;

	int m_score = 0;									///< All time cumulative.
	int m_rating = 0;									///< Trending.
	
	/// Network Availability
	
	std::chrono::system_clock::time_point m_lastConnected;
	std::chrono::system_clock::time_point m_lastAttempted;
	unsigned m_failedAttempts = 0;
	DisconnectReason m_lastDisconnect = NoDisconnect;	///< Reason for disconnect that happened last.

	/// Used by isOffline() and (todo) for peer to emit session information.
	std::weak_ptr<Session> m_session;
};
using Peers = std::vector<Peer>;

}
}
