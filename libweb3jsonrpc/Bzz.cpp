
#include <libdevcore/SHA3.h>
#include <libdevcore/CommonData.h>
#include <libwebthree/Swarm.h>
#include <libdevcore/CommonIO.h>
#include <libcvcore/CommonJS.h>
#include <libdevcore/TrieCommon.h>	// TODO: bytes ostream operator doesn't work without this import
#include "Bzz.h"

using namespace std;
using namespace dev;
using namespace dev::rpc;

Bzz::Bzz(dev::bzz::Interface& _bzz): m_bzz(_bzz){}

std::string Bzz::bzz_put(std::string const& _data)
{
	bytes b = jsToBytes(_data);
	m_bzz.put(b);
	return toJS(sha3(b));
}

std::string Bzz::bzz_get(std::string const& _hash)
{
	return toJS(static_cast<bytesConstRef>(m_bzz.get(jsToFixed<32>(_hash))));
}