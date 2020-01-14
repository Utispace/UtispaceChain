

#pragma once

#include <deque>
#include <libdevcore/Guards.h>
#include "RLPXFrameCoder.h"
#include "RLPXPacket.h"
namespace ba = boost::asio;
namespace bi = boost::asio::ip;

namespace dev
{
namespace p2p
{


class RLPXFrameWriter
{

	struct WriterState
	{
		std::deque<RLPXPacket> q;
		mutable Mutex x;
		
		RLPXPacket* writing = nullptr;
		size_t remaining = 0;
		bool multiFrame = false;
		uint16_t sequence = -1;
	};
	
public:
	enum PacketPriority { PriorityLow = 0, PriorityHigh };
	static const uint16_t EmptyFrameLength;
	static const uint16_t MinFrameDequeLength;

	RLPXFrameWriter(uint16_t _protocolType): m_protocolId(_protocolType) {}
	RLPXFrameWriter(RLPXFrameWriter const& _s): m_protocolId(_s.m_protocolId) {}
	
	/// Returns total number of queued packets. Thread-safe.
	size_t size() const { size_t l = 0; size_t h = 0; DEV_GUARDED(m_q.first.x) h = m_q.first.q.size(); DEV_GUARDED(m_q.second.x) l = m_q.second.q.size(); return l + h; }
	
	/// Moves @_payload output to queue, to be muxed into frames by mux() when network buffer is ready for writing. Thread-safe.
	void enque(uint8_t _packetType, RLPStream& _payload, PacketPriority _priority = PriorityLow);

	/// Returns number of packets framed and outputs frames to o_bytes. Not thread-safe.
	size_t mux(RLPXFrameCoder& _coder, unsigned _size, std::deque<bytes>& o_toWrite);
	
	/// Moves @_p to queue, to be muxed into frames by mux() when network buffer is ready for writing. Thread-safe.
	void enque(RLPXPacket&& _p, PacketPriority _priority = PriorityLow);

	uint16_t protocolId() const { return m_protocolId; }
	
private:
	uint16_t const m_protocolId;
	std::pair<WriterState, WriterState> m_q;		// High, Low frame queues
	uint16_t m_sequenceId = 0;				// Sequence ID
};

}
}
