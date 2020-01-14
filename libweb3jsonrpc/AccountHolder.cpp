

#include "AccountHolder.h"
#include <random>
#include <libdevcore/Guards.h>
#include <libcv/Client.h>
#include <libcvcore/KeyManager.h>


using namespace std;
using namespace dev;
using namespace dev::eth;

vector<TransactionSkeleton> g_emptyQueue;
static std::mt19937 g_randomNumberGenerator(utcTime());
static Mutex x_rngMutex;

vector<Address> AccountHolder::allAccounts() const
{
	vector<Address> accounts;
	accounts += realAccounts();
	for (auto const& pair : m_proxyAccounts)
		if (!isRealAccount(pair.first))
			accounts.push_back(pair.first);
	return accounts;
}

Address const& AccountHolder::defaultTransactAccount() const
{
	auto accounts = realAccounts();
	if (accounts.empty())
		return ZeroAddress;
	Address const* bestMatch = &*accounts.begin();
	for (auto const& account : accounts)
		if (m_client()->balanceAt(account) > m_client()->balanceAt(*bestMatch))
			bestMatch = &account;
	return *bestMatch;
}

int AccountHolder::addProxyAccount(const Address& _account)
{
	Guard g(x_rngMutex);
	int id = std::uniform_int_distribution<int>(1)(g_randomNumberGenerator);
	id = int(u256(FixedHash<32>(sha3(bytesConstRef((byte*)(&id), sizeof(int) / sizeof(byte))))));
	if (isProxyAccount(_account) || id == 0 || m_transactionQueues.count(id))
		return 0;
	m_proxyAccounts.insert(make_pair(_account, id));
	m_transactionQueues[id].first = _account;
	return id;
}

bool AccountHolder::removeProxyAccount(unsigned _id)
{
	if (!m_transactionQueues.count(_id))
		return false;
	m_proxyAccounts.erase(m_transactionQueues[_id].first);
	m_transactionQueues.erase(_id);
	return true;
}

void AccountHolder::queueTransaction(TransactionSkeleton const& _transaction)
{
	if (!m_proxyAccounts.count(_transaction.from))
		return;
	int id = m_proxyAccounts[_transaction.from];
	m_transactionQueues[id].second.push_back(_transaction);
}

vector<TransactionSkeleton> const& AccountHolder::queuedTransactions(int _id) const
{
	if (!m_transactionQueues.count(_id))
		return g_emptyQueue;
	return m_transactionQueues.at(_id).second;
}

void AccountHolder::clearQueue(int _id)
{
	if (m_transactionQueues.count(_id))
		m_transactionQueues.at(_id).second.clear();
}

AddressHash SimpleAccountHolder::realAccounts() const
{
	return m_keyManager.accountsHash();
}

TransactionNotification SimpleAccountHolder::authenticate(dev::eth::TransactionSkeleton const& _t)
{
	TransactionNotification ret;
	bool locked = true;
	if (m_unlockedAccounts.count(_t.from))
	{
		chrono::steady_clock::time_point start = m_unlockedAccounts[_t.from].first;
		chrono::seconds duration(m_unlockedAccounts[_t.from].second);
		auto end = start + duration;
		if (start < end && chrono::steady_clock::now() < end)
			locked = false;
	}
	ret.r = TransactionRepercussion::Locked;
	if (locked && m_getAuthorisation)
	{
		if (m_getAuthorisation(_t, isProxyAccount(_t.from)))
			locked = false;
		else
			ret.r = TransactionRepercussion::Refused;
	}
	if (locked)
		return ret;
	if (isRealAccount(_t.from))
	{
		if (Secret s = m_keyManager.secret(_t.from, [&]() { return m_getPassword(_t.from); }))
		{

			//tie  解开tuple
			tie(ret.hash, ret.created) = m_client()->submitTransaction(_t, s);

			//这里也返回，加个nonce异常的返回错误
			if ( ret.created == Address(1) ) //nonceCheck
				ret.r = TransactionRepercussion::NonceError;
			else if ( ret.created == Address(2) ) //blocklimit 检查失败
				ret.r = TransactionRepercussion::BlockLimitError;
			else if ( ret.created == Address(3) ) //blocklimit 检查失败
				ret.r = TransactionRepercussion::TxPermissionError;
			else
				ret.r = TransactionRepercussion::Success;
		}
		else
			ret.r = TransactionRepercussion::Locked;
	}
	else if (isProxyAccount(_t.from))
	{
		ret.r = TransactionRepercussion::ProxySuccess;
		queueTransaction(_t);
	}
	else
		ret.r = TransactionRepercussion::UnknownAccount;
	return ret;
}

bool SimpleAccountHolder::unlockAccount(Address const& _account, string const& _password, unsigned _duration)
{
	if (!m_keyManager.hasAccount(_account))
		return false;

	if (_duration == 0)
		// Lock it even if the password is wrong.
		m_unlockedAccounts[_account].second = 0;

	m_keyManager.notePassword(_password);

	try
	{
		if (!m_keyManager.secret(_account, [&] { return _password; }, false))
			return false;
	}
	catch (PasswordUnknown const&)
	{
		return false;
	}
	m_unlockedAccounts[_account] = make_pair(chrono::steady_clock::now(), _duration);

	return true;
}

TransactionNotification FixedAccountHolder::authenticate(dev::eth::TransactionSkeleton const& _t)
{
	TransactionNotification ret;
	if (isRealAccount(_t.from))
	{
		if (m_accounts.count(_t.from))
		{
			tie(ret.hash, ret.created) = m_client()->submitTransaction(_t, m_accounts[_t.from]);

			if ( ret.created == Address(1) ) //nonceCheck
				ret.r = TransactionRepercussion::NonceError;
			else if ( ret.created == Address(2) ) //blocklimit 检查失败
				ret.r = TransactionRepercussion::BlockLimitError;
			else if ( ret.created == Address(3) ) //blocklimit 检查失败
				ret.r = TransactionRepercussion::TxPermissionError;
			else if ( ret.created == Address(4))
				ret.r = TransactionRepercussion::DeployPermissionError;
			else
				ret.r = TransactionRepercussion::Success;
		}
		else
			ret.r = TransactionRepercussion::Locked;
	}
	else if (isProxyAccount(_t.from))
	{
		ret.r = TransactionRepercussion::ProxySuccess;
		queueTransaction(_t);
	}
	else
		ret.r = TransactionRepercussion::UnknownAccount;
	return ret;
}



