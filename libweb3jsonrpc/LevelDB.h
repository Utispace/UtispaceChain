
#pragma once
#include "DBFace.h"

namespace leveldb
{
	class DB;
}

namespace dev
{
namespace rpc
{

class LevelDB: public dev::rpc::DBFace
{
public:
	LevelDB();
	virtual RPCModules implementedModules() const override
	{
		return RPCModules{RPCModule{"db", "1.0"}};
	}
	virtual bool db_put(std::string const& _name, std::string const& _key, std::string const& _value) override;
	virtual std::string db_get(std::string const& _name, std::string const& _key) override;
	
private:
	leveldb::DB* m_db;
};

}
}
