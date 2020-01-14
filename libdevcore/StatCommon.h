

#pragma once

namespace dev
{

enum StatCode
{
    // db
    DB_GET = 1,
    DB_SET,
    DB_GET_SIZE,
    DB_SET_SIZE,
    DB_HIT_MEM,

    // tx exec
    TX_EXEC = 10,

    // tx flow
    TX_TRACE = 20,

    // block flow
    BLOCK_SEAL = 40,
    BLOCK_EXEC = 41,
    BLOCK_SIGN = 42,
    BLOCK_COMMIT = 43,
    BLOCK_BLKTOCHAIN = 44,
    BLOCK_VIEWCHANG = 45,
    
    // broadcast
    BROADCAST_BLOCK_SIZE = 10000, 
    BROADCAST_TX_SIZE = 20000,

    // error code
    ERROR_CODE = 0x60000000,
    // hight code
    NO_CODE = 0x70000000
};

// log tag
#define STAT_DB_GET "DB get"
#define STAT_DB_SET "DB set"
#define STAT_DB_GET_SIZE "DB get size"
#define STAT_DB_SET_SIZE "DB set size"
#define STAT_DB_HIT_MEM "DB hit mem"

#define STAT_PBFT_VIEWCHANGE_TAG "PBFT ViewChange"
#define STAT_TX_EXEC "TX Exec"
#define STAT_TX_TRACE "Tx Trace Time"
#define STAT_BLOCK_PBFT_SEAL "PBFT Seal Time"
#define STAT_BLOCK_PBFT_EXEC "PBFT Exec Time"
#define STAT_BLOCK_PBFT_SIGN "PBFT Sign Time"
#define STAT_BLOCK_PBFT_COMMIT "PBFT Commit Time"
#define STAT_BLOCK_PBFT_CHAIN "PBFT BlkToChain Time"

#define STAT_BLOCK_PBFT_VIEWCHANGE "PBFT viewchange time"

#define STAT_BROADCAST_BLOCK_SIZE "Broadcast Blk size"
#define STAT_BROADCAST_TX_SIZE "Broadcast Tx size"

}
