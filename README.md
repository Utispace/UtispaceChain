# UtiSpaceChain
Official C++ implementation of UtiSpace project - <http://utispace.com>

Welcome to the UtiSpace source code repository!

Main modules of repository are p2p network protocol, simple wallet, account management and consensus algorithm.

There is no testnet running currently.

## Supported Operating Systems
Centos 7


## Code architecture / 项目代码架构
### cv
UtiSpace core service, main module to start blockchain node.
UtiSpace核心服务，区块链节点启动时的核心库，主要入口
### cvpoc
UtiSpace proof of contribution, Consensus algorithm. (Not implemented, currently using Practical Byzantine Fault Tolerance).
UtiSpace proof of contribution，共识算法
### cvvm
UtiSpace virtual machine, capable of running smart contracts.
UtiSpace virtual machine，基于EVM改造，执行智能合约的虚拟机
### libdevcore
Core blockchain service library, blockchain data structure definition, memory pool management, queue management.
核心服务库，区块链核心库，包括块链数据结构定义、内存池管理、队列管理等等
### libdevcrypto
Encryption algorithm library.
加密算法库
### libp2p
P2P communication protocol, communication management and exception handling.
底层通信协议代码库，包括p2p网络连接、会话管理、异常处理等底层网络相关的代码组件

## 编译代码
### 环境准备
Cent OS 7.2 64位服务器

yum install -y git openssl openssl-devel deltarpm cmake3 gcc-c++

yum install -y nodejs

### 获取源码
cd /data

git clone https://github.com/UtiSpace/UtispaceChain.git

cd UtispaceChain

mkdir build

cd ./build

### 开始编译
cmake3 ../

make

make install


