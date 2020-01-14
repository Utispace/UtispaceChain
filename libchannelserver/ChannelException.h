
#pragma once

#include <string>

namespace dev
{

namespace channel {

class ChannelException: public std::exception {
public:
	ChannelException() {};
	ChannelException(int errorCode, const std::string &msg): _errorCode(errorCode), _msg(msg) {};

	virtual int errorCode() { return _errorCode; };
	virtual const char* what() const _GLIBCXX_USE_NOEXCEPT override { return _msg.c_str(); };

	bool operator!() const {
		return _errorCode == 0;
	}

private:
	int _errorCode = 0;
	std::string _msg = "";
};

}

}
