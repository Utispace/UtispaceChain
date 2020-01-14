
#pragma once

#include "VMInterface.h"

namespace dev
{
namespace eth
{

enum class VMKind
{
	Interpreter,
	JIT,
	Smart,
	Dual
};

class VMFactory
{
public:
	VMFactory() = delete;

	static std::unique_ptr<VMFace> create();

	static std::unique_ptr<VMFace> create(VMKind _kind);

	static void setKind(VMKind _kind);

	static VMKind getKind();
};

}
}
