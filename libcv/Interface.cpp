

#include "Interface.h"
using namespace std;
using namespace dev;
using namespace eth;

void Interface::submitTransaction(Secret const& _secret, u256 const& _value, Address const& _dest, bytes const& _data, u256 const& _gas, u256 const& _gasPrice, u256 const& _randomid)
{
	TransactionSkeleton ts;
	ts.creation = false;
	ts.value = _value;
	ts.to = _dest;
	ts.data = _data;
	ts.gas = _gas;
	ts.gasPrice = _gasPrice;
	ts.randomid = _randomid;
	submitTransaction(ts, _secret);
}

Address Interface::submitTransaction(Secret const& _secret, u256 const& _endowment, bytes const& _init, u256 const& _gas, u256 const& _gasPrice, u256 const& _randomid)
{
	TransactionSkeleton ts;
	ts.creation = true;
	ts.value = _endowment;
	ts.data = _init;
	ts.gas = _gas;
	ts.gasPrice = _gasPrice;
	ts.randomid = _randomid;
	return submitTransaction(ts, _secret).second;
}

BlockHeader Interface::blockInfo(BlockNumber _block) const
{
	if (_block == PendingBlock)
		return pendingInfo();
	return blockInfo(hashFromNumber(_block));
}

BlockDetails Interface::blockDetails(BlockNumber _block) const
{
	if (_block == PendingBlock)
		return pendingDetails();
	return blockDetails(hashFromNumber(_block));
}
