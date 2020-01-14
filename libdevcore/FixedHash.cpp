
#include "FixedHash.h"
#include <ctime>
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace dev;

boost::random_device dev::s_fixedHashEngine;

h128 dev::fromUUID(std::string const& _uuid)
{
	try
	{
		return h128(boost::replace_all_copy(_uuid, "-", ""));
	}
	catch (...)
	{
		return h128();
	}
}

std::string dev::toUUID(h128 const& _uuid)
{
	std::string ret = toHex(_uuid.ref());
	for (unsigned i: {20, 16, 12, 8})
		ret.insert(ret.begin() + i, '-');
	return ret;
}

