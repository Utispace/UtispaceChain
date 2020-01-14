
#pragma once
#include "BzzFace.h"

namespace dev
{

namespace bzz
{
class Interface;
}

namespace rpc
{

class Bzz: public BzzFace
{
public:
	Bzz(dev::bzz::Interface& _bzz);
	virtual RPCModules implementedModules() const override
	{
		return RPCModules{RPCModule{"bzz", "1.0"}};
	}
	virtual std::string bzz_put(std::string const& _data) override;
	virtual std::string bzz_get(std::string const& _hash) override;

private:
	dev::bzz::Interface& m_bzz;
};

}
}
