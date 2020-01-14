
#pragma once
#include <libcvcore/KeyManager.h>


class AccountManager
{
public:
	bool execute(int argc, char** argv);
	void static streamAccountHelp(std::ostream& _out);
	void static streamWalletHelp(std::ostream& _out);

private:
	std::string createPassword(std::string const& _prompt) const;
	dev::KeyPair makeKey() const;
	bool openWallet();

	std::unique_ptr<dev::eth::KeyManager> m_keyManager;
};


