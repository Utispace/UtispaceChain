
#pragma once

#include <libdevcore/Common.h>
#include "Common.h"
#include <libcvvmcore/EVMSchedule.h>

#include <vector>

namespace dev
{
namespace eth
{


class PrecompiledContract
{
public:
	PrecompiledContract() = default;
	PrecompiledContract(std::function<bigint(unsigned)> const& _cost, std::function<void(bytesConstRef, bytesRef)> const& _exec):
		m_cost(_cost),
		m_execute(_exec)
	{}
	PrecompiledContract(unsigned _base, unsigned _word, std::function<void(bytesConstRef, bytesRef)> const& _exec);

	bigint cost(bytesConstRef _in) const { return m_cost(_in.size()); }
	void execute(bytesConstRef _in, bytesRef _out) const { m_execute(_in, _out); }

private:
	std::function<bigint(unsigned)> m_cost;
	std::function<void(bytesConstRef, bytesRef)> m_execute;
};

struct ChainOperationParams
{
	ChainOperationParams();

	explicit operator bool() const { return accountStartNonce != Invalid256; }

	/// The chain sealer name: e.g. Ethash, NoProof, BasicAuthority
	std::string sealEngineName = "NoProof";

	/// General chain params.
	u256 blockReward = 0;
	u256 maximumExtraDataSize = 1024;
	u256 accountStartNonce = 0;
	bool tieBreakingGas = true;

	/// Precompiled contracts as specified in the chain params.
	std::unordered_map<Address, PrecompiledContract> precompiled;


	std::unordered_map<std::string, std::string> otherParams;

	/// Convenience method to get an otherParam as a u256 int.
	u256 u256Param(std::string const& _name) const;
	std::string param(std::string const& _name) const;

	bool evmEventLog = false; 
	bool evmCoverLog = false; 
	bool statLog = false; 
	Address sysytemProxyAddress;
	Address god;

	std::string listenIp;
	int cryptoMod = 0;	
	int cryptoprivatekeyMod = 0;
	int ssl = 0;
	int rpcPort = 6789;
	int rpcSSLPort = 6790;
	int p2pPort = 16789;
	std::string wallet;
	std::string keystoreDir;
	std::string dataDir;
	std::string logFileConf;
	
	//dfs related parameters
	std::string nodeId;
	std::string groupId;
	std::string storagePath;
	
	std::string rateLimitConfig;
	int statsInterval;//接口统计间隔 按秒计
	int channelPort = 0;

	std::string vmKind;
	unsigned networkId;
	int logVerbosity = 4;

	u256 intervalBlockTime = 3000;

	bool broadcastToNormalNode = false;


	u256 godMinerStart = 0;
	u256 godMinerEnd = 0;
	std::map<std::string, NodeConnParams> godMinerList;
};

}
}
