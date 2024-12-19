#pragma once

#include "Singleton.h"

struct SProfile
{
	std::string szProfileName;
	std::string szP4Url;
	std::string szP4UrlFolder;
	std::string szP4UrlServer;
	std::string szP4UrlServerFolder;
	std::string szP4Workspace;
	std::string szPatchFolder;
	std::string szRMakeCmd;
	std::string szManualPatchExe;
	std::string szCountryCode;
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
