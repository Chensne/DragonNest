#pragma once
#include "EtUIDialog.h"
#include "DnInterface.h"

#ifdef PRE_ADD_ALTHEIA_NEWCAPTION_TRIGGER 
class CDnImageLeftBlindNpcDlg;
class CDnImageRightBlindNpcDlg;

class CDnImageBlindDlg : public CEtUIDialog, public CEtUICallback
{
public:
	enum emBLIND_NPC_IMAGE_TYPE
	{
		E_NPC_IMAGE_LEFT,
		E_NPC_IMAGE_RIGHT,
	};

	enum emBLIND_STATE
	{
		E_BLIND_OPEN,
		E_BLIND_CLOSE
	};

public:
	CDnImageBlindDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnImageBlindDlg();

protected:
	CEtUIStatic*	m_pBlindStatic;
	CEtUITextBox*	m_pBlindCationTextBox;

	float			m_fCaptionDelta;
	SUICoord		m_uiBlind;
	emBLIND_STATE	m_BlindState;

	CDnImageLeftBlindNpcDlg*  m_pLeftImageBlindCaptionDlg;
	CDnImageRightBlindNpcDlg* m_pRightImageBlindCaptionDlg;

public:
	void CloseBlind();
	void ClearCaptionBox()  { if(!m_pBlindCationTextBox) return; m_pBlindCationTextBox->ClearText(); }

	void SetCaption(const wchar_t *wszCaption, int nDelay=0);
	void UpdateBlind();
	void SetNPCImage(bool bShowNPCName, WCHAR* wszName, const char* szTextureFileName, int nImageType);

public:
	virtual void Initialize(bool bShow);
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );
	virtual void OnChangeResolution();
};

#endif