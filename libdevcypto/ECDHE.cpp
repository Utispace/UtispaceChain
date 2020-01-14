

#include "ECDHE.h"
#include <libdevcore/SHA3.h>
#include "CryptoPP.h"

using namespace std;
using namespace dev;
using namespace dev::crypto;

void dev::crypto::ecdh::agree(Secret const& _s, Public const& _r, Secret& o_s)
{
	Secp256k1PP::get()->agree(_s, _r, o_s);
}

void ECDHE::agree(Public const& _remote, Secret& o_sharedSecret) const
{
	if (m_remoteEphemeral)
		// agreement can only occur once
		BOOST_THROW_EXCEPTION(InvalidState());
	
	m_remoteEphemeral = _remote;
	Secp256k1PP::get()->agree(m_ephemeral.secret(), m_remoteEphemeral, o_sharedSecret);
}

