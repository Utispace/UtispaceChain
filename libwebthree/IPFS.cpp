
#include "IPFS.h"
#include <libdevcore/Hash.h>
#include <libdevcore/Base58.h>
using namespace std;
using namespace dev;
#include "libexecstream/exec-stream.h"

static bytes exec(string const& _args)
{
	string output;
	try
	{
		exec_stream_t es("ipfs", _args);
		do
		{
			string s;
			getline(es.out(), s);
			output += s;
		} while(es.out().good());
	}
	catch (exception const &e)
	{
		throw IPFSCommunicationError(e.what());
	}
	return bytes(output.begin(), output.end());
}
static void exec(string const& _args, bytesConstRef _in)
{
	try
	{
		exec_stream_t es("ipfs", _args);
		es.in() << string(_in.begin(), _in.end());
	}
	catch (exception const &e)
	{
		throw IPFSCommunicationError(e.what());
	}
}

h256 IPFS::putBlockForSHA256(bytesConstRef _data)
{
	exec("block put", _data);
	return sha256(_data);
}

bytes IPFS::putBlock(bytesConstRef _data)
{
	return sha256AsMultihash(putBlockForSHA256(_data));
}

bytes IPFS::getBlockForSHA256(h256 const& _sha256)
{
	auto b = sha256AsMultihash(_sha256);
	return getBlock(&b);
}

bytes IPFS::getBlock(bytesConstRef _multihash)
{
	return exec("block get " + toBase58(_multihash));
}
