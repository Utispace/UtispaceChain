
#pragma once

#include <json/json.h>
#include <libcvcore/Common.h>
#include <libp2p/Common.h>
#include <libcvcore/BlockHeader.h>
#include <libcv/LogFilter.h>

namespace dev
{

Json::Value toJson(std::map<h256, std::pair<u256, u256>> const& _storage);
Json::Value toJson(std::unordered_map<u256, u256> const& _storage);
Json::Value toJson(Address const& _address);

namespace p2p
{

Json::Value toJson(PeerSessionInfo const& _p);

}

namespace eth
{

class Transaction;
class LocalisedTransaction;
class SealEngineFace;
class NodeConnParams;
struct BlockDetails;
class Interface;
using Transactions = std::vector<Transaction>;
using UncleHashes = h256s;
using TransactionHashes = h256s;

Json::Value toJson(BlockHeader const& _bi, SealEngineFace* _face = nullptr);
//TODO: wrap these params into one structure eg. "LocalisedTransaction"
Json::Value toJson(Transaction const& _t, std::pair<h256, unsigned> _location, BlockNumber _blockNumber);
Json::Value toJson(BlockHeader const& _bi, BlockDetails const& _bd, UncleHashes const& _us, Transactions const& _ts, SealEngineFace* _face = nullptr);
Json::Value toJson(BlockHeader const& _bi, BlockDetails const& _bd, UncleHashes const& _us, TransactionHashes const& _ts, SealEngineFace* _face = nullptr);
Json::Value toJson(TransactionSkeleton const& _t);
Json::Value toJson(Transaction const& _t);
Json::Value toJson(LocalisedTransaction const& _t);
Json::Value toJson(NodeConnParams const& _t);
Json::Value toJson(TransactionReceipt const& _t);
Json::Value toJson(LocalisedTransactionReceipt const& _t);
Json::Value toJson(LocalisedLogEntry const& _e);
Json::Value toJson(LogEntry const& _e);
Json::Value toJson(std::unordered_map<h256, LocalisedLogEntries> const& _entriesByBlock);
Json::Value toJsonByBlock(LocalisedLogEntries const& _entries);
TransactionSkeleton toTransactionSkeleton(Json::Value const& _json);
LogFilter toLogFilter(Json::Value const& _json);
LogFilter toLogFilter(Json::Value const& _json, Interface const& _client);	// commented to avoid warning. Uncomment once in use @ PoC-7.

//以name方式调用的参数信息
struct CnsParams
{
	CnsParams():jParams(Json::nullValue)
	{
	}

	std::string strContractName;
	std::string strFunc;
	std::string strVersion;
	Json::Value jParams;

	Json::Value toJsonObject() const
	{
		Json::Value jValue(Json::objectValue);
		jValue["contractName"] = strContractName;
		jValue["version"] = strVersion;
		jValue["method"] = strFunc;
		jValue["params"] = jParams;
		return jValue;
	}
};

void fromJsonGetParams(Json::Value const& _json, CnsParams &params);
bool fromJsonGetParams(std::string const& _json, CnsParams &params);

class AddressResolver
{
public:
	static Address fromJS(std::string const& _address);
};

}

/*namespace shh
{

Json::Value toJson(h256 const& _h, Envelope const& _e, Message const& _m);
Message toMessage(Json::Value const& _json);
Envelope toSealed(Json::Value const& _json, Message const& _m, Secret const& _from);
std::pair<Topics, Public> toWatch(Json::Value const& _json);

}*/

namespace rpc
{
h256 h256fromHex(std::string const& _s);
}

template <class T>
Json::Value toJson(std::vector<T> const& _es)
{
	Json::Value res(Json::arrayValue);
	for (auto const& e: _es)
		res.append(toJson(e));
	return res;
}

template <class T>
Json::Value toJson(std::unordered_set<T> const& _es)
{
	Json::Value res(Json::arrayValue);
	for (auto const& e: _es)
		res.append(toJson(e));
	return res;
}

template <class T>
Json::Value toJson(std::set<T> const& _es)
{
	Json::Value res(Json::arrayValue);
	for (auto const& e: _es)
		res.append(toJson(e));
	return res;
}

}
