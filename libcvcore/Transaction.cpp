

#include <libdevcore/vector_ref.h>
#include <libdevcore/easylog.h>
#include <libdevcore/CommonIO.h>
#include <libdevcrypto/Common.h>
#include <libcvvmcore/EVMSchedule.h>
#include <libcvcore/Exceptions.h>
#include <libdevcore/easylog.h>
#include <libcvcore/CommonJS.h>
#include <libweb3jsonrpc/JsonHelper.h>
#include "Transaction.h"
#include <libdevcore/easylog.h>
using namespace std;
using namespace dev;
using namespace dev::eth;

u256 TransactionBase::maxGas = 30000000; //默认二千万

TransactionBase::TransactionBase(TransactionSkeleton const& _ts, Secret const& _s):
	m_type(_ts.creation ? ContractCreation : MessageCall),
	m_randomid(_ts.randomid),
	m_value(_ts.value),
	m_receiveAddress(_ts.to),
	m_gasPrice(_ts.gasPrice),
	m_gas(_ts.gas),
	m_blockLimit(_ts.blockLimit),
	m_data(_ts.data),
	m_sender(_ts.from)
{
	if (_s)
		sign(_s);
}

TransactionBase::TransactionBase(bytesConstRef _rlpData, CheckTransaction _checkSig)
{
	int field = 0;
	RLP rlp(_rlpData);
	try
	{
		transactionRLPDecode(_rlpData);

		if (_checkSig >= CheckTransaction::Cheap && !m_vrs.isValid()) {

			BOOST_THROW_EXCEPTION(InvalidSignature());
		}

		if (_checkSig == CheckTransaction::Everything) {
			m_sender = sender();
		}
	}
	catch (Exception& _e)
	{
		_e << errinfo_name("invalid transaction format: " + toString(rlp) + " RLP: " + toHex(rlp.data()));
		throw;
	}
}

void TransactionBase::transactionRLPDecode(bytesConstRef _rlp)
{
	RLP rlp(_rlp);
	if (!rlp.isList())
		BOOST_THROW_EXCEPTION(InvalidTransactionFormat() << errinfo_comment("transaction RLP must be a list"));

	size_t rlpItemCount = rlp.itemCount();
	if (rlpItemCount == 10)
	{
		transactionRLPDecode10Ele(rlp);
	}
	/*else if (rlpItemCount == 13)
	{
		transactionRLPDecode13Ele(rlp);
	}*/
	else
	{
		BOOST_THROW_EXCEPTION(InvalidTransactionFormat() << errinfo_comment("invalid fields in the transaction RLP"));
	}
}

void TransactionBase::transactionRLPDecode10Ele(const RLP &rlp)
{
	int index = 0;
	m_randomid       = rlp[index++].toInt<u256>(); // 0 
	m_gasPrice       = rlp[index++].toInt<u256>(); // 1
	m_gas            = rlp[index++].toInt<u256>(); // 2
	m_blockLimit     = rlp[index++].toInt<u256>(); // 3
	m_receiveAddress = rlp[index/*注意这里没有++,条件表达式被使用了两次*/].isEmpty() ? Address() : rlp[index].toHash<Address>(RLP::VeryStrict); // 4
	index++;  //跳过m_receiveAddress的++
	m_value          = rlp[index++].toInt<u256>(); // 5
	auto dataRLP     = rlp[index++];               // 6
	m_data           = dataRLP.toBytes();          

	byte v           = rlp[index++].toInt<byte>(); // 7
	h256 r           = rlp[index++].toInt<u256>(); // 8
	h256 s           = rlp[index++].toInt<u256>(); // 9

	if (v > 36)
		m_chainId = (v - 35) / 2;
	else if (v == 27 || v == 28)
		m_chainId = -4;
	else
		BOOST_THROW_EXCEPTION(InvalidSignature());

	v = v - (m_chainId * 2 + 35);
	m_vrs = SignatureStruct{ r, s, v };


	m_type            = (m_receiveAddress == Address() ? ContractCreation : MessageCall);
	m_transactionType = DefaultTransaction;

	LOG(DEBUG) << "DefaultTransaction to address = " << m_receiveAddress;
	
}

/*void TransactionBase::transactionRLPDecode13Ele(const RLP &rlp)
{
	int index = 0;
	
	m_randomid        = rlp[index++].toInt<u256>(); //0
	m_gasPrice        = rlp[index++].toInt<u256>(); //1
	m_gas             = rlp[index++].toInt<u256>(); //2
	m_blockLimit      = rlp[index++].toInt<u256>(); //3
	m_receiveAddress  = rlp[index].isEmpty() ? Address() : rlp[index].toHash<Address>(RLP::VeryStrict); //4
	m_type            = rlp[index++].isEmpty() ? ContractCreation : MessageCall;
	m_value           = rlp[index++].toInt<u256>(); //5
	if (!rlp[index].isData())
		BOOST_THROW_EXCEPTION(InvalidTransactionFormat() << errinfo_comment("transaction data RLP must be an array"));
	m_data            = rlp[index++].toBytes();     // 6
	m_strCNSName      = rlp[index++].toString();    // 7
	m_strCNSVer       = rlp[index++].toString();    // 8
	m_cnsType         = rlp[index++].toInt<u256>(); // 9

	byte v            = rlp[index++].toInt<byte>(); // 10
	h256 r            = rlp[index++].toInt<u256>(); // 11
	h256 s            = rlp[index++].toInt<u256>(); // 12

	if (v > 36)
		m_chainId = (v - 35) / 2;
	else if (v == 27 || v == 28)
		m_chainId = -4;
	else
		BOOST_THROW_EXCEPTION(InvalidSignature());

	v = v - (m_chainId * 2 + 35);

	m_vrs             = SignatureStruct{ r, s, v };
	m_transactionType = CNSNewTransaction;
	m_type            = (m_receiveAddress == Address() && m_strCNSName.empty()) ? ContractCreation : MessageCall;

	LOG(DEBUG) << "[CNSNewTransaction] cnsName|cnsVersion|isNewCNS|m_type="
		<< m_strCNSName << "|"
		<< m_strCNSVer << "|"
		<< isNewCNS() << "|"
		<< m_type;
}*/


Address TransactionBase::receiveAddress() const
{
	return m_receiveAddress;
}
							   /// Synonym for receiveAddress().
Address TransactionBase::to() const
{
	return m_receiveAddress;
}

bytes const&  TransactionBase::data() const
{
	return m_data;
}

Address const& TransactionBase::safeSender() const noexcept
{
	try
	{
		return sender();
	}
	catch (...)
	{
		return ZeroAddress;
	}
}

Address const& TransactionBase::sender() const
{
	if (!m_sender)
	{
		auto p = recover(m_vrs, sha3(WithoutSignature));

		if (!p)
			BOOST_THROW_EXCEPTION(InvalidSignature());
		m_sender = right160(dev::sha3(bytesConstRef(p.data(), sizeof(p))));

	}

	return m_sender;
}

void TransactionBase::sign(Secret const& _priv)
{
	auto sig = dev::sign(_priv, sha3(WithoutSignature));
	SignatureStruct sigStruct = *(SignatureStruct const*)&sig;
	if (sigStruct.isValid())
		m_vrs = sigStruct;
}

void TransactionBase::streamRLP(RLPStream& _s, IncludeSignature _sig, bool _forEip155hash) const
{
	if (m_type == NullTransaction)
		return;

	_s.appendList((_sig || _forEip155hash ? 3 : 0) + 7);
	_s << m_randomid << m_gasPrice << m_gas << m_blockLimit ; //这里加入新字段
	if (m_receiveAddress==Address())
	{
		_s << "";
	}
	else
	{
		_s << m_receiveAddress;
	}

	_s << m_value;
	_s << m_data;



	if (_sig)
	{
		int vOffset = m_chainId * 2 + 35;
		_s << (m_vrs.v + vOffset) << (u256)m_vrs.r << (u256)m_vrs.s;
	}
	else if (_forEip155hash)
		_s << m_chainId << 0 << 0;
}

void TransactionBase::streamRLP(std::stringstream& _s, IncludeSignature _sig, bool _forEip155hash) const {
	if (m_type == NullTransaction)
		return;

	_s << m_randomid << m_gasPrice << m_gas << m_blockLimit; //这里加入新字段
	if (m_receiveAddress == Address())
	{
		_s << "";
	}
	else
	{
		_s << m_receiveAddress;
	}

	_s << m_value;
	_s << toHex(m_data);

	if (_sig)
	{
		int vOffset = m_chainId * 2 + 35;
		_s << (m_vrs.v + vOffset) << (u256)m_vrs.r << (u256)m_vrs.s;
	}
	else if (_forEip155hash)
		_s << m_chainId << 0 << 0;
}

static const u256 c_secp256k1n("115792089237316195423570985008687907852837564279074904382605163141518161494337");

void TransactionBase::checkLowS() const
{
	if (m_vrs.s > c_secp256k1n / 2)
		BOOST_THROW_EXCEPTION(InvalidSignature());
}

void TransactionBase::checkChainId(int chainId) const
{
	if (m_chainId != chainId && m_chainId != -4)
		BOOST_THROW_EXCEPTION(InvalidSignature());
}

bigint TransactionBase::gasRequired(bool _contractCreation, bytesConstRef _data, EVMSchedule const& _es, u256 const& _gas)
{
	bigint ret = (_contractCreation ? _es.txCreateGas : _es.txGas) + _gas;
	for (auto i : _data)
		ret += i ? _es.txDataNonZeroGas : _es.txDataZeroGas;
	return ret;
}

h256 TransactionBase::sha3(IncludeSignature _sig) const
{
	if (_sig == WithSignature && m_hashWith)
		return m_hashWith;

	RLPStream s;
	//std::stringstream s;
	streamRLP(s, _sig, m_chainId > 0 && _sig == WithoutSignature);

	auto ret = dev::sha3(s.out());
	//auto ret = dev::sha3(s.str());
	if (_sig == WithSignature)
		m_hashWith = ret;
	return ret;
}

