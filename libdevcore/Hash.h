
#pragma once

#include <string>
#include "FixedHash.h"
#include "vector_ref.h"
#include "SHA3.h"

namespace dev
{

h256 sha256(bytesConstRef _input);

h160 ripemd160(bytesConstRef _input);

}
