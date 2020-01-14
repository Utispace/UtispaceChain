
#pragma once

#include "LogGuard.h"

namespace dev 
{

class DBGetLogGuard : public TimeIntervalLogGuard
{
public:
    DBGetLogGuard() : TimeIntervalLogGuard(StatCode::DB_GET, STAT_DB_GET, report_interval) {}
    static uint64_t report_interval;
};

class DBSetLogGuard : public TimeIntervalLogGuard
{
public:
    DBSetLogGuard() : TimeIntervalLogGuard(StatCode::DB_SET, STAT_DB_SET, report_interval) {}
    static uint64_t report_interval;
};

class DBMemHitGuard : public TimeIntervalLogGuard
{
public:
    DBMemHitGuard() : TimeIntervalLogGuard(StatCode::DB_HIT_MEM, STAT_DB_HIT_MEM, report_interval) 
    {
        m_success = 0;
    }
    void hit() { m_success = 1; }
    static uint64_t report_interval;
};

class DBInterval 
{
public:
    static uint64_t DB_get_size_interval;
    static uint64_t DB_set_size_interval;
};

void statGetDBSizeLog(uint64_t s);
void statSetDBSizeLog(uint64_t s);

}
