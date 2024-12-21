#pragma once

#include "DnCustomDlg.h"
#include "DnBlindDlg.h"

class CDnGuildWarPreliminaryStatusDlg : public CDnCustomDlg, public CDnBlindCallBack
{
public:
	CDnGuildWarPreliminaryStatusDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnGuildWarPreliminaryStatusDlg();

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg );

public:
	void SetPresentCondition( int nBlueTeamSocre, int nRedTeamScore, int nMySocre, int nMyGuildScore );

protected:
	CEtUIStatic*	m_pStaticInterimResult[3];	// �߰���� (0:û���켼, 1:ȫ���켼, 2:����)
	CEtUIStatic*	m_pStaticBlueTeamScore;		// û�� ����
	CEtUIStatic*	m_pStaticRedTeamScore;		// ȫ�� ����
	CEtUIStatic*	m_pStaticMyScore;			// �� ����
	CEtUIStatic*	m_pStaticMyGuildScore;		// �� ��� ����
};
