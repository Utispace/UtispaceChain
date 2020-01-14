
#pragma once

#if !defined(_WIN32)

#include <string>
#include <thread>
#include <sys/un.h>
#include "IpcServerBase.h"

namespace dev
{
class UnixDomainSocketServer: public IpcServerBase<int>
{
public:
	UnixDomainSocketServer(std::string const& _appId);
	~UnixDomainSocketServer();
	bool StartListening() override;
	bool StopListening() override;

protected:
	void Listen() override;
	void CloseConnection(int _socket) override;
	size_t Write(int _connection, std::string const& _data) override;
	size_t Read(int _connection, void* _data, size_t _size) override;

	sockaddr_un m_address;
	int m_socket = 0;
};

} // namespace dev

#endif
