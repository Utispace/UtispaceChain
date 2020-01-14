

include(EthUtils)
if (ODBC)
	set(LIBS ethashseal;ethereum;evm;ethcore;evmcore;ethash-cl;ethash;testutils;pbftseal;odbc)
else ()
	set(LIBS ethashseal;ethereum;evm;ethcore;evmcore;ethash-cl;ethash;testutils;pbftseal)
endif()
set(Eth_INCLUDE_DIRS "${CPP_ETHEREUM_DIR}")

# if the project is a subset of main cpp-ethereum project
# use same pattern for variables as Boost uses
if ((DEFINED CyberVeinChain_VERSION) OR (DEFINED ethereum_VERSION))

	foreach (l ${LIBS})
		string(TOUPPER ${l} L)
		set ("Eth_${L}_LIBRARIES" ${l})
	endforeach()

else()

	foreach (l ${LIBS})
		string(TOUPPER ${l} L)

		find_library(Eth_${L}_LIBRARY
			NAMES ${l}
			PATHS ${CMAKE_LIBRARY_PATH}
			PATH_SUFFIXES "lib${l}" "${l}" "lib${l}/Debug" "lib${l}/Release"
			# libevmjit is nested...
			"evmjit/libevmjit" "evmjit/libevmjit/Release"
			NO_DEFAULT_PATH
		)

		set(Eth_${L}_LIBRARIES ${Eth_${L}_LIBRARY})

		if (MSVC)
			find_library(Eth_${L}_LIBRARY_DEBUG
				NAMES ${l}
				PATHS ${CMAKE_LIBRARY_PATH}
				PATH_SUFFIXES "lib${l}/Debug"
				# libevmjit is nested...
				"evmjit/libevmjit/Debug"
				NO_DEFAULT_PATH
			)
			eth_check_library_link(Eth_${L})
		endif()
	endforeach()

endif()
