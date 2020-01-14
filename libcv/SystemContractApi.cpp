

#include "SystemContractApi.h"
#include "SystemContract.h"

using namespace dev::eth;

//允许不同的实现
std::shared_ptr<SystemContractApi> SystemContractApiFactory::create(const Address _address, const Address _god, Client* _client)
{
    //return std::unique_ptr<SystemContractApi>(new SystemContractApi);
	return shared_ptr<SystemContractApi>(new SystemContract(_address, _god, _client));
}