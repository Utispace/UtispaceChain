
#pragma once
#include "TestFace.h"

namespace dev
{

namespace eth
{
class Client;
}

namespace rpc
{

class Test: public TestFace
{
public:
	Test(eth::Client& _eth);
	virtual RPCModules implementedModules() const override
	{
		return RPCModules{RPCModule{"test", "1.0"}};
	}

	virtual bool test_setChainParams(const Json::Value &param1) override;
	virtual bool test_mineBlocks(int _number) override;
	virtual bool test_modifyTimestamp(int _timestamp) override;
	virtual bool test_addBlock(std::string const& _rlp) override;
	virtual bool test_rewindToBlock(int _number) override;

private:
	eth::Client& m_eth;
};

}
}
