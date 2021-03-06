

#pragma once

#include <mutex>
#include <unordered_map>
#include <vector>
#include <unordered_set>
#include <memory>
#include <utility>
#include <thread>

#include <libdevcore/Guards.h>
#include <libdevcore/Worker.h>
#include <libcvcore/Common.h>
#include <libp2p/Common.h>
#include <libdevcore/OverlayDB.h>
#include <libcvcore/BlockHeader.h>
#include "BlockChainSync.h"
#include "CommonNet.h"
#include "EthereumPeer.h"
#include "Web3Observer.h"

namespace dev
{

class RLPStream;

namespace eth
{

class TransactionQueue;
class BlockQueue;
class BlockChainSync;
class NodeConnParams;

struct CustomMessageRequest {
	std::string fromNodeID;
	std::string toNodeID;
	int sendTimestamp;
	int TTL;
};

struct CustomMessageResponse;



/**
 * @brief The EthereumHost class
 * @warning None of this is thread-safe. You have been warned.
 * @doWork Syncs to peers and sends new blocks and transactions.
 */
class EthereumHost: public p2p::HostCapability<EthereumPeer>, Worker
{
public:
	/// Start server, but don't listen.
	EthereumHost(BlockChain const& _ch, OverlayDB const& _db, TransactionQueue& _tq, BlockQueue& _bq, u256 _networkId);

	/// Will block on network process events.
	virtual ~EthereumHost();

	unsigned protocolVersion() const { return c_protocolVersion; }
	u256 networkId() const { return m_networkId; }
	void setNetworkId(u256 _n) { m_networkId = _n; }

	void reset();

	bool isSyncing() const;
	bool isBanned(p2p::NodeID const& _id) const { return !!m_banned.count(_id); }

	void noteNewTransactions() { m_newTransactions = true; }
	void noteNewBlocks() { m_newBlocks = true; }
	void onForceSync() { m_sync->onForceSync(); }
	void onBlockImported(BlockHeader const& _info) { m_sync->onBlockImported(_info); }

	BlockChain const& chain() const { return m_chain; }
	OverlayDB const& db() const { return m_db; }
	BlockQueue& bq() { return m_bq; }
	BlockQueue const& bq() const { return m_bq; }
	SyncStatus status() const;
	h256 latestBlockSent() { return m_latestBlockSent; }
	static char const* stateName(SyncState _s) { return s_stateNames[static_cast<int>(_s)]; }

	static unsigned const c_oldProtocolVersion;
	void foreachPeer(std::function<bool(std::shared_ptr<EthereumPeer>)> const& _f) const;

	//广播自己的topic
	//void broadcastTopics();

	/**
	* 给所有已连接节点发送新增的节点配置
	* in : vParams - 需要新加的节点配置信息
	*/
	void addNodeConnParam(std::vector<NodeConnParams> const &vParams);


	/**
	* 给所有已连接节点发送删除节点配置
	* in : sParams - 删除节点的NodeId
	*/
	void delNodeConnParam(std::string const &sParams);

	void setWeb3Observer(Web3Observer::Ptr _observer);

	/*
	 * 链上链下通讯
	 */
	void sendCustomMessage(p2p::NodeID nodeID, std::shared_ptr<bytes> message);

	//根据topic，获取节点nodeID列表
	std::vector<p2p::NodeID> getPeersByTopic(std::string topic);
	//链上链下2期消息
	//h512 sendChannelMessage(std::string topic, std::shared_ptr<bytes> message);
	//链上链下2期回包消息
	//bool sendChannelReplyMessage(p2p::NodeID nodeID, std::shared_ptr<bytes> message);

	int getTopicsSeq() { return _topicsSeq; };

	//设置自身的topics
	void setTopics(std::shared_ptr<std::set<std::string> > topics);

	std::shared_ptr<std::set<std::string> > getTopics() { return _topics; };

	//链上链下2期topics消息
	void sendTopicsMessage(p2p::NodeID nodeID, int type, int seq, std::shared_ptr<std::set<std::string> > topics);

	//获取NodeId与块高的映射
	void getPeersHeight(std::map<h512, u256>& mp);
protected:
	std::shared_ptr<p2p::Capability> newPeerCapability(std::shared_ptr<p2p::SessionFace> const& _s, unsigned _idOffset, p2p::CapDesc const& _cap, uint16_t _capID) override;

private:
	static char const* const s_stateNames[static_cast<int>(SyncState::Size)];

	std::tuple<std::vector<std::shared_ptr<EthereumPeer>>, std::vector<std::shared_ptr<EthereumPeer>>, std::vector<std::shared_ptr<p2p::SessionFace>>> randomSelection(unsigned _percent = 25, std::function<bool(EthereumPeer*)> const& _allow = [](EthereumPeer const*) { return true; });

	/// Sync with the BlockChain. It might contain one of our mined blocks, we might have new candidates from the network.
	virtual void doWork() override;

	void maintainTransactions();
	void maintainBlocks(h256 const& _currentBlock);
	void onTransactionImported(ImportResult _ir, h256 const& _h, h512 const& _nodeId);

	///	Check to see if the network peer-state initialisation has happened.
	bool isInitialised() const { return (bool)m_latestBlockSent; }

	/// Initialises the network peer-state, doing the stuff that needs to be once-only. @returns true if it really was first.
	bool ensureInitialised();

	virtual void onStarting() override { startWorking(); }
	virtual void onStopping() override { stopWorking(); }

	BlockChain const& m_chain;
	OverlayDB const& m_db;					///< References to DB, needed for some of the Ethereum Protocol responses.
	TransactionQueue& m_tq;					///< Maintains a list of incoming transactions not yet in a block on the blockchain.
	BlockQueue& m_bq;						///< Maintains a list of incoming blocks not yet on the blockchain (to be imported).

	u256 m_networkId;

	h256 m_latestBlockSent;
	h256Hash m_transactionsSent;

	std::unordered_set<p2p::NodeID> m_banned;

	bool m_newTransactions = false;
	bool m_newBlocks = false;

	mutable RecursiveMutex x_sync;
	mutable Mutex x_transactions;
	std::unique_ptr<BlockChainSync> m_sync;
	std::atomic<time_t> m_lastTick = { 0 };

	std::shared_ptr<EthereumHostDataFace> m_hostData;
	std::shared_ptr<EthereumPeerObserverFace> m_peerObserver;
	std::shared_ptr<ChannelMessageObserverFace> _channelObserver;

	int _topicsSeq = 0;
	std::shared_ptr<std::set<std::string> > _topics;
};

}
}
