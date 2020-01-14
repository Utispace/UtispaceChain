
#pragma once

#include <string>
#include <libdevcore/FixedHash.h>
#include <libcvcore/Common.h>

namespace dev
{
namespace eth
{

/// The network id.
enum class Network
{
    MainNetwork = 1
};
static dev::h256 const c_genesisStateRoot;
static std::string const c_genesisJsonString = std::string() +
R"E(
{
"nonce": "0x0",
"difficulty": "0x0",
"mixhash": "0x0",
"coinbase": "0x0",
"timestamp": "0x0",
"parentHash": "0x0",
"extraData": "0x0",
"gasLimit": "0x13880000000000",
"god":"0x4d23de3297034cdd4a58db35f659a9b61fc7577b",
"alloc": {}, 	"initMinerNodes":["de0fa385816b505799433e54b88788e21cb42092a6ff5bcaa2285d7ace906e5e6ce8ef2b30134ff276a5834d58721291acc5864e07e6d52469b79b28e699dfde"]
}
)E";

static std::string const c_configJsonString = std::string() +
R"E(
{
        "sealEngine": "PBFT",
        "systemproxyaddress":"0x0",
        "listenip":"127.0.0.1",
        "rpcport":      "8545",
        "p2pport":      "30303",
        "wallet":"/mydata/nodedata-1/keys.info",
        "keystoredir":"/mydata/nodedata-1/keystore/",
        "datadir":"/mydata/nodedata-1/",
        "vm":"interpreter",
        "networkid":"12345",
        "logverbosity":"4",
        "coverlog":"OFF",
        "eventlog":"ON",
        "logconf":"/mydata/nodedata-1/log.conf",
        "params": {
                "accountStartNonce": "0x0",
        	  "maximumExtraDataSize": "0x0",
        	 "tieBreakingGas": false,
                "blockReward": "0x0",
                "networkID" : "0x0"
        },
        "NodeextraInfo":[
        {
         					"Nodeid":"de0fa385816b505799433e54b88788e21cb42092a6ff5bcaa2285d7ace906e5e6ce8ef2b30134ff276a5834d58721291acc5864e07e6d52469b79b28e699dfde",
            "Nodedesc": "node1",
            "Agencyinfo": "node1",
            "Peerip": "127.0.0.1",
            "Identitytype": 1,
            "Port":30303,
            "Idx":0
        }
        ]
}
)E";

std::string const& genesisInfo(Network )
{
    return c_configJsonString;
}

h256 const& genesisStateRoot(Network )
{
    return c_genesisStateRoot;
}

}
}
