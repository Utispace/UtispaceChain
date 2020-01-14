
#pragma once

#if defined(_WIN32)

#include <string>
#include <thread>
#include <mutex>
#include <unordered_set>
#include <Windows.h>
#include "IpcServerBase.h"

namespace dev
{
class WindowsPipeServer: public IpcServerBase<HANDLE>
{
public:
	WindowsPipeServer(std::string const& _appId);

protected:
	void Listen() override;
	void CloseConnection(HANDLE _socket) override;
	size_t Write(HANDLE _connection, std::string const& _data) override;
	size_t Read(HANDLE _connection, void* _data, size_t _size) override;
};

} // namespace dev

#endif