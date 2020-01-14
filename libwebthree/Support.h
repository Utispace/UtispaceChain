
#pragma once

#include <libcvcore/Common.h>
#include <libcvcore/ABI.h>
#include <libcv/Interface.h>

namespace dev
{

class WebThreeDirect;

/// Utilities supporting typical use of the Ethereum network.
class Support
{
public:
	Support(WebThreeDirect* _web3);
	~Support();

	// URI managment
	static strings decomposed(std::string const& _name);

	// Registrar
	Address registrar() const;
	h256 content(std::string const& _url) const { return lookup<h256>(decomposed(_url), "content"); }
	Address address(std::string const& _url) const { return lookup<Address>(decomposed(_url), "addr"); }
	Address subRegistrar(std::string const& _url) const { return lookup<Address>(decomposed(_url), "subRegistrar"); }
	Address owner(std::string const& _url) const { return lookup<Address>(decomposed(_url), "owner"); }

	// URL Hinter
	Address urlHint() const;
	std::string urlHint(h256 const& _content) const;
	void hintURL(h256 const& _content, std::string const& _url, Secret const& _s) const;

	// SHA256 Hinter
	Address sha256Hint() const;
	h256 sha256Hint(h256 const& _content) const;
	void hintSHA256(h256 const& _content, h256 const& _sha256, Secret const& _s) const;

	// ICAP
	Address icapRegistrar() const;
	std::pair<Address, bytes> decodeICAP(std::string const& _icap) const;

private:
	bytes call(Address const& _to, bytes const& _data) const;

	template <class T> T lookup(strings const& _path, std::string const& _query) const
	{
		return eth::abiOut<T>(auxLookup(_path, _query));
	}
	bytes auxLookup(strings const& _path, std::string const& _query) const;

	WebThreeDirect* m_web3;
};

}
