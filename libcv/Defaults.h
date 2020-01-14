

#pragma once

#include <libdevcore/Common.h>

namespace dev
{
namespace eth
{

struct Defaults
{
	friend class BlockChain;
	friend class State;

public:
	Defaults();

	static Defaults* get() { if (!s_this) s_this = new Defaults; return s_this; }
	static void setDBPath(std::string const& _dbPath) { get()->m_dbPath = _dbPath; }
	static std::string const& dbPath() { return get()->m_dbPath; }

private:
	std::string m_dbPath;

	static Defaults* s_this;
};

}
}
