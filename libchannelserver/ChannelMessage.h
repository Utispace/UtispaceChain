
#pragma once

#include <string>
#include <thread>
#include <queue>
#include <libdevcore/FixedHash.h>
#include <arpa/inet.h>

namespace dev
{

namespace channel {

struct Message {
	typedef std::shared_ptr<Message> Ptr;
	const static size_t HEADER_LENGTH = 4 + 2 + 32 + 4;
	const static size_t MAX_LENGTH = 1024 * 1024 * 1024;

	uint32_t length = 0; 
	uint16_t type = 0;
	std::string seq = ""; //32字节
	int result = 0;

	std::shared_ptr<bytes> data;

	Message() {
		data = std::make_shared<bytes>();
	}

	void encode(bytes &buffer) {
		uint32_t lengthN = htonl(HEADER_LENGTH + data->size());
		uint16_t typeN = htons(type);
		int32_t resultN = htonl(result);

		buffer.insert(buffer.end(), (byte*) &lengthN,	(byte*) &lengthN + sizeof(lengthN));
		buffer.insert(buffer.end(), (byte*) &typeN, (byte*) &typeN + sizeof(typeN));
		buffer.insert(buffer.end(), seq.data(), seq.data() + seq.size());
		buffer.insert(buffer.end(), (byte*) &resultN,	(byte*) &resultN + sizeof(resultN));

		buffer.insert(buffer.end(), data->begin(), data->end());
	}

	ssize_t decode(const byte* buffer, size_t size) {
		if(size < HEADER_LENGTH) {
			return 0;
		}

		length = ntohl(*((uint32_t*)&buffer[0]));

		if(length > MAX_LENGTH) {
			return -1;
		}

		if(size < length) {
			return 0;
		}

		type = ntohs(*((uint16_t*)&buffer[4]));
		seq = ntohl(*((uint32_t*)&buffer[6]));
		seq.assign(&buffer[6], &buffer[6] + 32);
		result = ntohl(*((uint32_t*)&buffer[38]));

		data->assign(&buffer[HEADER_LENGTH], &buffer[HEADER_LENGTH] + length - HEADER_LENGTH);

		return length;
	}
};

}

}
