#include "StdAfx.h"
#include "Singleton.h"
#include "Settings.h"
#include <Windows.h>
#include <fstream>
#include "resource.h"
#include "Define.h"
#include <string>

#ifdef _SKY
Data data;
Settings settings;

Settings::~Settings()
{
	//destructor
}

Settings::Settings()
{
	//constructor
}

bool FileExists(std::string path)
{
	std::ifstream my_file(path.c_str()); //trick vs2005
	if (my_file)
	{
		return true;
	}
	return false;
}

void Settings::LoadSettings()
{
	if (!FileExists(STORE_ACCOUNT))
		return;

	std::fstream fh;
	fh.open(STORE_ACCOUNT, std::fstream::in | std::fstream::binary);
	fh.read((char*)&data.version, sizeof(data.version));
	fh.read((char*)&data.Username, sizeof(data.Username));
	fh.read((char*)&data.Password, sizeof(data.Password));
	fh.read((char*)&data.RememberMe, sizeof(data.RememberMe));
	fh.read((char*)&data.ShowNewUI, sizeof(data.ShowNewUI));
	if (data.version > 2)
		fh.read((char*)&data.RESERVED, sizeof(data.RESERVED));
	this->EncDecData((BYTE*)&data);
	fh.close();
}

void Settings::SaveSettings()
{
	//set Version!
	this->SetVersion(STORE_ACCOUNT_VERSION);

	std::fstream fh;
	fh.open(STORE_ACCOUNT, std::fstream::out | std::fstream::binary);
	this->EncDecData((BYTE*)&data);
	fh.write((char*)&data, sizeof(Data));
	fh.close();
}

void Settings::SetVersion(BYTE ver)
{
	data.version = ver;
}

void Settings::EncDecData(BYTE * ce)
{
	for (DWORD i = 0; i < sizeof(Data); i++)
	{
		ce[i] ^= XOR_KEY;
	}
}

#endif