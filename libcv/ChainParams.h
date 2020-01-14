

#pragma once

#include <libdevcore/Common.h>
#include <libcvcore/Common.h>
#include <libcvcore/ChainOperationParams.h>
#include <libcvcore/BlockHeader.h>
#include "Account.h"

namespace dev
{
namespace eth
{

class SealEngineFace;

struct ChainParams: public ChainOperationParams
{
	ChainParams();
	ChainParams(ChainParams const& /*_org*/) = default;
	ChainParams(std::string const& _s, h256 const& _stateRoot = h256());
	ChainParams(bytes const& _genesisRLP, AccountMap const& _state) { populateFromGenesis(_genesisRLP, _state); }
	ChainParams(std::string const& _json, bytes const& _genesisRLP, AccountMap const& _state): ChainParams(_json) { populateFromGenesis(_genesisRLP, _state); }

	SealEngineFace* createSealEngine();

	/// Genesis params.
	h256 parentHash = h256();
	Address author = Address();
	u256 difficulty = 1;
	u256 gasLimit = 1 << 31;
	u256 gasUsed = 0;
	u256 timestamp = 0;
	bytes extraData;
	mutable h256 stateRoot;	///< Only pre-populate if known equivalent to genesisState's root. If they're different Bad Things Will Happen.
	AccountMap genesisState;

	unsigned sealFields = 0;
	bytes sealRLP;

	h256 calculateStateRoot(bool _force = false) const;

	std::vector<std::string> _vInitIdentityNodes;
	/// Genesis block info.
	bytes genesisBlock() const;
	/// load config/genesis
	ChainParams loadConfig(std::string const& _json, h256 const& _stateRoot = h256()) const;
	ChainParams loadGenesisState(std::string const& _json,  h256 const& _stateRoot = h256()) const;
	ChainParams loadGenesis(std::string const& _json, h256 const& _stateRoot = h256()) const;
	ChainParams loadGodMiner(std::string const& _json) const;

private:
	void populateFromGenesis(bytes const& _genesisRLP, AccountMap const& _state);
};

}
}
