
#pragma once
#include "DBFace.h"

namespace dev
{
namespace rpc
{

class MemoryDB: public dev::rpc::DBFace
{
public:
	virtual RPCModules implementedModules() const override
	{
		return RPCModules{RPCModule{"db", "1.0"}};
	}
	virtual bool db_put(std::string const& _name, std::string const& _key, std::string const& _value) override;
	virtual std::string db_get(std::string const& _name, std::string const& _key) override;
	
private:
	std::map<std::string, std::string> m_db;
};

}
}
