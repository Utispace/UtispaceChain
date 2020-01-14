

#pragma once

#include "SessionData.h"

namespace dev {

	namespace p2p {

		class CVSessionData : public SessionBaseData
		{
		public:
			virtual std::string getPub256(); 
			virtual void setPub256(std::string pub256);
			Signature getNodeSign(); 
			void setNodeSign(const Signature& _nodeSign); 
		private:
			std::string m_pub256;           
			Signature m_nodeSign;    
		};
	}
}
