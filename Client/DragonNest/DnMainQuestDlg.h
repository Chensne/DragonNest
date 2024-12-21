#pragma once

#if defined(PRE_ADD_MAINQUEST_UI)
#include "DnCustomDlg.h"
#include "EtUIDialog.h"
#include "DnDataManager.h"

class CDnMainQuestAnswerDlg;
class CDnMainQuestRewardDlg;
class CDnImageLeftBlindNpcDlg;
class CDnImageRightBlindNpcDlg;

class CDnMainQuestDlg : public CDnCustomDlg, public CEtUICallback
{
public:
	CDnMainQuestDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnMainQuestDlg(void);

protected:
	CDnMainQuestAnswerDlg*		m_pAnswerDlg;
	CDnMainQuestRewardDlg*		m_pRewardDlg;

	CDnImageLeftBlindNpcDlg*	m_pLeftNpcImgDlg;
	CDnImageRightBlindNpcDlg*	m_pRightNpcImgDlg;

	CEtUIStatic*				m_pStaticName;
	CEtUIHtmlTextBox*			m_pTextBoxMain;
	CEtUIButton*				m_pSelectButton;
	CEtUIStatic*				m_pBlackBG;

	CDnItem*					m_pItem;
	CDnItemSlotButton*			m_pSlotBtn;
	CEtUITextureControl*		m_pItemEffectTexCtrl;
	EtTextureHandle				m_hItemEffectBGImage;
	
	bool							m_bIsOpenMainQuestDlg;
	QuestRenewal::SelectType::eType m_SelectBtnType;

public:
	virtual void InitialUpdate();
	virtual void Initialize(bool bShow);
	virtual void InitCustomControl(CEtUIControl *pControl);
	virtual void Show( bool bShow );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual void Process( float fElapsedTime );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void OnChangeResolution();

protected:
	void SetNpcTalk( LPCWSTR wszNpcTalk );	
	void SetNpcPortrait(const std::string strLeft, const std::string strRight);
	void SetButtonText(BYTE cSelectType);
	void TakeRecompenseItem();
	bool SetSlotItem(int nItemIndex, const std::string strImgName);

public:
	void Start();
	void End();

	const bool IsOpenMainQuestDlg()				{ return m_bIsOpenMainQuestDlg; }
	void SetOpenMainQuestDlgFlag(bool bFlag)	{ m_bIsOpenMainQuestDlg = bFlag; }	
	
	void SetMainQuestParagraph(TALK_PARAGRAPH talk_para);
	bool GetAnswerIndex(OUT std::wstring& szIndex, OUT std::wstring& szTarget, int nCommand, bool bClearTextBoxSelect = true) const;
	bool OnAnswer(int nNpcID);
	void SetRecompense(const TQuestRecompense &questRecompense, const bool bRecompense);
	void SetLevelCapRecompense(const TQuestRecompense & questRecompese);
};

#endif // PRE_ADD_MAINQUEST_UI