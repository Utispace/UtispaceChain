

#include "CVSessionData.h"

using namespace dev::p2p;

std::string CVSessionData::getPub256()
{
	return m_pub256;
}

void CVSessionData::setPub256(std::string pub256)
{
	m_pub256 = pub256;
}

Signature CVSessionData::getNodeSign()
{
	return m_nodeSign;
}

void CVSessionData::setNodeSign(const Signature& _nodeSign)
{
	m_nodeSign = _nodeSign;
}
