


#pragma once

#include <libdevcore/Common.h>
#include <libdevcore/Guards.h>
#include <libdevcore/easylog.h>
#include <libdevcore/CommonIO.h>
#include <boost/timer.hpp>
#include <thread>
#include <libcv/Transaction.h>
#include <libcv/Block.h>
#include <libcv/BlockChain.h>


using namespace std;
using namespace dev::eth;

namespace dev
{
namespace eth
{


class NonceCheck 
{
private:   
    //不用 起线程refresh 
  
    std::unordered_map< std::string,bool  > m_cache;    
    unsigned m_startblk;  //cache中最老的块号
    unsigned m_endblk;  //cache中最新的块号

    

    mutable SharedMutex  m_lock;
    
   std::string generateKey(Transaction const & _t);  //根据交易信息生成 命中的value，后面可以加blocklimit 
   
public:
    static u256    maxblocksize;//最多缓存多少个块的的交易的nonce 从全网配置中读取

    // 构建cache 
    NonceCheck(){}



	 ~NonceCheck() ;

	void init(BlockChain const& _bc);
	//如果存在cache中，返回false 否则true
    bool ok(Transaction const & _transaction,bool _needinsert=false/*如果不存在是否插入*/);
   

    void updateCache(BlockChain const& _bc,bool _rebuild=false);//每次import新块的时候 更新缓存 

    void delCache( Transactions const & _transcations);//考虑 当前块处理所有交易的回滚

};



}
}
