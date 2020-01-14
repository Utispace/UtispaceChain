aux_source_directory(. SRC_LIST)

file(GLOB HEADERS "*.h")

include(EthDependencies)

add_executable(CyberVeinChain ${SRC_LIST} ${HEADERS})

find_package(Eth)
find_package(Web3)

target_include_directories(CyberVeinChain PRIVATE ..)
					
target_link_libraries(CyberVeinChain ${Eth_EVM_LIBRARIES})
target_link_libraries(CyberVeinChain ${Web3_WEB3JSONRPC_LIBRARIES})
target_link_libraries(CyberVeinChain ${Web3_WEBTHREE_LIBRARIES})
target_link_libraries(CyberVeinChain JsonRpcCpp::Client)
target_link_libraries(CyberVeinChain pbftseal)
target_link_libraries(CyberVeinChain channelserver)
target_link_libraries(CyberVeinChain web3jsonrpc)
#target_link_libraries(CyberVeinChain abi)

if (UNIX AND NOT APPLE)
	target_link_libraries(CyberVeinChain pthread)
endif()

install(TARGETS CyberVeinChain DESTINATION bin)
