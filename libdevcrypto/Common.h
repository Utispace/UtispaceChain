
#pragma once

#include <mutex>
#include <libdevcore/Common.h>
#include <libdevcore/FixedHash.h>
#include <libdevcore/Exceptions.h>

namespace dev
{

using Secret = SecureFixedHash<32>;

using Public = h512;

using Signature = h520;

struct SignatureStruct
{
	SignatureStruct() = default;
	SignatureStruct(Signature const& _s) { *(h520*)this = _s; }
	SignatureStruct(h256 const& _r, h256 const& _s, byte _v): r(_r), s(_s), v(_v) {}
	operator Signature() const { return *(h520 const*)this; }

	bool isValid() const noexcept;

	Public recover(h256 const& _hash) const;

	h256 r;
	h256 s;
	byte v = 0;
};

/// An Ethereum address: 20 bytes.
/// @NOTE This is not endian-specific; it's just a bunch of bytes.
using Address = h160;

/// The zero address.
extern Address ZeroAddress;

/// A vector of Ethereum addresses.
using Addresses = h160s;

/// A hash set of Ethereum addresses.
using AddressHash = std::unordered_set<h160>;

/// A vector of secrets.
using Secrets = std::vector<Secret>;

/// Convert a secret key into the public key equivalent.
Public toPublic(Secret const& _secret);

/// Convert a public key to address.
Address toAddress(Public const& _public);

/// Convert a secret key into address of public key equivalent.
/// @returns 0 if it's not a valid secret key.
Address toAddress(Secret const& _secret);

// Convert transaction from and nonce to address.
Address toAddress(Address const& _from, u256 const& _nonce);

/// Encrypts plain text using Public key.
void encrypt(Public const& _k, bytesConstRef _plain, bytes& o_cipher);

/// Decrypts cipher using Secret key.
bool decrypt(Secret const& _k, bytesConstRef _cipher, bytes& o_plaintext);

/// Symmetric encryption.
void encryptSym(Secret const& _k, bytesConstRef _plain, bytes& o_cipher);

/// Symmetric decryption.
bool decryptSym(Secret const& _k, bytesConstRef _cipher, bytes& o_plaintext);

/// Encrypt payload using ECIES standard with AES128-CTR.
void encryptECIES(Public const& _k, bytesConstRef _plain, bytes& o_cipher);

/// Encrypt payload using ECIES standard with AES128-CTR.
/// @a _sharedMacData is shared authenticated data.
void encryptECIES(Public const& _k, bytesConstRef _sharedMacData, bytesConstRef _plain, bytes& o_cipher);

/// Decrypt payload using ECIES standard with AES128-CTR.
bool decryptECIES(Secret const& _k, bytesConstRef _cipher, bytes& o_plaintext);

/// Decrypt payload using ECIES standard with AES128-CTR.
/// @a _sharedMacData is shared authenticated data.
bool decryptECIES(Secret const& _k, bytesConstRef _sharedMacData, bytesConstRef _cipher, bytes& o_plaintext);

/// Encrypts payload with random IV/ctr using AES128-CTR.
std::pair<bytes, h128> encryptSymNoAuth(SecureFixedHash<16> const& _k, bytesConstRef _plain);

/// Encrypts payload with specified IV/ctr using AES128-CTR.
bytes encryptAES128CTR(bytesConstRef _k, h128 const& _iv, bytesConstRef _plain);

/// Decrypts payload with specified IV/ctr using AES128-CTR.
bytesSec decryptAES128CTR(bytesConstRef _k, h128 const& _iv, bytesConstRef _cipher);

/// Encrypts payload with specified IV/ctr using AES128-CTR.
inline bytes encryptSymNoAuth(SecureFixedHash<16> const& _k, h128 const& _iv, bytesConstRef _plain) { return encryptAES128CTR(_k.ref(), _iv, _plain); }
inline bytes encryptSymNoAuth(SecureFixedHash<32> const& _k, h128 const& _iv, bytesConstRef _plain) { return encryptAES128CTR(_k.ref(), _iv, _plain); }

/// Decrypts payload with specified IV/ctr using AES128-CTR.
inline bytesSec decryptSymNoAuth(SecureFixedHash<16> const& _k, h128 const& _iv, bytesConstRef _cipher) { return decryptAES128CTR(_k.ref(), _iv, _cipher); }
inline bytesSec decryptSymNoAuth(SecureFixedHash<32> const& _k, h128 const& _iv, bytesConstRef _cipher) { return decryptAES128CTR(_k.ref(), _iv, _cipher); }

/// Recovers Public key from signed message hash.
Public recover(Signature const& _sig, h256 const& _hash);

/// Returns siganture of message hash.
Signature sign(Secret const& _k, h256 const& _hash);

/// Verify signature.
bool verify(Public const& _k, Signature const& _s, h256 const& _hash);

/// Derive key via PBKDF2.
bytesSec pbkdf2(std::string const& _pass, bytes const& _salt, unsigned _iterations, unsigned _dkLen = 32);

/// Derive key via Scrypt.
bytesSec scrypt(std::string const& _pass, bytes const& _salt, uint64_t _n, uint32_t _r, uint32_t _p, unsigned _dkLen);

/// Simple class that represents a "key pair".
/// All of the data of the class can be regenerated from the secret key (m_secret) alone.
/// Actually stores a tuplet of secret, public and address (the right 160-bits of the public).
class KeyPair
{
public:
	/// Null constructor.
	KeyPair() = default;

	/// Normal constructor - populates object from the given secret key.
	/// If the secret key is invalid the constructor succeeds, but public key
	/// and address stay "null".
	KeyPair(Secret const& _sec);

	/// Create a new, randomly generated object.
	static KeyPair create();

	/// Create from an encrypted seed.
	static KeyPair fromEncryptedSeed(bytesConstRef _seed, std::string const& _password);

	Secret const& secret() const { return m_secret; }

	Secret const& sec() const { return m_secret; }
	/// Retrieve the public key.
	Public const& pub() const { return m_public; }

	/// Retrieve the associated address of the public key.
	Address const& address() const { return m_address; }

	bool operator==(KeyPair const& _c) const { return m_public == _c.m_public; }
	bool operator!=(KeyPair const& _c) const { return m_public != _c.m_public; }

private:
	Secret m_secret;
	Public m_public;
	Address m_address;
};

namespace crypto
{

DEV_SIMPLE_EXCEPTION(InvalidState);

/// Key derivation
h256 kdf(Secret const& _priv, h256 const& _hash);


class Nonce
{
public:
	/// Returns the next nonce (might be read from a file).
	static Secret get() { static Nonce s; return s.next(); }

private:
	Nonce() = default;

	/// @returns the next nonce.
	Secret next();

	std::mutex x_value;
	Secret m_value;
};

}

}
