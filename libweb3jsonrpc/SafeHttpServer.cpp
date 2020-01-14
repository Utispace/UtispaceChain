
#include <microhttpd.h>
#include <sstream>
#include "SafeHttpServer.h"

using namespace std;
using namespace dev;
/// structure copied from libjson-rpc-cpp httpserver version 0.6.0
struct mhd_coninfo
{
	struct MHD_PostProcessor *postprocessor;
	MHD_Connection* connection;
	stringstream request;
	jsonrpc::HttpServer* server;
	int code;
};

bool SafeHttpServer::SendResponse(string const& _response, void* _addInfo)
{
	struct mhd_coninfo* client_connection = static_cast<struct mhd_coninfo*>(_addInfo);
	struct MHD_Response *result = MHD_create_response_from_buffer(
	                                  _response.size(),
	                                  static_cast<void *>(const_cast<char *>(_response.c_str())),
	                                  MHD_RESPMEM_MUST_COPY
	                              );

	MHD_add_response_header(result, "Content-Type", "application/json");
	MHD_add_response_header(result, "Access-Control-Allow-Origin", m_allowedOrigin.c_str());

	int ret = MHD_queue_response(client_connection->connection, client_connection->code, result);
	MHD_destroy_response(result);
	return ret == MHD_YES;
}

bool SafeHttpServer::SendOptionsResponse(void* _addInfo)
{
	struct mhd_coninfo* client_connection = static_cast<struct mhd_coninfo*>(_addInfo);
	struct MHD_Response *result = MHD_create_response_from_buffer(0, nullptr, MHD_RESPMEM_MUST_COPY);

	MHD_add_response_header(result, "Allow", "POST, OPTIONS");
	MHD_add_response_header(result, "Access-Control-Allow-Origin", m_allowedOrigin.c_str());
	MHD_add_response_header(result, "Access-Control-Allow-Headers", "origin, content-type, accept");
	MHD_add_response_header(result, "DAV", "1");

	int ret = MHD_queue_response(client_connection->connection, client_connection->code, result);
	MHD_destroy_response(result);
	return ret == MHD_YES;
}

int SafeHttpServer::callback(void *cls, struct MHD_Connection *connection, const char *url, const char *method, const char *version, const char *upload_data, size_t *upload_data_size, void **con_cls) {
	(void)version;
	std::string account = "";
	string strUrl(url);
	string strMethod(method);

	return HttpServer::callback(cls, connection, url, method, version, upload_data, upload_data_size, con_cls);
}

bool SafeHttpServer::StartListening() {
	return HttpServer::StartListening();
}
bool SafeHttpServer::StopListening() {
	return HttpServer::StopListening();
}