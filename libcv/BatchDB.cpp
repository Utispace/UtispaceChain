
#include "BatchDB.h"
#include <libdevcore/FileSystem.h>
#include <libdevcore/easylog.h>

#include <iostream>
#include <string>
using namespace std;



BatchDB::BatchDB(void)
{
	LOG(DEBUG)<<"BatchDB::BatchDB";
}


BatchDB::~BatchDB(void)
{

}

ldb::Status BatchDB::Put(ldb::Slice const& key, ldb::Slice const& value)
{
	ldb::Status _status;

	ldb::WriteBatch::Put(key,value);
	
	return _status;
}

