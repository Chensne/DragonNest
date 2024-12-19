#pragma once
#include "DnCustomDlg.h"
#include "DnJobIconStatic.h"


class CDnPVPInfoDlg : public CDnCustomDlg
{
	enum eInfoValue
	{
		PVPMedalTableID = 374341682, // 메달의 종류가 변동이 생기면 테이블로 빼야하겟지만 , 앞으로 변동은 없을거라고 (기획자분 말씀) 해서 따로 빼지 않고 사용하겠습니다.
		PVPWinUIString = 126039,
	};
public:
	CDnPVPInfoDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnPVPInfoDlg(void);

protected:

	CDnJobIconStatic *pUIJobIcon; // 직업이름 
	CEtUIStatic *pUIGuildName; // 길드이름
	CEtUIStatic *pUILevel;     // 레벨
	CEtUIStatic *pUIName;      // 아이디
	CEtUITextureControl *pUIRankIcon;  // 계급 아이콘 
	CEtUIStatic *pUIRankName;  // 계급 이름
	CEtUIStatic *pUIXP;        // 현재 콜로세움 XP
	CEtUIStatic *pUINeedXP;    // 다음 레벨에 필요로하는 XP
	CEtUIStatic *pUIScore;
	CEtUIStatic *pUIMedal;

#ifdef PRE_ADD_DWC
	bool m_bIsDWCUser;
#endif
	
public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Process( float fElapsedTime );

	void SetPVPInfo();

#ifdef PRE_ADD_DWC
	void SetDWCInfo();
#endif
};
