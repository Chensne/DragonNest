#pragma once

#include "EtUIDialog.h"

#include "DnNoticeMarkHandler.h"

class CDnChannelMoveDlg : public CEtUIDialog
{
public:
	CDnChannelMoveDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnChannelMoveDlg(void);

	struct SMarkInfo
	{
		CEtUITextureControl * m_pUnionMark;
		BYTE m_cMarkType;
	};

protected:
	CEtUIStatic *m_pStaticMapName;
	CEtUIStatic *m_pStaticDungeon;
	CEtUIStatic *m_pStaticVillage;
	CEtUIStatic *m_pStaticWorld;
	CEtUIStatic *m_pStaticMeritMark;
	CEtUITextureControl*		m_pMarks[_MAX_MARK_COUNT];
	CEtUITextureControl*		m_pMarksNoChannel[_MAX_MARK_COUNT];
	EtTextureHandle				m_hMarkTexture;

	CDnNoticeMarkHandler		m_NoticeMarkHandler;

	CEtUIStatic *m_pStaticChannel;
	CEtUIButton *m_pButtonChannel;

	std::wstring m_ChannelMeritDesc;

public:
	void RefreshMapInfo();
	void SetChannelID( int nChannelID );

	bool OnMouseOver(CEtUITextureControl** ppCtrl, float fMouseX, float fMouseY);
	void UpdateMarks(float fElapsedTime);
	void SetUnionMarks(std::vector<int>& unionMarkTableIds);

	void SetGuildWarMark(bool bShow, bool bNew);

	void SetGuildRewardMark(bool bShow, bool bNew);
	bool IsMarkGroupTypeInInfoList(eMarkGroupType type) { return m_NoticeMarkHandler.IsMarkGroupTypeInInfoList(type); }

#if defined(PRE_ADD_DWC)
	void SetDWCMark(bool bShow, bool bNew);
#endif

#if defined(PRE_ADD_WEEKLYEVENT)
	void SetWeeklyEventMark(bool bShow, bool bNew);
#endif

#if defined(PRE_ADD_ACTIVEMISSION)
	void SetActiveMissionEventMark( int acMissionID, std::wstring & str, bool bShow, bool bNew);
#endif // PRE_ADD_ACTIVEMISSION

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Process( float fElapsedTime );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );

protected:
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
};