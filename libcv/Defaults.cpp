

#include "Defaults.h"

#include <libdevcore/FileSystem.h>
using namespace std;
using namespace dev;
using namespace dev::eth;

Defaults* Defaults::s_this = nullptr;

Defaults::Defaults()
{
	m_dbPath = getDataDir();
}
