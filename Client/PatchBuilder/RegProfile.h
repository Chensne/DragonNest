#pragma once

#include "Singleton.h"

struct SProfile
{
	std::string szProfileName;
	std::string szSVNUrl;
	std::string szSVNUpdateFolder;
	std::string szPatchFolder;
	std::string szRMakeCmd;
	std::string szManualPatchExe;
	std::string szCountryCode;
	std::string szSVNUrlServer;
};

class CRegProfile : public CSingleton< CRegProfile >
{
public:
	CRegProfile(void);
	virtual ~CRegProfile(void);

protected:
	std::vector< SProfile > m_vecProfile;

public:
	int GetProfileCount() { return ( int )m_vecProfile.size(); }
	void LoadProfile();
	void SaveProfile();
	void AddProfile( SProfile &AddProfile );
	void DeleteProfile( int nIndex );

	SProfile *GetProfile( int nIndex );

};
