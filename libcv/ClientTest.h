

#pragma once

#include <tuple>
#include <libcv/Client.h>

namespace dev
{
namespace eth
{

DEV_SIMPLE_EXCEPTION(ChainParamsInvalid);
DEV_SIMPLE_EXCEPTION(ChainParamsNotNoProof);

class ClientTest: public Client
{
public:
	/// Trivial forwarding constructor.
	ClientTest(
		ChainParams const& _params,
		int _networkID,
		p2p::Host* _host,
		std::shared_ptr<GasPricer> _gpForAdoption,
		std::string const& _dbPath = std::string(),
		WithExisting _forceAction = WithExisting::Trust,
		TransactionQueue::Limits const& _l = TransactionQueue::Limits{1024, 1024}
	);

	void setChainParams(std::string const& _genesis);
	void mineBlocks(unsigned _count);
	void modifyTimestamp(u256 const& _timestamp);
	void rewindToBlock(unsigned _number);
	bool addBlock(std::string const& _rlp);

protected:
	unsigned m_blocksToMine;
	virtual void onNewBlocks(h256s const& _blocks, h256Hash& io_changed) override;
};

ClientTest& asClientTest(Interface& _c);
ClientTest* asClientTest(Interface* _c);

}
}
