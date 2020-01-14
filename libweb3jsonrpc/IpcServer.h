
#pragma once

#include "UnixSocketServer.h"
#include "WinPipeServer.h"

namespace dev
{
#if defined(_WIN32)
	using IpcServer = WindowsPipeServer;
#else
	using IpcServer = UnixDomainSocketServer;
#endif
} // namespace dev
