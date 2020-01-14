

#include "Executive.h"

#include <boost/timer.hpp>
#include <json/json.h>
#include <libdevcore/CommonIO.h>
#include <libcvvm/VMFactory.h>
#include <libcvvm/VM.h>
#include <libcvcore/CommonJS.h>
#include "Interface.h"
#include "State.h"
#include "ExtVM.h"
#include "BlockChain.h"
#include "Block.h"
using namespace std;
using namespace dev;
using namespace dev::eth;

StandardTrace::StandardTrace():
	m_trace(Json::arrayValue)
{}

bool changesMemory(Instruction _inst)
{
	return
	    _inst == Instruction::MSTORE ||
	    _inst == Instruction::MSTORE8 ||
	    _inst == Instruction::MLOAD ||
	    _inst == Instruction::CREATE ||
	    _inst == Instruction::CALL ||
	    _inst == Instruction::CALLCODE ||
	    _inst == Instruction::SHA3 ||
	    _inst == Instruction::CALLDATACOPY ||
	    _inst == Instruction::CODECOPY ||
	    _inst == Instruction::EXTCODECOPY ||
	    _inst == Instruction::DELEGATECALL;
}

bool changesStorage(Instruction _inst)
{
	return _inst == Instruction::SSTORE;
}

void StandardTrace::operator()(uint64_t _steps, uint64_t PC, Instruction inst, bigint newMemSize, bigint gasCost, bigint gas, VM* voidVM, ExtVMFace const* voidExt)
{
	(void)_steps;

	ExtVM const& ext = dynamic_cast<ExtVM const&>(*voidExt);
	VM& vm = *voidVM;

	Json::Value r(Json::objectValue);

	Json::Value stack(Json::arrayValue);
	if (!m_options.disableStack)
	{
		for (auto const& i : vm.stack())
			stack.append("0x" + toHex(toCompactBigEndian(i, 1)));
		r["stack"] = stack;
	}

	bool newContext = false;
	Instruction lastInst = Instruction::STOP;

	if (m_lastInst.size() == ext.depth)
	{
		// starting a new context
		assert(m_lastInst.size() == ext.depth);
		m_lastInst.push_back(inst);
		newContext = true;
	}
	else if (m_lastInst.size() == ext.depth + 2)
	{
		m_lastInst.pop_back();
		lastInst = m_lastInst.back();
	}
	else if (m_lastInst.size() == ext.depth + 1)
	{
		// continuing in previous context
		lastInst = m_lastInst.back();
		m_lastInst.back() = inst;
	}
	else
	{
		LOG(WARNING) << "GAA!!! Tracing VM and more than one new/deleted stack frame between steps!";
		LOG(WARNING) << "Attmepting naive recovery...";
		m_lastInst.resize(ext.depth + 1);
	}

	Json::Value memJson(Json::arrayValue);
	if (!m_options.disableMemory && (changesMemory(lastInst) || newContext))
	{
		for (unsigned i = 0; i < vm.memory().size(); i += 32)
		{
			bytesConstRef memRef(vm.memory().data() + i, 32);
			memJson.append(toHex(memRef, 2, HexPrefix::DontAdd));
		}
		r["memory"] = memJson;
	}

	if (!m_options.disableStorage && (m_options.fullStorage || changesStorage(lastInst) || newContext))
	{
		Json::Value storage(Json::objectValue);
		for (auto const& i : ext.state().storage(ext.myAddress))
			storage["0x" + toHex(toCompactBigEndian(i.second.first, 1))] = "0x" + toHex(toCompactBigEndian(i.second.second, 1));
		r["storage"] = storage;
	}

	if (m_showMnemonics)
		r["op"] = instructionInfo(inst).name;
	r["pc"] = toString(PC);
	r["gas"] = toString(gas);
	r["gasCost"] = toString(gasCost);
	if (!!newMemSize)
		r["memexpand"] = toString(newMemSize);

	m_trace.append(r);
}

string StandardTrace::json(bool _styled) const
{
	return _styled ? Json::StyledWriter().write(m_trace) : Json::FastWriter().write(m_trace);
}

Executive::Executive(Block& _s, BlockChain const& _bc, unsigned _level):
	m_s(_s.mutableState()),
	m_envInfo(_s.info(), _bc.lastHashes(_s.info().parentHash())),
	m_depth(_level),
	m_sealEngine(*_bc.sealEngine())
{
}

Executive::Executive(Block& _s, LastHashes const& _lh, unsigned _level):
	m_s(_s.mutableState()),
	m_envInfo(_s.info(), _lh),
	m_depth(_level),
	m_sealEngine(*_s.sealEngine())
{
}

Executive::Executive(State& _s, Block const& _block, unsigned _txIndex, BlockChain const& _bc, unsigned _level):
	m_s(_s = _block.fromPending(_txIndex)),
	m_envInfo(_block.info(), _bc.lastHashes(_block.info().parentHash()), _txIndex ? _block.receipt(_txIndex - 1).gasUsed() : 0),
	m_depth(_level),
	m_sealEngine(*_bc.sealEngine())
{
}

u256 Executive::gasUsed() const
{
	return m_t.gas() - m_gas;
}

u256 Executive::gasUsedNoRefunds() const
{
	return m_t.gas() - m_gas + m_refunded;
}

void Executive::accrueSubState(SubState& _parentContext)
{
	if (m_ext)
		_parentContext += m_ext->sub;
}

void Executive::initialize(Transaction const& _transaction)
{
	m_t = _transaction;

	// Avoid transactions that would take us beyond the block gas limit.
	u256 startGasUsed = m_envInfo.gasUsed();

	//判断交易使用的gas和初始gas是否大于环境设置的gaslimit
	if (startGasUsed + (bigint)m_t.gas() > m_envInfo.gasLimit())
	{
		LOG(INFO) << "Cannot fit tx in block" << m_envInfo.number() << ": Require <" << (m_envInfo.gasLimit() - startGasUsed) << " Got" << m_t.gas();
		m_excepted = TransactionException::BlockGasLimitReached;
		BOOST_THROW_EXCEPTION(BlockGasLimitReached() << RequirementError((bigint)(m_envInfo.gasLimit() - startGasUsed), (bigint)m_t.gas()));
	}

	// Check gas cost is enough.

	//判断最小需要的gas是否足够
	m_baseGasRequired = m_t.gasRequired(m_sealEngine.evmSchedule(m_envInfo));
	/*if (m_baseGasRequired > m_t.gas())
	{
		LOG(INFO) << "Not enough gas to pay for the transaction: Require >" << m_baseGasRequired << " Got" << m_t.gas();
		m_excepted = TransactionException::OutOfGasBase;
		BOOST_THROW_EXCEPTION(OutOfGasBase() << RequirementError(m_baseGasRequired, (bigint)m_t.gas()));
	}*/

	// Avoid invalid transactions.
	/*
	u256 nonceReq;
	try
	{
		nonceReq = m_s.getNonce(m_t.sender());//这里会校验签名  块签名那里会验证 rpc会验证 p2p会验证 所以这里不需要了
	}
	catch (...)
	{
		LOG(INFO) << "Invalid Signature";
		m_excepted = TransactionException::InvalidSignature;
		throw;
	}
	*/
	/*if (m_t.nonce() != nonceReq)
	{
		LOG(INFO) << "Invalid Nonce: Require" << nonceReq << " Got" << m_t.nonce();
		m_excepted = TransactionException::InvalidNonce;
		BOOST_THROW_EXCEPTION(InvalidNonce() << RequirementError((bigint)nonceReq, (bigint)m_t.nonce()));
	}*/

	// Avoid unaffordable transactions.
	bigint gasCost = (bigint)m_t.gas() * m_t.gasPrice();
	bigint totalCost = m_t.value() + gasCost;
	/*if (m_s.balance(m_t.sender()) < totalCost)
	{
		LOG(INFO) << "Not enough cash: Require >" << totalCost << "=" << m_t.gas() << "*" << m_t.gasPrice() << "+" << m_t.value() << " Got" << m_s.balance(m_t.sender()) << "for sender: " << m_t.sender();
		m_excepted = TransactionException::NotEnoughCash;
		BOOST_THROW_EXCEPTION(NotEnoughCash() << RequirementError(totalCost, (bigint)m_s.balance(m_t.sender())) << errinfo_comment(m_t.sender().abridged()));
	}*/
	m_gasCost = (u256)gasCost;  // Convert back to 256-bit, safe now.
}

bool Executive::execute()
{
	// Entry point for a user-executed transaction.
	LOG(TRACE) << "Executive::execute into, from :" << m_t.sender();

	// Pay...
	//LOG(INFO) << "Paying" << formatBalance(m_gasCost) << "from sender for gas (" << m_t.gas() << "gas at" << formatBalance(m_t.gasPrice()) << ")";
	//m_s.subBalance(m_t.safeSender(), m_gasCost);
	if (m_t.isCreation())
	{
		bool b = create(m_t.sender(), m_t.value(), m_t.gasPrice(), m_t.gas() - (u256)m_baseGasRequired, &m_t.data(), m_t.sender());
		if (b)
		{
			LOG(TRACE) << "Executive::execute returns true...";
		}
		else
		{
			LOG(TRACE) << "Executive::execute return false...";
		}
		return b;
	}
	else
	{
		LOG(TRACE) << "Executive::execute m_t.receiveAddress:" << toString(m_t.receiveAddress()) << ",m_t.safeSender:" << toString(m_t.safeSender());
		return call(m_t.receiveAddress(), m_t.safeSender(), m_t.value(), m_t.gasPrice(), bytesConstRef(&m_t.data()), m_t.gas() - (u256)m_baseGasRequired);
	}
}

bool Executive::call(Address _receiveAddress, Address _senderAddress, u256 _value, u256 _gasPrice, bytesConstRef _data, u256 _gas)
{
	LOG(TRACE) << "Executive::call into, _receiveAddress :" << toString(_receiveAddress) << ",_senderAddress:" << toString(_senderAddress) << ",_value:" << _value;
	CallParameters params{_senderAddress, _receiveAddress, _receiveAddress, _value, _value, _gas, _data, {}, {}};

	return call(params, _gasPrice, _senderAddress);
}

bool Executive::call(CallParameters const& _p, u256 const& _gasPrice, Address const& _origin)
{
	LOG(TRACE) << "Executive::call" << toString(_p.senderAddress);
	// If external transaction.
	if (m_t)
	{
		// FIXME: changelog contains unrevertable balance change that paid
		//        for the transaction.
		// Increment associated nonce for sender.
		LOG(TRACE) << "Executive::call 1";
		m_s.incNonce(_p.senderAddress);
	}

	m_savepoint = m_s.savepoint();
	LOG(TRACE) << "Executive::call 2";

	//对合约代码判断是否已经在启动参数中预编译过 m_params是否存在
	if (m_sealEngine.isPrecompiled(_p.codeAddress))
	{
		LOG(TRACE) << "Executive::call 3";
		bigint g = m_sealEngine.costOfPrecompiled(_p.codeAddress, _p.data);
		LOG(TRACE) << "Executive::call 4";
		if (_p.gas < g)
		{
			LOG(TRACE) << "Executive::call 5";
			m_excepted = TransactionException::OutOfGasBase;
			LOG(TRACE) << "Executive::call 6";
			// Bail from exception.
			return true;	// true actually means "all finished - nothing more to be done regarding go().
		}
		else
		{
			LOG(TRACE) << "Executive::call 7";
			m_gas = (u256)(_p.gas - g);
			LOG(TRACE) << "Executive::call 8";
			m_sealEngine.executePrecompiled(_p.codeAddress, _p.data, _p.out);
			LOG(TRACE) << "Executive::call 9";
		}
	}
	else
	{
		LOG(TRACE) << "Executive::call 10";
		m_gas = _p.gas;
		//判断该地址是否有可执行代码
		if (m_s.addressHasCode(_p.codeAddress))
		{
			LOG(TRACE) << "Executive::call 11";
			m_outRef = _p.out; // Save ref to expected output buffer to be used in go()
			LOG(TRACE) << "Executive::call 12";
			bytes const& c = m_s.code(_p.codeAddress);
			LOG(TRACE) << "Executive::call 13";
			h256 codeHash = m_s.codeHash(_p.codeAddress);
			LOG(TRACE) << "Executive::call 14";
			m_ext = make_shared<ExtVM>(m_s, m_envInfo, m_sealEngine, _p.receiveAddress, _p.senderAddress, _origin, _p.apparentValue, _gasPrice, _p.data, &c, codeHash, m_depth);
			LOG(TRACE) << "Executive::call 15";
		}
	}
	LOG(TRACE) << "Executive::call 16";
	// Transfer ether.
	m_s.transferBalance(_p.senderAddress, _p.receiveAddress, _p.valueTransfer);
	LOG(TRACE) << "Executive::call 17";
	return !m_ext;
}

bool Executive::create(Address _sender, u256 _endowment, u256 _gasPrice, u256 _gas, bytesConstRef _init, Address _origin)
{
	LOG(TRACE) << "Executive::create" << _sender;

	u256 nonce = m_s.getNonce(_sender);
	m_s.incNonce(_sender);

	m_savepoint = m_s.savepoint();

	m_isCreation = true;

	// We can allow for the reverted state (i.e. that with which m_ext is constructed) to contain the m_orig.address, since
	// we delete it explicitly if we decide we need to revert.
	m_newAddress = right160(sha3(rlpList(_sender, nonce)));
	LOG(TRACE) << "Executive::create " << m_newAddress << "," << _sender << "," << nonce;
	m_gas = _gas;

	// Transfer ether before deploying the code. This will also create new
	// account if it does not exist yet.
	//这里调用addBalance的时候会创建新合约账户，这样下面ExtVM的assert就不会失败
	m_s.transferBalance(_sender, m_newAddress, _endowment);
	LOG(TRACE) << "Executive::create transferBalance isok";

	if (m_envInfo.number() >= m_sealEngine.chainParams().u256Param("EIP158ForkBlock"))
		m_s.incNonce(m_newAddress);

	LOG(TRACE) << "Executive::create number:"<< m_envInfo.number();
	// Schedule _init execution if not empty.
	if (!_init.empty()) {
		LOG(TRACE) << "Executive::create _init is not empty...";
		m_ext = make_shared<ExtVM>(m_s, m_envInfo, m_sealEngine, m_newAddress, _sender, _origin, _endowment, _gasPrice, bytesConstRef(), _init, sha3(_init), m_depth);
		
	}
	else if (m_s.addressHasCode(m_newAddress))
	{
		m_s.setNewCode(m_newAddress, {});
		LOG(TRACE) << "Executive::create addressHasCode...";
	}
	if (m_ext)
	{
		LOG(TRACE) << "Executive::create m_ext is true ";

	}
	else
	{
		LOG(TRACE) << "Executive::create m_ext is false ";

	}

	return !m_ext;
}

OnOpFunc Executive::simpleTrace()
{
	return [](uint64_t steps, uint64_t PC, Instruction inst, bigint newMemSize, bigint gasCost, bigint gas, VM * voidVM, ExtVMFace const * voidExt)
	{
		ExtVM const& ext = *static_cast<ExtVM const*>(voidExt);
		VM& vm = *voidVM;

		ostringstream o;

		o << "\n" << "    STACK" << "\n";
		for (auto i : vm.stack())
			o << (h256)i << "\n";
		o << "    MEMORY" << "\n" << ((vm.memory().size() > 1000) ? " mem size greater than 1000 bytes " : memDump(vm.memory()));
		o << "    STORAGE" << "\n";
		for (auto const& i : ext.state().storage(ext.myAddress))
			o << showbase << hex << i.second.first << ": " << i.second.second << "\n";
		LOG(TRACE) << o.str();
		LOG(TRACE) << " < " << dec << ext.depth << " : " << ext.myAddress << " : #" << steps << " : " << hex << setw(4) << setfill('0') << PC << " : " << instructionInfo(inst).name << " : " << dec << gas << " : -" << dec << gasCost << " : " << newMemSize << "x32" << " >";
	};
}

bool Executive::go(OnOpFunc const& _onOp)
{
	LOG(TRACE)<<"Executive::go";

	if (m_ext)
	{

		Timer t;

		try
		{
			// Create VM instance. Force Interpreter if tracing requested.
			auto vm = _onOp ? VMFactory::create(VMKind::Interpreter) : VMFactory::create();
			if (m_isCreation)
			{
				LOG(TRACE) << "Executive::go isCreation...";

				auto out = vm->exec(m_gas, *m_ext, _onOp);
				LOG(TRACE) << "Executive::go m_res:" <<m_res;

				if (m_res)
				{
					m_res->gasForDeposit = m_gas;
					m_res->depositSize = out.size();
				}
				LOG(TRACE) << "Executive::go out.size:" << out.size() << ",maxCodeSize:" << m_ext->evmSchedule().maxCodeSize;

				if (out.size() > m_ext->evmSchedule().maxCodeSize)
					BOOST_THROW_EXCEPTION(OutOfGas());
				else if (out.size() * m_ext->evmSchedule().createDataGas <= m_gas)
				{
					LOG(TRACE) << "Executive::go 1";

					if (m_res)
					{
						LOG(TRACE) << "Executive::go 2";
						m_res->codeDeposit = CodeDeposit::Success;

					}
					LOG(TRACE) << "Executive::go 3";

					m_gas -= out.size() * m_ext->evmSchedule().createDataGas;

					LOG(TRACE) << "Executive::go 4";

				}
				else
				{
					LOG(TRACE) << "Executive::go 5";

					if (m_ext->evmSchedule().exceptionalFailedCodeDeposit)
						BOOST_THROW_EXCEPTION(OutOfGas());
					else
					{
						LOG(TRACE) << "Executive::go 6";

						if (m_res)
						{
							LOG(TRACE) << "Executive::go 7";
							m_res->codeDeposit = CodeDeposit::Failed;
						}
						LOG(TRACE) << "Executive::go 8";

						out.clear();
					}
				}
				if (m_res)
				{
					LOG(TRACE) << "Executive::go 9";
					m_res->output = out; // copy output to execution result
				}

				LOG(TRACE)<<"Executive::go setCode "<<m_newAddress<<","<<toHex(out);

				m_s.setNewCode(m_ext->myAddress, std::move(out));
			}
			else
			{
				LOG(TRACE) << "Executive::go 10";

				if (m_res)
				{
					LOG(TRACE) << "Executive::go 11";

					m_res->output = vm->exec(m_gas, *m_ext, _onOp); // take full output
					bytesConstRef{&m_res->output} .copyTo(m_outRef);
				}
				else
				{
					LOG(TRACE) << "Executive::go 12";
					vm->exec(m_gas, *m_ext, m_outRef, _onOp); // take only expected output

				}
			}
		}
		catch (VMException const& _e)
		{
			LOG(INFO) << "Safe VM Exception. " << diagnostic_information(_e);
			m_gas = 0;
			m_excepted = toTransactionException(_e);
			revert();
		}
		catch (Exception const& _e)
		{
			// TODO: AUDIT: check that this can never reasonably happen. Consider what to do if it does.
			LOG(WARNING) << "Unexpected exception in VM. There may be a bug in this implementation. " << diagnostic_information(_e);
			exit(1);
			// Another solution would be to reject this transaction, but that also
			// has drawbacks. Essentially, the amount of ram has to be increased here.
		}
		catch (std::exception const& _e)
		{
			// TODO: AUDIT: check that this can never reasonably happen. Consider what to do if it does.
			LOG(WARNING) << "Unexpected std::exception in VM. Not enough RAM? " << _e.what();
			exit(1);
			// Another solution would be to reject this transaction, but that also
			// has drawbacks. Essentially, the amount of ram has to be increased here.
		}

		 LOG(INFO) << "VM took:" << (t.elapsed() * 1000000) << " Hash=" << (m_t.sha3()) << ",Randid=" << m_t.randomid();
	}
	return true;
}

void Executive::finalize()
{
	// Accumulate refunds for suicides.
	if (m_ext)
		m_ext->sub.refunds += m_ext->evmSchedule().suicideRefundGas * m_ext->sub.suicides.size();

	// SSTORE refunds...
	// must be done before the miner gets the fees.
	m_refunded = m_ext ? min((m_t.gas() - m_gas) / 2, m_ext->sub.refunds) : 0;
	m_gas += m_refunded;

	if (m_t)
	{
		//m_s.addBalance(m_t.sender(), m_gas * m_t.gasPrice());

		//u256 feesEarned = (m_t.gas() - m_gas) * m_t.gasPrice();
		//m_s.addBalance(m_envInfo.author(), feesEarned);
	}

	// Suicides...
	if (m_ext)
		for (auto a : m_ext->sub.suicides)
			m_s.kill(a);

	// Logs..
	if (m_ext)
		m_logs = m_ext->sub.logs;

	if (m_res) // Collect results
	{
		m_res->gasUsed = gasUsed();
		m_res->excepted = m_excepted; // TODO: m_except is used only in ExtVM::call
		m_res->newAddress = m_newAddress;
		m_res->gasRefunded = m_ext ? m_ext->sub.refunds : 0;
	}
}

void Executive::revert()
{
	if (m_ext)
		m_ext->sub.clear();

	// Set result address to the null one.
	m_newAddress = {};
	m_s.rollback(m_savepoint);
}
