#pragma once
#include "EtUIDialog.h"

class CDnGuildRequestDlg : public CEtUIDialog
{
public:
	CDnGuildRequestDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnGuildRequestDlg(void);

protected:
	CEtUIListBoxEx *m_pListBoxEx;

	CEtUIButton *m_pButtonSortJob;
	CEtUIButton *m_pButtonSortLevel;
	CEtUIButton *m_pButtonSortName;
	CEtUIStatic *m_pStaticWhiteText;
	CEtUIStatic *m_pStaticRedText;
	CEtUIStatic *m_pStaticCount;
	CEtUIButton *m_pButtonOK;
	CEtUIButton *m_pButtonCancel;

	std::vector<TGuildRecruitCharacter> m_vecRequestCharacterInfo;

	// 반대로 정렬하기 위해 각각의 정렬마다 현재값 기억해둔다.
	bool m_bCurrentReverse[3];

	void RefreshCount();
	void RefreshText();
	void RefreshList();

public:
	void OnRecvGuildRecruitCharacter( GuildRecruitSystem::SCGuildRecruitCharacterList *pPacket );
	void OnRecvGuildRecruitAcceptResult( GuildRecruitSystem::SCGuildRecruitAccept *pPacket );

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
};