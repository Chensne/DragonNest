#pragma once

#if defined(PRE_ADD_MAINQUEST_UI) 
#include "DnCustomDlg.h"
#include "EtUIDialog.h"
#include "DnDataManager.h"

class CDnMainQuestAnswerDlg : public CDnCustomDlg
{
public:
	CDnMainQuestAnswerDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnMainQuestAnswerDlg();

protected:
	CDnNpcAnswerHtmlTextBox*	m_pTextBoxAnswer;
	CEtUITextureControl*		m_pBGTexCtrl;
	EtTextureHandle				m_hMainBGImage;
	std::vector<TALK_ANSWER>	m_answers;

public:
	virtual void InitialUpdate();
	virtual void Initialize(bool bShow);
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );

	void SetMainBGImage(const std::string& strFileName);
	bool OnAnswer(int nNpcID);	
	void SetAnswer(std::vector<TALK_ANSWER>& answers);
	bool GetAnswerIndex(OUT std::wstring& szIndex, OUT std::wstring& szTarget, int nCommand, bool bClearTextBoxSelect = true) const;
	const bool IsShowAnswerTextBox() { return m_pTextBoxAnswer->IsShow(); }
};

#endif // PRE_ADD_MAINQUEST_UI