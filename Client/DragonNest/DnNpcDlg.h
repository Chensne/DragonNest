#pragma once
#include "DnCustomDlg.h"
#include "DnDataManager.h"
#include "DnBlindDlg.h"

class CDnNpcQuestRecompenseDlg;
#ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
class CDnNpcAcceptPresentDlg;
#endif // #ifdef PRE_ADD_NPC_REPUTATION_SYSTEM

#define PRE_DEL_NPCDLG_NEXTPAGE_FLAG

class CDnNpcDlg : public CDnCustomDlg, public CDnBlindCallBack, public CEtUICallback
{
public:
	CDnNpcDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnNpcDlg(void);

protected:
	CEtUIStatic *m_pStaticName;
	CEtUIHtmlTextBox *m_pTextBoxMain;
	CDnNpcAnswerHtmlTextBox *m_pTextBoxAnswer;
	CEtUITextureControl *m_pBGTextureCtl;
	CEtUITextureControl *m_pItemTextureCtl;
	CEtUIButton* m_pCloseBtn;
	CDnItemSlotButton *m_pSlotBtn;
	CDnItem *m_pItem;

#ifdef PRE_DEL_NPCDLG_NEXTPAGE_FLAG
#else
	bool m_bNextPage;
#endif	// #ifdef PRE_DEL_NPCDLG_NEXTPAGE_FLAG
	SUICoord m_MainTextBoxDummy;
	SUICoord m_AnswerTextBoxDummy;

	std::vector<TALK_ANSWER>	m_answers;

	EtTextureHandle m_hNpcImage;

	CDnNpcQuestRecompenseDlg *m_pNPCQuestRecompenseDlg;

#ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
	CDnNpcAcceptPresentDlg* m_pNpcAcceptPresentDlg;
#endif // #ifdef PRE_ADD_NPC_REPUTATION_SYSTEM

	bool m_bHideNextBlindOpened;

#ifdef PRE_ADD_AIM_ASSIST_BUTTON
	// npcDlg는 스마트무브에 대해 완전히 별도로 처리해야한다.
	bool m_bReadyMove;
	float m_fMovePosX;
	float m_fMovePosY;

	void ProcessSmartMoveCursor();
#endif

public:
	void Open();
	void Close();

	void SetNpcTalk( LPCWSTR wszNpcTalk, const std::string &strImageName, int nItemIndex );
	void SetAnswer( std::vector<TALK_ANSWER>& answers, bool bIsTalk );
	void SetRecompense( const TQuestRecompense &questRecompense, const bool bRecompense );
	void SetLevelCapRecompense( const TQuestRecompense & questRecompese );

#ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
	void SetAcceptPresent( int nNpcID );
	void OpenSelectedPresentCountDlg(int nNpcID);
	void SendSelectedNpcPresent( int nNpcID );
#endif // #ifdef PRE_ADD_NPC_REPUTATION_SYSTEM


	bool GetAnswerIndex(OUT std::wstring& szIndex, OUT std::wstring& szTarget, bool bClearTextBoxSelect = true) const;
	int GetAnswerLineIndex();

	// 아래가 호출되고나선 다음번 OnBlindOpened의 Open함수에서 Show(true)를 호출하지 않는다.
	void HideNextBlindOpened( bool bHide = true ) { m_bHideNextBlindOpened = bHide; }

#ifdef PRE_ADD_AIM_ASSIST_BUTTON
	void GetSmartMovePos( std::vector<EtVector2> &vecSmartMovePos );
#endif

public:
	virtual void InitialUpdate();
	virtual void InitCustomControl(CEtUIControl *pControl);
	virtual void Initialize(bool bShow);
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Render( float fElapsedTime );
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );

public:
	virtual void OnBlindOpen();
	virtual void OnBlindOpened();
	virtual void OnBlindClose();
	virtual void OnBlindClosed();
	virtual bool OnAnswer(int nNpcID);
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );

#if defined(PRE_ADD_MAILBOX_OPEN) || defined(PRE_ADD_68286)
protected:
	bool m_bSkipOpen;

public:
	void SetSkipOpen(bool bSkip) { m_bSkipOpen = bSkip; }
#endif // PRE_ADD_MAILBOX_OPEN

#if defined(PRE_ADD_QUEST_BACK_DIALOG_BUTTON)
	CDnNpcQuestRecompenseDlg*	GetNpcQuestRecompenseDlg()  { return m_pNPCQuestRecompenseDlg; }
	CDnNpcAcceptPresentDlg*		GetNpcPresentDlg()			{ return m_pNpcAcceptPresentDlg; }
#endif

};