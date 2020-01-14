
#include "Precompiled.h"
#include <libdevcore/easylog.h>
#include <libdevcore/SHA3.h>
#include <libdevcore/Hash.h>
#include <libdevcrypto/Common.h>
#include <libcvcore/Common.h>
using namespace std;
using namespace dev;
using namespace dev::eth;

PrecompiledRegistrar* PrecompiledRegistrar::s_this = nullptr;

PrecompiledExecutor const& PrecompiledRegistrar::executor(std::string const& _name)
{
	if (!get()->m_execs.count(_name))
		BOOST_THROW_EXCEPTION(ExecutorNotFound());
	return get()->m_execs[_name];
}

namespace
{

ETH_REGISTER_PRECOMPILED(ecrecover)(bytesConstRef _in, bytesRef _out)
{
	struct inType
	{
		h256 hash;
		h256 v;
		h256 r;
		h256 s;
	} in;

	memcpy(&in, _in.data(), min(_in.size(), sizeof(in)));

	h256 ret;
	u256 v = (u256)in.v;
	if (v >= 27 && v <= 28)
	{
		SignatureStruct sig(in.r, in.s, (byte)((int)v - 27));
		if (sig.isValid())
		{
			try
			{
				if (Public rec = recover(sig, in.hash))
				{
					ret = dev::sha3(rec);
					memset(ret.data(), 0, 12);
					ret.ref().copyTo(_out);
				}
			}
			catch (...) {}
		}
	}
}

ETH_REGISTER_PRECOMPILED(sha256)(bytesConstRef _in, bytesRef _out)
{
	dev::sha256(_in).ref().copyTo(_out);
}

ETH_REGISTER_PRECOMPILED(ripemd160)(bytesConstRef _in, bytesRef _out)
{
	h256(dev::ripemd160(_in), h256::AlignRight).ref().copyTo(_out);
}

ETH_REGISTER_PRECOMPILED(identity)(bytesConstRef _in, bytesRef _out)
{
	_in.copyTo(_out);
}

}
