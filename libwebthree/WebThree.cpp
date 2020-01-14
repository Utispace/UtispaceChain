
#include "WebThree.h"
#include <chrono>
#include <thread>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <libdevcore/easylog.h>
#include <libcv/Defaults.h>
#include <libcv/EthereumHost.h>
#include <libcv/ClientTest.h>
#include <libcv/NodeConnParamsManagerApi.h>
#include "BuildInfo.h"
#include <libpbftseal/PBFT.h>
#include <libpbftseal/PBFTClient.h>
#include "Swarm.h"
#include "Support.h"
using namespace std;
using namespace dev;
using namespace dev::p2p;
using namespace dev::eth;
using namespace dev::shh;

WebThreeDirect::WebThreeDirect(
    std::string const& _clientVersion,
    std::string const& _dbPath,
    eth::ChainParams const& _params,
    WithExisting _we,
    std::set<std::string> const& _interfaces,
    NetworkPreferences const& _n,
    bytesConstRef _network,
    bool _testing
):
	m_clientVersion(_clientVersion),
	m_net(_clientVersion, _n, _network, _params.statsInterval)
{
	LOG(INFO) << "My enode:" << enode();
	//设置NodeConnParamsManager中的NetworkFace指针
	NodeConnManagerSingleton::GetInstance().setNetworkFace(this);
	if (_dbPath.size())
		Defaults::setDBPath(_dbPath);
	if (_interfaces.count("eth"))
	{
		PBFT::init();

        if (_params.sealEngineName == "PBFT") {
			m_ethereum.reset(new eth::PBFTClient(_params, (int)_params.u256Param("networkID"), &m_net, shared_ptr<GasPricer>(), _dbPath, _we));
		}
		else
		{
			LOG(DEBUG) << "WebThreeDirect::WebThreeDirect Default ";
			m_ethereum.reset(new eth::Client(_params, (int)_params.u256Param("networkID"), &m_net, shared_ptr<GasPricer>(), _dbPath, _we));
		}
		string bp = DEV_QUOTED(ETH_BUILD_PLATFORM);
		vector<string> bps;
		boost::split(bps, bp, boost::is_any_of("/"));
		bps[0] = bps[0].substr(0, 5);
		bps[1] = bps[1].substr(0, 3);
		bps.back() = bps.back().substr(0, 3);
		m_ethereum->setExtraData(rlpList(0, string(dev::Version) + "++" + string(DEV_QUOTED(ETH_COMMIT_HASH)).substr(0, 4) + (ETH_CLEAN_REPO ? "-" : "*") + string(DEV_QUOTED(ETH_BUILD_TYPE)).substr(0, 1) + boost::join(bps, "/")));
	}

	if (_interfaces.count("bzz"))
	{
		m_swarm.reset(new bzz::Client(this));
	}

	m_support = make_shared<Support>(this);
}

WebThreeDirect::~WebThreeDirect()
{
	m_net.stop();
	m_ethereum.reset();
}

bzz::Interface* WebThreeDirect::swarm() const
{
	if (!m_swarm)
		BOOST_THROW_EXCEPTION(InterfaceNotSupported("bzz"));
	return m_swarm.get();
}

std::string WebThreeDirect::composeClientVersion(std::string const& _client)
{
	return _client + "/" + \
	       "v" + dev::Version + "/" + \
	       DEV_QUOTED(ETH_BUILD_OS) + "/" + \
	       DEV_QUOTED(ETH_BUILD_COMPILER) + "/" + \
	       DEV_QUOTED(ETH_BUILD_JIT_MODE) + "/" + \
	       DEV_QUOTED(ETH_BUILD_TYPE) + "/" + \
	       string(DEV_QUOTED(ETH_COMMIT_HASH)).substr(0, 8) + \
	       (ETH_CLEAN_REPO ? "" : "*") + "/";
}

p2p::NetworkPreferences const& WebThreeDirect::networkPreferences() const
{
	return m_net.networkPreferences();
}

void WebThreeDirect::setNetworkPreferences(p2p::NetworkPreferences const& _n, bool _dropPeers)
{
	auto had = isNetworkStarted();
	if (had)
		stopNetwork();
	m_net.setNetworkPreferences(_n, _dropPeers);
	if (had)
		startNetwork();
}

ba::io_service* WebThreeDirect::ioService() {
	return m_net.getIOService();
}

std::vector<PeerSessionInfo> WebThreeDirect::peers()
{
	return m_net.peerSessionInfo();
}

size_t WebThreeDirect::peerCount() const
{
	return m_net.peerCount();
}

void WebThreeDirect::setIdealPeerCount(size_t _n)
{
	return m_net.setIdealPeerCount(_n);
}

void WebThreeDirect::setPeerStretch(size_t _n)
{
	return m_net.setPeerStretch(_n);
}

bytes WebThreeDirect::saveNetwork()
{
	return m_net.saveNetwork();
}

void WebThreeDirect::addNode(NodeID const& _node, bi::tcp::endpoint const& _host)
{
	m_net.addNode(_node, NodeIPEndpoint(_host.address(), _host.port(), _host.port()));
}

void WebThreeDirect::requirePeer(NodeID const& _node, bi::tcp::endpoint const& _host)
{
	m_net.requirePeer(_node, NodeIPEndpoint(_host.address(), _host.port(), _host.port()));
}

void WebThreeDirect::addPeer(NodeSpec const& _s, PeerType _t)
{
	LOG(DEBUG) << "WebThreeDirect::addPeer ";
	m_net.addPeer(_s, _t);
}

