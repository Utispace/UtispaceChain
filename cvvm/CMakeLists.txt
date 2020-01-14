
set(SOURCES
	ExtVMFace.cpp
	VM.cpp
	VMOpt.cpp
	VMCalls.cpp
	VMFactory.cpp
	CoverTool.cpp
)

file(GLOB HEADERS "*.h" )

add_library(evm ${SOURCES} ${HEADERS})

find_package(Eth)

target_link_libraries(evm ${Eth_ETHCORE_LIBRARIES})
target_link_libraries(evm ${Eth_EVMCORE_LIBRARIES})
target_include_directories(evm PRIVATE .. ${JSONCPP_INCLUDE_DIR})


install(TARGETS evm RUNTIME DESTINATION bin ARCHIVE DESTINATION lib LIBRARY DESTINATION lib)
