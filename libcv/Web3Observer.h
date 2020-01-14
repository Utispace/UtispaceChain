

#pragma once

namespace dev {

namespace eth {

class Web3Observer : public std::enable_shared_from_this<Web3Observer> {
public:
	typedef std::shared_ptr<Web3Observer> Ptr;

	Web3Observer() {};
	virtual ~Web3Observer() {};

	virtual void onReceiveChannelMessage(const h512, std::shared_ptr<bytes>) = 0; //收到链上链下消息
};

}

}
