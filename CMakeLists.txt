

cmake_minimum_required(VERSION 3.0.0)

set(ETH_CMAKE_DIR "${CMAKE_CURRENT_LIST_DIR}/cmake" CACHE PATH "The path to the cmake directory")
list(APPEND CMAKE_MODULE_PATH ${ETH_CMAKE_DIR})

set(CPP_ETHEREUM_DIR "${CMAKE_CURRENT_LIST_DIR}" CACHE PATH "Path to the root directory for CyberVein")

message("CMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}")

# set cmake_policies
include(EthPolicy)
eth_policy()

# project name and version should be set after cmake_policy CMP0048
project(CyberVeinChain VERSION "1.1.0")


set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY On)

include(EthOptions)
include(EthCompilerSettings)
include(EthExecutableHelper)
include(EthDependencies)
include(EthUtils)

include(ProjectBoost)
include(ProjectCryptopp)
include(ProjectJsonCpp)
include(ProjectJsonRpcCpp)
include(ProjectSecp256k1)

configure_project(CPUID CURL EVMJIT FATDB ROCKSDB PARANOID VMTRACE)

add_subdirectory(cv)
add_subdirectory(libdevcore)
add_subdirectory(libdevcrypto)
add_subdirectory(libpbftseal)
add_subdirectory(libcvcore)
add_subdirectory(libcv)
add_subdirectory(libcvvm)
add_subdirectory(libcvvmcore)

if (NOT EMSCRIPTEN)
    add_subdirectory(libp2p)
endif()

add_subdirectory(libchannelserver)
add_subdirectory(libweb3jsonrpc)
add_subdirectory(libwebthree)
#add_subdirectory(abi)
add_subdirectory(utils)
