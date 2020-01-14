
#include "Swarm.h"

#include <libdevcore/easylog.h>
#include <libdevcore/SHA3.h>
#include <libdevcore/Hash.h>
#include <libcvcore/Common.h>
#include <libcv/Client.h>
#include "WebThree.h"
#include "Support.h"
#include "IPFS.h"
using namespace std;
using namespace dev;
using namespace bzz;
using namespace eth;

bzz::Interface::~Interface()
{
}

bzz::Client::Client(WebThreeDirect* _web3):
	m_ipfs(new IPFS)
{
	m_owner = _web3;
}

Pinneds bzz::Client::insertBundle(bytesConstRef _bundle)
{
	LOG(INFO) << "Bundle insert" << sha3(_bundle);

	Pinneds ret;
	RLP rlp(_bundle);
	bool first = true;
	for (auto const& r : rlp)
		if (first)
			first = false;
		else
		{
			LOG(INFO) << "   inserting slice" << sha3(r.toBytesConstRef());
			ret.push_back(Pinned(m_cache[sha3(r.toBytesConstRef())] = make_shared<bytes>(r.toBytes())));
		}
	return ret;
}

Pinned bzz::Client::put(bytes const& _data)
{
	h256 ret = sha3(_data);
	LOG(INFO) << "Inserting" << ret;

	if (!m_cache.count(ret))
		m_cache[ret] = make_shared<bytes>(_data);

	if (m_putAccount)
	{
		// send to IPFS...
		h256 sha256hash = sha256(&_data);
		LOG(INFO) << "IPFS-inserting" << sha256hash;

		// set in blockchain
		try
		{
			m_owner->support()->hintSHA256(ret, sha256hash, m_putAccount);
			m_ipfs->putBlockForSHA256(&_data);
		}
		catch (InterfaceNotSupported&) {}
	}

	return Pinned(m_cache[ret]);
}

Pinned bzz::Client::get(h256 const& _hash)
{
	LOG(INFO) << "Looking up" << _hash;
	auto it = m_cache.find(_hash);
	if (it != m_cache.end())
		return it->second;

	if (u256 sha256hash = m_owner->support()->sha256Hint(_hash))
	{
		LOG(INFO) << "IPFS Searching" << sha256hash;
		auto b = m_ipfs->getBlockForSHA256(sha256hash);
		if (!b.empty())
			return (m_cache[_hash] = make_shared<bytes>(b));
	}

	LOG(INFO) << "Not found" << _hash;
	throw ResourceNotAvailable();
}
