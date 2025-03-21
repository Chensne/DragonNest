#pragma once
#include "EtUIDialog.h"
#include "DnInterface.h"

class CDnAssertDlg : public CEtUIDialog, public CSingleton<CDnAssertDlg>
{
public:
	enum emReturnValue
	{
		valueNone,
		valueBreak,
		valueContinue,
		valueIgnore,
	};

public:
	CDnAssertDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnAssertDlg(void);

protected:
	CEtUIStatic *m_pStaticMsg;
	emReturnValue m_emRetValue;
	CDnInterface::InterfaceTypeEnum m_emInterfaceType;

public:
	void SetInfo( CDnInterface::InterfaceTypeEnum emType, const wchar_t *szMsg, CEtUICallback *pCallback );
	emReturnValue GetReturnValue() { return m_emRetValue; }

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Show( bool bShow );
};

void _gassert( const wchar_t *szExp, const wchar_t *szFile, unsigned nLine );