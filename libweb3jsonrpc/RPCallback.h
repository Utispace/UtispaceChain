
#pragma once

#include <iostream>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <unordered_map>
#include <libcv/Block.h>
#include <libdevcore/Guards.h>
#include <libdevcore/Worker.h>
#include <libchannelserver/ChannelSession.h>
#include <libweb3jsonrpc/AccountHolder.h>

using namespace std;
using namespace dev::channel;
using namespace dev::eth;

namespace dev {
    namespace eth {
        
        struct SeqSessionInfo {
            ChannelSession::Ptr session;
            string seq;
        };
        
        typedef std::shared_ptr<SeqSessionInfo> SSPtr;
        
        class RPCallback {
        public:
            //实例
            static RPCallback& getInstance();
            
            //保存hash和session的映射
            bool parseAndSaveSession(const string& jsonReqStr, const string& seq, ChannelSession::Ptr session);
            
            //查找session
            SSPtr getSessionInfoByHash(std::string hash);
            
            //设置accountHolder,用到里面的密钥签名
            void setAccountHolder(AccountHolder* _ethAccounts) { m_ethAccounts = _ethAccounts;}
        private:
            RPCallback();
            unordered_map<std::string, SSPtr> m_hashSessionMap;
            SharedMutex x_map;
            SharedMutex x_sessionMap;
            AccountHolder* m_ethAccounts;
        };
        
        class CallbackWorker : public Worker {
        public:
            virtual ~CallbackWorker(){};
            static const uint MAX_SZIE = 100;
            static CallbackWorker& getInstance();
            virtual void doWork() override;
            bool addBlock(shared_ptr<Block> _block);
        private:
            CallbackWorker(): Worker("CallbackWorker", 20){
                startWorking();
            }
            void checkIf(shared_ptr<Block> _block);
            
            vector<std::shared_ptr<Block>> m_blocks;
            SharedMutex x_block;
        };
    }
}

