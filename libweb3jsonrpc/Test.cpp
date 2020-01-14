
#include "Test.h"
#include <jsonrpccpp/common/errors.h>
#include <jsonrpccpp/common/exception.h>
#include <libcv/ClientTest.h>
#include <libcv/ChainParams.h>

using namespace std;
using namespace dev;
using namespace dev::rpc;
using namespace jsonrpc;

Test::Test(eth::Client& _eth): m_eth(_eth) {}

bool Test::test_setChainParams(Json::Value const& param1)
{
	try
	{
		Json::FastWriter fastWriter;
		std::string output = fastWriter.write(param1);
		asClientTest(m_eth).setChainParams(output);
	}
	catch (std::exception const&)
	{
		BOOST_THROW_EXCEPTION(JsonRpcException(Errors::ERROR_RPC_INTERNAL_ERROR));
	}

	return true;
}

bool Test::test_mineBlocks(int _number)
{
	try
	{
		asClientTest(m_eth).mineBlocks(_number);
	}
	catch (std::exception const&)
	{
		BOOST_THROW_EXCEPTION(JsonRpcException(Errors::ERROR_RPC_INTERNAL_ERROR));
	}

	return true;
}

bool Test::test_modifyTimestamp(int _timestamp)
{
	try
	{
		asClientTest(m_eth).modifyTimestamp(u256(_timestamp));
	}
	catch (std::exception const&)
	{
		BOOST_THROW_EXCEPTION(JsonRpcException(Errors::ERROR_RPC_INTERNAL_ERROR));
	}
	return true;
}

bool Test::test_addBlock(std::string const& _rlp)
{
	try
	{
		asClientTest(m_eth).addBlock(_rlp);
	}
	catch (std::exception const&)
	{
		BOOST_THROW_EXCEPTION(JsonRpcException(Errors::ERROR_RPC_INTERNAL_ERROR));
	}
	return true;
}

bool Test::test_rewindToBlock(int _number)
{
	try
	{
		m_eth.rewind(_number);
	}
	catch (std::exception const&)
	{
		BOOST_THROW_EXCEPTION(JsonRpcException(Errors::ERROR_RPC_INTERNAL_ERROR));
	}
	return true;
}
