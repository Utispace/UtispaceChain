

#pragma once
#include <iostream>
#include <libdevcore/db.h>
#include <leveldb/db.h>
#include <libdevcore/Common.h>
using namespace std;
using namespace dev;
using namespace ldb;

class BatchDB: public ldb::WriteBatch
{
public:
	BatchDB(void);
	~BatchDB(void);
	ldb::Status Put(ldb::Slice const& key, ldb::Slice const& value);
};
