#pragma once
#include "DnCustomDlg.h"


#ifdef PRE_MOD_SELECT_CHAR

class CDnCharGestureDlg : public CDnCustomDlg
{
public:
	CDnCharGestureDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnCharGestureDlg();

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Show( bool bShow );

public:
	enum eParentType{ PARENT_CHARSELECT = 0, PARENT_CHARCREATE };
	void SetCharCreateDlg( bool bCharCreate ) { m_bCharCreate = bCharCreate; }

private:
	void SetGestureInfo();

protected:
	enum { MAX_GESTURE = 5 };
	CDnLifeSkillButton*			m_pGestureSlotButton[MAX_GESTURE];
	CEtUIStatic*				m_pGestureText[MAX_GESTURE];
	CEtUIButton*				m_pGestureButton[MAX_GESTURE];
	std::vector<std::string>	m_vecStrGestureActionName;
	bool						m_bCharCreate;
};

#endif // PRE_MOD_SELECT_CHAR