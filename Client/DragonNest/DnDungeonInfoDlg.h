#pragma once
#include "EtUIDialog.h"
#include "DnNoticeMarkHandler.h"

class CDnLatencyDlg;

// 채널무브랑 관련이 많아서 옆에 두기로 한다.
class CDnDungeonInfoDlg : public CEtUIDialog
{
public:
	CDnDungeonInfoDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnDungeonInfoDlg(void);

protected:
	CDnLatencyDlg *m_pLatency;
	CEtUIStatic *m_pStaticMapName;
	CEtUIStatic *m_pStaticDiff;
	SUICoord m_uiDefaultCoord[3];
	CEtUITextureControl*		m_pMarks0[_MAX_MARK_COUNT];
	CEtUITextureControl*		m_pMarks1[_MAX_MARK_COUNT];
	SUICoord					m_pMarkCoord0[_MAX_MARK_COUNT];
	SUICoord					m_pMarkCoord1[_MAX_MARK_COUNT];
	EtTextureHandle				m_hMarkTexture;
	CDnNoticeMarkHandler		m_NoticeMarkHandler;

	// 보스게이지 뜰때 이동할 임시 위치 컨트롤
	CEtUIStatic *m_pStaticMovePosition;
	SUICoord m_uiMovePositionOffset;
	bool m_bMoveOffset;

public:
	void RefreshInfo();
	void MoveOffset( bool bMove );
	void SetUnionMarks(std::vector<int>& unionMarkTableIds);
	void SetGuildWarMark(bool bShow, bool bNew);

	bool OnMouseOver(CEtUITextureControl** ppCtrl, float fMouseX, float fMouseY);
	void UpdateMarks(float fElapsedTime);

	void SetGuildRewardMark(bool bShow, bool bNew);
	bool IsMarkGroupTypeInInfoList(eMarkGroupType type) { return m_NoticeMarkHandler.IsMarkGroupTypeInInfoList(type); }

#if defined(PRE_ADD_WEEKLYEVENT)
	void SetWeeklyEventMark(bool bShow, bool bNew);
#endif

#if defined(PRE_ADD_ACTIVEMISSION)
	void SetActiveMissionEventMark( int acMissionID, std::wstring & str, bool bShow, bool bNew);
	void GetActiveMarkPos( float & fx, float & fy );
#endif // PRE_ADD_ACTIVEMISSION

	void SetLatency(int Latency);
public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Process(  float fElapsedTime );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
};