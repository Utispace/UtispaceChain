
#pragma once

#include "VMFace.h"

namespace dev
{
namespace eth
{

class SmartVM: public VMFace
{
public:
	virtual bytesConstRef execImpl(u256& io_gas, ExtVMFace& _ext, OnOpFunc const& _onOp) override final;

private:
	std::unique_ptr<VMFace> m_selectedVM;
};

}
}
