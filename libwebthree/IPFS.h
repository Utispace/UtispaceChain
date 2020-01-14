
#pragma once

#include <libdevcore/Exceptions.h>
#include <libdevcore/Common.h>
#include <libdevcore/FixedHash.h>


namespace dev
{

DEV_SIMPLE_EXCEPTION(IPFSExecutableNotFound);
struct IPFSCommunicationError: virtual Exception { public: IPFSCommunicationError(std::string _s): Exception("Communication with ipfs executable failed:  " + _s + ".") {} };

class IPFS
{
public:
	/// Places data into IPFS and @returns SHA2-256 hash of the data.
	h256 putBlockForSHA256(bytesConstRef _data);
	/// Places data into IPFS and @returns multihash of the data.
	bytes putBlock(bytesConstRef _data);

	/// Retrieves data from IPFS SHA2-256 hash is given.
	bytes getBlockForSHA256(h256 const& _sha256);
	/// Retrieves data from IPFS whose multihash is given.
	bytes getBlock(bytesConstRef _multihash);

	static h256 multihashAsSHA256(bytesConstRef _b)
	{
		if (_b.size() == 34 && _b[0] == 0x12 && _b[1] == 0x20)
			return h256(_b.cropped(2));
		throw std::invalid_argument("Given data is not a SHA2-256 multihash");
	}

	static bytes sha256AsMultihash(h256 const& _h)
	{
		return bytes{0x12, 0x20} + _h.asBytes();
	}
};

}
