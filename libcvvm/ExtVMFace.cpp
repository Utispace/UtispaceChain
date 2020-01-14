

#include "ExtVMFace.h"
#include <libcvcore/SealEngine.h>
using namespace dev;
using namespace dev::eth;

ExtVMFace::ExtVMFace(EnvInfo const& _envInfo, Address _myAddress, Address _caller, Address _origin, u256 _value, u256 _gasPrice, bytesConstRef _data, bytes _code, h256 const& _codeHash, unsigned _depth):
	m_envInfo(_envInfo),
	myAddress(_myAddress),
	caller(_caller),
	origin(_origin),
	value(_value),
	gasPrice(_gasPrice),
	data(_data),
	code(std::move(_code)),
	codeHash(_codeHash),
	depth(_depth)
{}
