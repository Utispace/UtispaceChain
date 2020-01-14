

#include "Common.h"
#include "Exceptions.h"
#include "easylog.h"
#include "BuildInfo.h"
using namespace std;
using namespace dev;

namespace dev
{

char const* Version = ETH_PROJECT_VERSION;
char const* Copyright="by FISCO, (c)2016-2017.";

const u256 Invalid256 = ~(u256)0;

void InvariantChecker::checkInvariants(HasInvariants const* _this, char const* _fn, char const* _file, int _line, bool _pre)
{
	if (!_this->invariants())
	{
		LOG(WARNING) << (_pre ? "Pre" : "Post") << "invariant failed in" << _fn << "at" << _file << ":" << _line;
		::boost::exception_detail::throw_exception_(FailedInvariant(), _fn, _file, _line);
	}
}



TimerHelper::~TimerHelper()
{
	auto e = std::chrono::high_resolution_clock::now() - m_t;
	if (!m_ms || e > chrono::milliseconds(m_ms))
		LOG(INFO) << m_id << chrono::duration_cast<chrono::milliseconds>(e).count() << "ms";
}

uint64_t utcTime()
{
	struct timeval tv;    
   	gettimeofday(&tv,NULL);    
   	return tv.tv_sec * 1000 + tv.tv_usec / 1000;  
}

string inUnits(bigint const& _b, strings const& _units)
{
	ostringstream ret;
	u256 b;
	if (_b < 0)
	{
		ret << "-";
		b = (u256)-_b;
	}
	else
		b = (u256)_b;

	u256 biggest = 1;
	for (unsigned i = _units.size() - 1; !!i; --i)
		biggest *= 1000;

	if (b > biggest * 1000)
	{
		ret << (b / biggest) << " " << _units.back();
		return ret.str();
	}
	ret << setprecision(3);

	u256 unit = biggest;
	for (auto it = _units.rbegin(); it != _units.rend(); ++it)
	{
		auto i = *it;
		if (i != _units.front() && b >= unit)
		{
			ret << (double(b / (unit / 1000)) / 1000.0) << " " << i;
			return ret.str();
		}
		else
			unit /= 1000;
	}
	ret << b << " " << _units.front();
	return ret.str();
}

}
