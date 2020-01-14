

#pragma once
#include "Common.h"
#include "string"
using namespace std;
class ParseCert
{
public:
	ParseCert(void);
	~ParseCert(void);
public:
	void ParseInfo(ba::ssl::verify_context& ctx);
	bool getExpire();
	string getSerialNumber();
	string getSubjectName();
	int getCertType();
private:
	int mypint( const char ** s,int n,int min,int max,int * e);
	time_t ASN1_TIME_get ( ASN1_TIME * a,int *err);
private:
	bool m_isExpire;
	string m_serialNumber;
	int m_certType;
	string m_subjectName;
};

