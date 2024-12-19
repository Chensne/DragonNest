#pragma once

#include "EtUIDialog.h"

#if defined( PRE_ADD_PRESET_SKILLTREE )

class CDnSkillTreeDlg;

class CDnSkillTreeSetMessageDlg : public CEtUIDialog
{
public:
	enum{
		E_ADD_SKILLTREE_PRESET,
		E_DEL_SKILLTREE_PRESET,
	};

public:
	CDnSkillTreeSetMessageDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallBack = NULL, bool bAutoCursor = false );
	virtual ~CDnSkillTreeSetMessageDlg();

	virtual void Initialize(bool bShow);
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );

	void SetModeShow( const int eMode );
	void SetSkillTreeDlg( CDnSkillTreeDlg * pSkillTreeDlg );

protected:
	CEtUIIMEEditBox * m_pNameEditBox;
	CEtUIStatic * m_pNameBoard;
	CEtUIStatic * m_pNameComment;
	CEtUIStatic * m_pDelComment;
	CEtUIButton * m_pApplyButton;

	CDnSkillTreeDlg * m_pSkillTreeDlg;

	int m_nMode;
};

#endif	// #if defined( PRE_ADD_PRESET_SKILLTREE )