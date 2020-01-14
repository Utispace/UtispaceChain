
#if !defined(ETH_EMSCRIPTEN)

#include "Common.h"
#include "TrieDB.h"
using namespace std;
using namespace dev;

h256 const dev::c_shaNull = sha3(rlp(""));
h256 const dev::EmptyTrie = sha3(rlp(""));



#endif // ETH_EMSCRIPTEN
