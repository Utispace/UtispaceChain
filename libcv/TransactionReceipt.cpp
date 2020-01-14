

#include "TransactionReceipt.h"

using namespace std;
using namespace dev;
using namespace dev::eth;

TransactionReceipt::TransactionReceipt(bytesConstRef _rlp)
{
	RLP r(_rlp);
	m_stateRoot = (h256)r[0];
	m_gasUsed = (u256)r[1];
	m_contractAddress=(Address)r[2];//新加的
	m_bloom = (LogBloom)r[3];
	for (auto const& i: r[4])
		m_log.emplace_back(i);
}

TransactionReceipt::TransactionReceipt(h256 _root, u256 _gasUsed, LogEntries const& _log, Address const& _contractAddress)
{

	m_stateRoot=(_root);
	m_gasUsed=(_gasUsed);
	m_bloom=(eth::bloom(_log));
	m_contractAddress=(_contractAddress);
	m_log=(_log);
}

void TransactionReceipt::streamRLP(RLPStream& _s) const
{
	_s.appendList(5) << m_stateRoot << m_gasUsed << m_contractAddress<< m_bloom;
	_s.appendList(m_log.size());
	for (LogEntry const& l: m_log)
		l.streamRLP(_s);
}

std::ostream& dev::eth::operator<<(std::ostream& _out, TransactionReceipt const& _r)
{
	_out << "Root: " << _r.stateRoot() << "\n";
	_out << "Gas used: " << _r.gasUsed() << "\n";
	_out << "contractAddress : " << _r.contractAddress() << "\n";
	_out << "Logs: " << _r.log().size() << " entries:" << "\n";
	for (LogEntry const& i: _r.log())
	{
		_out << "Address " << i.address << ". Topics:" << "\n";
		for (auto const& j: i.topics)
			_out << "  " << j << "\n";
		_out << "  Data: " << toHex(i.data) << "\n";
	}
	_out << "Bloom: " << _r.bloom() << "\n";
	return _out;
}
