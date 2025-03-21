#pragma once

#include "Singleton.h"

class CDnGameWeaponMng : public CSingleton<CDnGameWeaponMng>
{
public:
	struct SWeaponInfo
	{
		std::string szActionFile;
		int nWeapon1;
		int nWeapon2;
	};

private:
	std::vector<SWeaponInfo> m_vecWeaponInfo;
	int m_nCurrentInfoIndex;
	std::string m_szCurrentActionFileName;

	EtAniObjectHandle m_hWeaponObject1;
	EtAniObjectHandle m_hWeaponObject2;

	void AttachWeapon( int nWeapon1, int nWeapon2 );

public:
	CDnGameWeaponMng(void);
	virtual ~CDnGameWeaponMng(void);

	void OnLoadAction( CString szActionFileName );
	void GetAttachedWeapon( int &nWeapon1, int &nWeapon2 );
	void ChangeAttachWeapon( int nWeapon1, int nWeapon2 );
};