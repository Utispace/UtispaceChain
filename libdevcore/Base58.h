
#pragma once

#include <string>
#include "Common.h"
#include "FixedHash.h"

namespace dev
{

extern std::string AlphabetIPFS;
extern std::string AlphabetFlickr;

std::string toBase58(bytesConstRef _in, std::string const& _alphabet = AlphabetIPFS);
bytes fromBase58(std::string const& _in, std::string const& _alphabet = AlphabetIPFS);

}
