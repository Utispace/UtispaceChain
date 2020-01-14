

#pragma once

#include <libdevcore/Guards.h>
#include "RLPXFrameCoder.h"
#include "RLPXPacket.h"
namespace ba = boost::asio;
namespace bi = boost::asio::ip;

namespace dev
{
namespace p2p
{

/**
 * RLPFrameReader
 * Reads and assembles RLPX frame byte buffers into RLPX packets. Additionally
 * buffers incomplete packets which are pieced into multiple frames (has sequence).
 */
class RLPXFrameReader
{
public:
	RLPXFrameReader(uint16_t _protocolType): m_protocolType(_protocolType) {}
	
	/// Processes a single frame returning complete packets.
	std::vector<RLPXPacket> demux(RLPXFrameCoder& _coder, RLPXFrameInfo const& _info, bytesRef _frame);
	
protected:
	uint16_t m_protocolType;
	std::map<uint16_t, std::pair<RLPXPacket, uint32_t>> m_incomplete;	///< Sequence: Incomplete packet and bytes remaining.
};

}
}
