#pragma once
#include "EtUIDialog.h"

#if defined(PRE_ADD_DWC)
class CDnDwcAddMemberDlg;

class CDnDwcDlg : public CEtUIDialog, public CEtUICallback
{
public:
	CDnDwcDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL, bool bAutoCursor = false );
	virtual ~CDnDwcDlg();

protected:
	enum{
		E_CALLBACK_LEAVE_TEAM,		
	};

	CEtUIListBoxEx*  m_pListBoxEX;
	CEtUIButton*	 m_pHelpButton;
	CEtUIButton*	 m_pInviteButton;	// �� �ʴ�
	CEtUIButton*	 m_pDIsBandButton;	// �� ��ü

	CEtUIStatic*	 m_pStaticTeamName;	
	CEtUIStatic*	 m_pStaticPvPRank;
	CEtUIStatic*	 m_pStaticResult;
	CEtUIStatic*	 m_pStaticScore;

	CDnDwcAddMemberDlg*			m_pInviteTeamDlg;
	std::vector<TDWCTeam>		m_vTeamInfomationList;
	std::vector<TDWCTeamMember> m_vMemberInfomationList;

public:
	enum eRefreshType {
		E_REFRESH_TEAM_DATA,
		E_REFRESH_MEMBER_DATA,
		E_REFRESH_ALL_DATA,
	};

	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Show( bool bShow );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );
	
	void Reset();
	void RefreshAllData(eRefreshType eType = E_REFRESH_ALL_DATA);
	void RefreshUI();
	void RefreshTeamInfo();
	void RefreshTeamMemberInfo();
	
	void ClearDataVectors() { m_vMemberInfomationList.clear(); m_vTeamInfomationList.clear(); }
	void AllDisableUI();
};

#endif // PRE_ADD_DWC