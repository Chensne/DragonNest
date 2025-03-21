#include "stdafx.h"
#include "properties.h"
#include <fstream>

////////////////////////////////////////////////////////////////

CProperties::CProperties()
{
	pProperties = new PropertiesMap();
}

CProperties::~CProperties()
{
	delete pProperties;
}

CProperties::CProperties(const CProperties & rhs)
{
	pProperties = new PropertiesMap();
	*this->pProperties = *rhs.pProperties;
}

CProperties & CProperties::operator = (const CProperties & rhs)
{
	if(this != &rhs)
	{
		delete pProperties;
		pProperties = new PropertiesMap();
		*this->pProperties = *rhs.pProperties;
	}
	return *this;
}

void CProperties::AddProperties(const CProperties * pAnother)
{
	for(PropertiesMapItr itr = pProperties->begin(); itr != pProperties->end(); ++itr)
	{
		this->AddProperty(itr->first, itr->second);
	}
}

int CProperties::ImportProperties(const char * pFilePath)
{
	if(NULL == pFilePath)
	{
		return -1;
	}
	std::locale locl = std::locale::global(std::locale(".936"));
	std::ifstream ifs(pFilePath);
	if(!ifs)
	{
		return -2;
	}
	std::string s;
	int count = 0;
	while(std::getline(ifs, s))
	{
		if('#' == s[0])
		{
			continue;
		}
		if('/' == s[0] && '/' == s[1])
		{
			continue;
		}
		std::string::size_type key_end_pos = s.find("=");
		if(std::string::npos == key_end_pos)
		{
			continue;
		}
		std::string key = s.substr(0, key_end_pos);
		std::string value = s.substr(key_end_pos + 1);
		if(0 == value.length())
		{
			continue;
		}
		this->AddProperty(key, value);
		count++;
	}
	std::locale::global(std::locale(locl));
	return count;
}

bool CProperties::ExportProperties(const char * pFilePath)
{
	if(NULL == pFilePath)
	{
		return false;
	}
	std::locale locl = std::locale::global(std::locale(".936"));
	std::ofstream ofs(pFilePath);
	if(!ofs)
	{
		return false;
	}
	ofs << this->toString() << std::endl;
	ofs.flush();
	std::locale::global(std::locale(locl));
	return true;
}

void CProperties::AddProperty(const std::string & Key, const std::string & Value)
{
	this->pProperties->insert(std::make_pair(Key, Value));
}

bool CProperties::RemoveProperty(const std::string & Key)
{
	PropertiesMapItr itr = this->pProperties->find(Key);
	if(itr == this->pProperties->end())
	{
		return false;
	}
	this->pProperties->erase(itr);
	return true;
}

char * CProperties::GetProperty(const std::string & Key, char * pValueBuffer, unsigned int Length)
{
	PropertiesMapItr itr = this->pProperties->find(Key);
	if(itr == this->pProperties->end())
	{
		return NULL;
	}
	if(itr->second.length() + 1 > Length)
	{
		return NULL;
	}
	strncpy(pValueBuffer, itr->second.c_str(), Length);
	return pValueBuffer;
}

//template <typename T>
//T CProperties::GetProperty(const std::string & Key, const T & Def)
//{
//	std::string value = _GetProperty();
//	return value.empty() ? Def : ConvertTraits<T>(value);
//}

int CProperties::Size()
{
	return pProperties->size();
}

std::string CProperties::toString()
{
	std::string toString("");
	PropertiesMapItr itr = this->pProperties->begin();
	for(; itr != this->pProperties->end(); ++itr)
	{
		toString.append(itr->first);
		toString.append("=");
		toString.append(itr->second);
#if (defined WIN32 || defined WIN64)
		toString.append("\n");
#else
		toString.append("\r\n");
#endif
	}
	return toString;
}

std::string CProperties::_GetProperty(const std::string & Key)
{
	PropertiesMapItr itr = this->pProperties->find(Key);
	if(itr != this->pProperties->end())
	{
		return itr->second;
	}
	return "";
}
