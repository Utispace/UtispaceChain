

#pragma once

#include <boost/asio.hpp>
#include <string>
#include <thread>
#include <queue>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <libdevcore/FixedHash.h>

#include "ChannelException.h"
#include "ChannelSession.h"

namespace dev
{

namespace channel {

class ChannelServer: public std::enable_shared_from_this<ChannelServer> {
public:
	void run();

	void onAccept(const boost::system::error_code& error, ChannelSession::Ptr session);

	void startAccept();

	void setBind(const std::string &host, int port) { _listenHost = host; _listenPort = port; };

	void setEnableSSL(bool enableSSL) { _enableSSL = enableSSL; };

	void setConnectionHandler(std::function<void(dev::channel::ChannelException, ChannelSession::Ptr)> handler) { _connectionHandler = handler; };

	void setIOService(std::shared_ptr<boost::asio::io_service> ioService) { _ioService = ioService; };
	void setSSLContext(std::shared_ptr<boost::asio::ssl::context> sslContext) { _sslContext = sslContext; };

	void stop();

private:
	void onHandshake(const boost::system::error_code& error, ChannelSession::Ptr session);

	std::shared_ptr<boost::asio::io_service> _ioService;
	std::shared_ptr<boost::asio::ssl::context> _sslContext;

	std::vector<std::shared_ptr<std::thread> > _serverThreads;

	std::shared_ptr<boost::asio::ip::tcp::acceptor> _acceptor;

	std::function<void(dev::channel::ChannelException, ChannelSession::Ptr)> _connectionHandler;

	std::string _listenHost = "";
	int _listenPort = 0;
	bool _enableSSL = false;
};

}

}
