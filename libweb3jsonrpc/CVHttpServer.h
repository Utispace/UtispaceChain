
#include <stdarg.h>
#include <stdint.h>
#include <sys/types.h>
#if defined(_WIN32) && !defined(__CYGWIN__)
#include <ws2tcpip.h>
#if defined(_MSC_FULL_VER) && !defined(_SSIZE_T_DEFINED)
#define _SSIZE_T_DEFINED
typedef intptr_t ssize_t;
#endif // !_SSIZE_T_DEFINED */
#else
#include <unistd.h>
#include <sys/time.h>
#include <sys/socket.h>
#endif
#include <sys/select.h>
#include <map>
#include <microhttpd.h>
#include "jsonrpccpp/server/abstractserverconnector.h"
#ifndef KEY_REQUEST_METHODNAME
#define KEY_REQUEST_METHODNAME "method"
#endif

typedef int(*pCallBack)(void *, struct MHD_Connection *, const char *, const char *, const char *, const char *, size_t *, void **);
typedef void(*pCompletedCallbak)(void *, struct MHD_Connection *, void **, enum MHD_RequestTerminationCode);

namespace jsonrpc
{

	class HttpServer : public AbstractServerConnector
	{
	public:
		HttpServer(int port, const std::string &sslcert = "", const std::string &sslkey = "", int threads = 50, const std::string &config = "");

		virtual bool StartListening();
		virtual bool StopListening();

		bool virtual SendResponse(const std::string &response,
			void *addInfo = NULL);
		bool virtual SendOptionsResponse(void *addInfo);

		void SetUrlHandler(const std::string &url, IClientConnectionHandler *handler);

		bool isRunning() const { return running; }

		virtual pCallBack getCallback() {
			return HttpServer::callback;
		}
		virtual pCompletedCallbak getCompletedCallback() {
			return HttpServer::reqeuest_completed; // do nothing by default
		}
	private:
		int port;
		int threads;
		bool running;
		std::string path_sslcert;
		std::string path_sslkey;
		std::string sslcert;
		std::string sslkey;
		struct MHD_Daemon *daemon;
		std::map<std::string, IClientConnectionHandler *> urlhandler;

	protected:
		static int callback(void *cls, struct MHD_Connection *connection, const char *url, const char *method, const char *version, const char *upload_data, size_t *upload_data_size, void **con_cls);
		static void reqeuest_completed(void *cls,
			struct MHD_Connection *connection,
			void **con_cls,
			enum MHD_RequestTerminationCode toe) {}

		IClientConnectionHandler *GetHandler(const std::string &url);
	};

}