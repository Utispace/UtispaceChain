
#pragma once

#include <string>
#include "Common.h"
#include "FixedHash.h"

namespace dev
{

std::string toBase64(bytesConstRef _in);
bytes fromBase64(std::string const& _in);

template <size_t N> inline std::string toBase36(FixedHash<N> const& _h)
{
	static char const* c_alphabet = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	typename FixedHash<N>::Arith a = _h;
	std::string ret;
	for (; a > 0; a /= 36)
	{
		unsigned r = (unsigned)(a - a / 36 * 36); // boost's % is broken
		ret = c_alphabet[r] + ret;
	}
	return ret;
}

template <size_t N> inline FixedHash<N> fromBase36(std::string const& _h)
{
	typename FixedHash<N>::Arith ret = 0;
	for (char c: _h)
		ret = ret * 36 + (c < 'A' ? c - '0' : (c - 'A' + 10));
	return ret;
}

}
