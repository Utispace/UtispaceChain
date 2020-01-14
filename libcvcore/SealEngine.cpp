

#include "SealEngine.h"
#include "Transaction.h"
#include <libcvvm/ExtVMFace.h>
using namespace std;
using namespace dev;
using namespace eth;

SealEngineRegistrar* SealEngineRegistrar::s_this = nullptr;

void NoProof::init()
{
	ETH_REGISTER_SEAL_ENGINE(NoProof);
}

void SealEngineFace::verify(Strictness _s, BlockHeader const& _bi, BlockHeader const& _parent, bytesConstRef _block) const
{
	_bi.verify(_s, _parent, _block);
}

void SealEngineFace::populateFromParent(BlockHeader& _bi, BlockHeader const& _parent) const
{
	_bi.populateFromParent(_parent);
}

void SealEngineFace::verifyTransaction(ImportRequirements::value _ir, TransactionBase const& _t, BlockHeader const&) const
{
	if (_ir & ImportRequirements::TransactionSignatures)
		_t.checkLowS();
}

SealEngineFace* SealEngineRegistrar::create(ChainOperationParams const& _params)
{
	SealEngineFace* ret = create(_params.sealEngineName);
	assert(ret && "Seal engine not found.");
	if (ret)
		ret->setChainParams(_params);
	return ret;
}

EVMSchedule const& SealEngineBase::evmSchedule(EnvInfo const& _envInfo) const
{
	if (_envInfo.number() >= chainParams().u256Param("EIP158ForkBlock"))
		return EIP158Schedule;
	else if (_envInfo.number() >= chainParams().u256Param("EIP150ForkBlock"))
		return EIP150Schedule;
	else if (_envInfo.number() >= chainParams().u256Param("homsteadForkBlock"))
		return HomesteadSchedule;
	else
		return FrontierSchedule;
}
