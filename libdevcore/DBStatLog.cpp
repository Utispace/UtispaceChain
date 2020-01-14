
#include "DBStatLog.h"

using namespace statemonitor;
namespace dev
{

uint64_t DBGetLogGuard::report_interval(60);  // 1min

uint64_t DBSetLogGuard::report_interval(60);

uint64_t DBMemHitGuard::report_interval(60);

uint64_t DBInterval::DB_get_size_interval(60);

uint64_t DBInterval::DB_set_size_interval(60);

void statGetDBSizeLog(uint64_t s)
{
    recordStateByTimeOnce(StatCode::DB_GET_SIZE, DBInterval::DB_get_size_interval, (double)s, STAT_DB_GET_SIZE, "");
}

void statSetDBSizeLog(uint64_t s)
{
    recordStateByTimeOnce(StatCode::DB_SET_SIZE, DBInterval::DB_set_size_interval, (double)s, STAT_DB_SET_SIZE, "");
}

}
