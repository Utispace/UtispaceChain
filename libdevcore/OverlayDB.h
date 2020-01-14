

#pragma once

#include <memory>
#include <libdevcore/db.h>
#include <libdevcore/Common.h>
#include <libdevcore/easylog.h>
#include <libdevcore/MemoryDB.h>
#include <libdevcore/FileSystem.h>
//ÅÐ¶ÏÊÇ·ñ°üº¬odbc
#if defined ETH_HAVE_ODBC
#include "<odbc/MysqlDB.h>"
#endif

namespace dev
{

class OverlayDB: public MemoryDB
{
public:
	OverlayDB(ldb::DB* _db = nullptr): m_db(_db) 
	{
		m_cryptoMod = dev::getCryptoMod();
		std::map<int, std::string> keyData = dev::getDataKey();
		m_superKey = keyData[0] + keyData[1] + keyData[2] + keyData[3];
		m_ivData = m_superKey.substr(0,16);
	}
	~OverlayDB();

	ldb::DB* db() const { return m_db.get(); }

	void commit();
	void rollback();

	std::string lookup(h256 const& _h) const;
	bool exists(h256 const& _h) const;
	void kill(h256 const& _h);
	bool deepkill(h256 const& _h);

	bytes lookupAux(h256 const& _h) const;

private:
	enum CRYPTOTYPE
	{
		CRYPTO_DEFAULT = 0,
		CRYPTO_LOCAL,
		CRYPTO_KEYCENTER
	};
	int m_cryptoMod;
	std::string m_superKey;
	std::string m_ivData;

	using MemoryDB::clear;
	//ÅÐ¶ÏÊÇ·ñ°üº¬odbc
	#if defined ETH_HAVE_ODBC
	std::shared_ptr<ldb::DB> m_db;
	#else
	std::shared_ptr<ldb::DB> m_db;
	#endif
	
	ldb::ReadOptions m_readOptions;
	ldb::WriteOptions m_writeOptions;
};

}
