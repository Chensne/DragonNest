#pragma once

#include "DnCustomDlg.h"
#include "DnActor.h"
#include "DnPartyTask.h"

class CDnBuffUIMng;
class CDnGuildWarSituationMng;

class CDnMiniPlayerGuildWarGaugeDlg : public CDnCustomDlg
{
public:
	CDnMiniPlayerGuildWarGaugeDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnMiniPlayerGuildWarGaugeDlg();

	enum eIconIndex
	{
		eBuff_Icon1 = 0,
		eBuff_Icon2,
		eBuff_Icon3,
		eBuff_Icon4,
		eBuff_Icon5,
		eBuff_Icon6,
		eBuff_Icon7,

		eMax_Icon,
	};

protected:

	DnActorHandle m_hActor;

	CEtUIStatic * m_pStaticBase;
	CEtUIStatic * m_pStaticName;
	CEtUIStatic * m_pStaticRedMaster;
	CEtUIStatic * m_pStaticBlueMaster;
	CEtUIStatic * m_pStaticSubMaster;
	CEtUIStatic * m_pStaticSelectBar;

	CEtUIProgressBar * m_pHP;
	CEtUIProgressBar * m_pMP;

	CEtUITextureControl * m_pTextureRank;
	CDnJobIconStatic * m_pTextureClass;
	CEtUITextureControl * m_pBuffTextureCtrl[MAX_BUFF_SLOT];

	EtTextureHandle	m_hSkillIconList[BUFF_TEXTURE_COUNT];

	CDnBuffUIMng *	m_pBuffUIMng;
	int				m_pIconPosition[eMax_Icon];
	SUICoord		m_pIconCoord[eMax_Icon];

	CDnGuildWarSituationMng * m_pGuildWarSituationMng;

	USHORT m_cTeam;
	UINT m_uiUserState;
	bool m_bSelect;
	bool m_bSelectMode;
	bool m_bUser;

public:
	void SetSelectMode( bool bSelectMode );
	void Select( bool bSelect );
	bool IsSelect()	{ return m_pStaticSelectBar->IsShow(); }

	void RefreshIcon();
	void ProcessBuffs();
	void ProcessGauge();
	void SetUser( CDnPartyTask::PartyStruct * pStruct );
	void SetUserState( UINT uiUserState );
	void ClearUser();
	UINT GetUserState()	{ return m_uiUserState; }
	bool IsUser()	{ return m_bUser; }

	void SetGuildWarSituationMng( CDnGuildWarSituationMng * pGuildWarSituationMng );

	void SetPartyShow();

	DnActorHandle GetActor()	{ return m_hActor; }

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );
	virtual void Render( float fElapsedTime );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
};
