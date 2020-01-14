#include "SessionData.h"

using namespace dev::p2p;

void SessionBaseData::setSign(std::string sign)
{
	m_sign = sign;
}
std::string SessionBaseData::getSign()
{
	return m_sign;
}

void SessionBaseData::setSeed(std::string seed)
{
	m_seed = seed;
}

std::string SessionBaseData::getSeed()
{
	return m_seed;
}

std::string SessionBaseData::getPub256()
{
	return "";
}

void SessionBaseData::setPub256(std::string)
{

}

Signature SessionBaseData::getNodeSign()
{
	Signature signature;
	return signature;
}

void SessionBaseData::setNodeSign(const Signature&)
{

}
