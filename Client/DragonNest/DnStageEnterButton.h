#pragma once

#include "DnWorld.h"

class CDnStageEnterButton : public CEtUIRadioButton
{
public:
	CDnStageEnterButton( CEtUIDialog *pParent );
	virtual ~CDnStageEnterButton(void);

protected:
	std::wstring m_strDungeonName;
	std::wstring m_strPartyMaxCount;
	std::wstring m_strDungeonDesc;
	int m_nMapType;
	int m_nMinLevel;
	int m_nMaxLevel;
	int m_nMapIndex;
	char m_cPermit;

	SUICoord m_BaseCoordLeft;
	SUICoord m_BaseCoordMid;
	SUICoord m_BaseCoordRight;

	SUICoord m_uvCoordLeft;
	SUICoord m_uvCoordMid;
	SUICoord m_uvCoordRight;
	bool m_bDarklairEnterButton;
	bool m_bMissionAchieve;

public:
	void Clear();
	void MakeStringByPermit(std::wstring& str);
	const WCHAR* GetDayOfWeekString(int nDay);
	void SetInfo( const wchar_t *wszDungeonName, const wchar_t *wszDungeonDesc, int nMinPartyCount, int nMaxPartyCount, int nMapType, int nMinLevel, int nMaxLevel, int nMapIndex = 0, char cPermit = CDnWorld::PermitEnter );

	const wchar_t *GetDungeonName() { return m_strDungeonName.c_str(); }
	const wchar_t *GetDungeonDesc() { return m_strDungeonDesc.c_str(); }

	void SetDarklairEnterButton() { m_bDarklairEnterButton = true; }
	void SetAchieve( bool bAchieve ) { m_bMissionAchieve = bAchieve; }

protected:
	virtual bool HandleMouse( UINT uMsg, float fX, float fY, WPARAM wParam, LPARAM lParam );

public:
	virtual void Render( float fElapsedTime );
};
