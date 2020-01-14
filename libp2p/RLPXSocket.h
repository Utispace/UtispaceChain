

#pragma once
#include "Common.h"
#include <libdevcore/easylog.h>
#include <libdevcore/FileSystem.h>
#include <boost/filesystem.hpp>
#define SSL_SOCKET 1
#define BASE_SOCKET 0

namespace dev
{
namespace p2p
{


class RLPXSocket: public std::enable_shared_from_this<RLPXSocket>
{
public:
	RLPXSocket(ba::io_service& _ioService)
	{
		if (dev::getSSL() == SSL_SOCKET)
		{
			LOG(DEBUG)<<"RLPXSocket::m_socketType:1";
			m_socketType = SSL_SOCKET;
			ba::ssl::context sslContext(ba::ssl::context::sslv23);
			std::string certData = dev::asString(contents(getDataDir() + "/ca.crt"));
			if (certData != "")
			{
				sslContext.add_certificate_authority(boost::asio::const_buffer(certData.c_str(), certData.size()));
			}
			else
			{
				std::cout<<"Get CaPublicKey File Err......................"<<"\n";
			}
			certData = dev::asString(contents(getDataDir() + "/server.crt"));
			if (certData != "")
			{
				sslContext.use_certificate_chain(boost::asio::const_buffer(certData.c_str(), certData.size()));
			}
			else
			{
				std::cout<<"Get PublicKey File Err......................"<<"\n";
			}
			certData = dev::getPrivateKey();
			if (certData != "")
			{
				sslContext.use_private_key(boost::asio::const_buffer(certData.c_str(), certData.size()),ba::ssl::context_base::pem);
			}
			else
			{
				std::cout<<"Get PrivateKey File Err......................"<<"\n";
			}
			m_sslsocket = std::make_shared<ba::ssl::stream<bi::tcp::socket> >(_ioService,sslContext);
		}
		else
		{
			LOG(DEBUG)<<"RLPXSocket::m_socketType:0";
			m_socketType = BASE_SOCKET;
			m_socket = std::make_shared<bi::tcp::socket>(_ioService);
		}
	}
	~RLPXSocket() 
	{ 
		close(); 
	}
	
	bool isConnected() const 
	{ 
		if (m_socketType == SSL_SOCKET)
		{
			return m_sslsocket->lowest_layer().is_open();
		}
		return m_socket->is_open(); 
	}
	void close() 
	{ 
		try 
		{ 
			boost::system::error_code ec;
			if (m_socketType == SSL_SOCKET)
			{
				m_sslsocket->lowest_layer().shutdown(bi::tcp::socket::shutdown_both, ec);
				if (m_sslsocket->lowest_layer().is_open())
					m_sslsocket->lowest_layer().close();
			}
			else
			{
				m_socket->shutdown(bi::tcp::socket::shutdown_both, ec);
				if (m_socket->is_open()) 
					m_socket->close();
			}	 
		} 
		catch (...){} 
	}
	bi::tcp::endpoint remoteEndpoint() 
	{ 
		boost::system::error_code ec; 
		if (m_socketType == SSL_SOCKET)
		{
			return m_sslsocket->lowest_layer().remote_endpoint(ec);
		}
		return m_socket->remote_endpoint(ec);
	}

	int getSocketType()
	{
		return m_socketType;
	}

	bi::tcp::socket& ref() 
	{ 
		if (m_socketType == SSL_SOCKET)
		{
			return m_sslsocket->next_layer();
		}
		return *m_socket;
	}

	ba::ssl::stream<bi::tcp::socket>& sslref()
	{
		return *m_sslsocket;
	}
protected:
	int m_socketType;
	std::shared_ptr<bi::tcp::socket> m_socket;
	std::shared_ptr<ba::ssl::stream<bi::tcp::socket> > m_sslsocket;
};

}
}
