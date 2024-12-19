#pragma once

#include "DnCustomDlg.h"
#include "DnBlindDlg.h"

#define	MAX_TOURNAMENT_COUNT	16
#define RESULT_LINE_COUNT		28 // MAX_TOURNAMENT_COUNT + (MAX_TOURNAMENT_COUNT / 2) + (MAX_TOURNAMENT_COUNT / 4)

class CDnGuildWarFinalTournamentListDlg : public CDnCustomDlg, public CDnBlindCallBack
{
public:
	CDnGuildWarFinalTournamentListDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnGuildWarFinalTournamentListDlg();

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg );

public:
	void SetTournamentUI();
	void ClearTournamentUI();

protected:
	CEtUIStatic*	m_pStaticTournamentInfo;								// ��ʸ�Ʈ ����
	CEtUIStatic*	m_pStaticGuildName[ MAX_TOURNAMENT_COUNT ];				// ��ʸ�Ʈ ���� ��� �̸�
	CEtUIStatic*	m_pStaticListOn[ MAX_TOURNAMENT_COUNT ];				// ��ʸ�Ʈ �����ִ� ��� ǥ��
	CEtUIStatic*	m_pStaticLeftRoundResultLine[ RESULT_LINE_COUNT / 2 ];	// ���� ���� ��� ǥ�� ��
	CEtUIStatic*	m_pStaticRightRoundResultLine[ RESULT_LINE_COUNT / 2 ];	// ������ ���� ��� ǥ�� ��
	CEtUIButton*	m_pButtonVote[ MAX_TOURNAMENT_COUNT ];					// ��ǥ ��ư
	CEtUITextureControl* m_pGuildMark[ MAX_TOURNAMENT_COUNT ];
};
