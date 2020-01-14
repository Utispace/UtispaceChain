

#pragma once

#include <unordered_map>
#include <functional>
#include <libdevcore/CommonData.h>
#include <libdevcore/Exceptions.h>

namespace dev
{
namespace eth
{

using PrecompiledExecutor = std::function<void(bytesConstRef _in, bytesRef _out)>;

DEV_SIMPLE_EXCEPTION(ExecutorNotFound);

class PrecompiledRegistrar
{
public:
	/// Get the executor object for @a _name function or @throw ExecutorNotFound if not found.
	static PrecompiledExecutor const& executor(std::string const& _name);

	/// Register an executor. In general just use ETH_REGISTER_PRECOMPILED.
	static PrecompiledExecutor registerPrecompiled(std::string const& _name, PrecompiledExecutor const& _exec) { return (get()->m_execs[_name] = _exec); }
	/// Unregister an executor. Shouldn't generally be necessary.
	static void unregisterPrecompiled(std::string const& _name) { get()->m_execs.erase(_name); }

private:
	static PrecompiledRegistrar* get() { if (!s_this) s_this = new PrecompiledRegistrar; return s_this; }

	std::unordered_map<std::string, PrecompiledExecutor> m_execs;
	static PrecompiledRegistrar* s_this;
};

// TODO: unregister on unload with a static object.
#define ETH_REGISTER_PRECOMPILED(Name) static void __eth_registerPrecompiledFunction ## Name(bytesConstRef _in, bytesRef _out); static PrecompiledExecutor __eth_registerPrecompiledFactory ## Name = ::dev::eth::PrecompiledRegistrar::registerPrecompiled(#Name, &__eth_registerPrecompiledFunction ## Name); static void __eth_registerPrecompiledFunction ## Name

}
}
