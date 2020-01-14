
#pragma once

#include <string>
#include "CommonIO.h"

namespace dev
{
void setCryptoMod(int _datacryptoMod);
int getCryptoMod();

void setCryptoPrivateKeyMod(int cryptoprivatekeyMod);
int getCryptoPrivateKeyMod();

void setPrivateKey(std::string const& privateKey);
std::string	getPrivateKey();

void setSSL(int ssl);
int getSSL();

std::map<int,std::string> getDataKey();
void setDataKey(std::string const& _dataKey1,std::string const& _dataKey2,std::string const& _dataKey3,std::string const& _dataKey4);
/// Sets the data dir for the default ("ethereum") prefix.
void setDataDir(std::string const& _dir);
/// @returns the path for user data.
std::string getDataDir(std::string _prefix = "ethereum");
/// @returns the default path for user data, ignoring the one set by `setDataDir`.
std::string getDefaultDataDir(std::string _prefix = "ethereum");
/// Sets the ipc socket dir
void setIpcPath(std::string const& _ipcPath);
/// @returns the ipc path (default is DataDir)
std::string getIpcPath();

void setConfigPath(std::string const& _configPath);

std::string getConfigPath();

void setCaInitType(std::string const& _caInitType = "cybervein");

std::string getCaInitType();
}
