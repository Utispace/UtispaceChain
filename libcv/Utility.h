

#pragma once

#include <string>
#include <libdevcore/Common.h>
#include <libdevcore/FixedHash.h>

namespace dev
{
namespace eth
{


bytes parseData(std::string const& _args);

void upgradeDatabase(std::string const& _basePath, h256 const& _genesisHash);

}
}
