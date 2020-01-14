
#pragma once

#include <libcv/Client.h>

namespace dev
{
namespace eth
{

class PBFT;

DEV_SIMPLE_EXCEPTION(NotPBFTSealEngine);
DEV_SIMPLE_EXCEPTION(ChainParamsNotPBFT);
DEV_SIMPLE_EXCEPTION(InitFailed);

class PBFTClient: public Client
{
public:
	/// Trivial forwarding constructor.
	PBFTClient(
	    ChainParams const& _params,
	    int _networkID,
	    p2p::Host* _host,
	    std::shared_ptr<GasPricer> _gpForAdoption,
	    std::string const& _dbPath = std::string(),
	    WithExisting _forceAction = WithExisting::Trust,
	    TransactionQueue::Limits const& _l = TransactionQueue::Limits {102400, 102400}
	);

	virtual ~PBFTClient();

	void startSealing() override;
	void stopSealing() override;
	//bool isMining() const override { return isWorking(); }

	PBFT* pbft() const;

protected:
	void init(ChainParams const& _params, p2p::Host *_host);
	void doWork(bool _doWait) override;
	void rejigSealing() override;
	void syncBlockQueue() override;
	void syncTransactionQueue(u256 const& _max_block_txs);
	void executeTransaction();
	void onTransactionQueueReady() override;

	bool submitSealed(bytes const & _block, bool _isOurs);

private:
	bool  m_empty_block_flag;
	float m_exec_time_per_tx;
	uint64_t m_last_exec_finish_time;
	uint64_t m_left_time;

	ChainParams m_params;
};

PBFTClient& asPBFTClient(Interface& _c);
PBFTClient* asPBFTClient(Interface* _c);

}
}
