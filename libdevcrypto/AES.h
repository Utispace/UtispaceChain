
#pragma once

#include "Common.h"

namespace dev
{

bytes aesDecrypt(bytesConstRef _cipher, std::string const& _password, unsigned _rounds = 2000, bytesConstRef _salt = bytesConstRef());
bytes aesCBCEncrypt(bytesConstRef plainData,std::string const& keyData,int keyLen,bytesConstRef ivData);
bytes aesCBCDecrypt(bytesConstRef cipherData,std::string const& keyData,int keyLen,bytesConstRef ivData);
}
