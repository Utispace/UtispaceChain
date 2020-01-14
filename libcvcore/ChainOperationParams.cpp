

#include "ChainOperationParams.h"
#include <libdevcore/easylog.h>
#include <libdevcore/CommonData.h>
using namespace std;
using namespace dev;
using namespace eth;

PrecompiledContract::PrecompiledContract(unsigned _base, unsigned _word, std::function<void(bytesConstRef, bytesRef)> const& _exec):
	PrecompiledContract([=](unsigned size) -> bigint
	{
		bigint s = size;
		bigint b = _base;
		bigint w = _word;
		return b + (s + 31) / 32 * w;
	}, _exec)
{}

ChainOperationParams::ChainOperationParams()
{
	otherParams = std::unordered_map<std::string, std::string>{
		{"minGasLimit", "0x1388"},
		{"maxGasLimit", "0x7fffffffffffffff"},
		{"gasLimitBoundDivisor", "0x0400"},
		{"minimumDifficulty", "0x020000"},
		{"difficultyBoundDivisor", "0x0800"},
		{"durationLimit", "0x0d"},
		{"registrar", "5e70c0bbcd5636e0f9f9316e9f8633feb64d4050"},
		{"networkID", "0x0"}
	};
	blockReward = u256("0x4563918244F40000");
}

u256 ChainOperationParams::u256Param(string const& _name) const
{
	std::string at("");

	auto it = otherParams.find(_name);
	if (it != otherParams.end())
		at = it->second;

	return u256(fromBigEndian<u256>(fromHex(at)));
}

string ChainOperationParams::param(string const& _name) const
{
	std::string at("");

	auto it = otherParams.find(_name);
	if (it != otherParams.end())
		at = it->second;

	return at;
}

