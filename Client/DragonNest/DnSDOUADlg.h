#pragma once
#include "EtUIDialog.h"
#include "RyeolHttpClient.h"

#if defined(_CH) && defined(_AUTH)
using namespace Ryeol;

class CDnSDOUADlg : public CEtUIDialog
{
public:
	CDnSDOUADlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnSDOUADlg(void);

protected:
	CEtUIIMEEditBox *m_pIMEEditBoxName;
	CEtUIEditBox *m_pEditBoxNumber;
	CEtUIEditBox *m_pEditBoxEmail;
	CHttpClient m_HttpClient;
	wstring m_strUrlInfo;
	wstring m_strAccountName;

	bool MakeAndSendHttpInfo();

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Show( bool bShow );
	void SetUrlInfo(WCHAR* wszAccountName, char* szUrlInfo);
};

#endif	// #if defined(_CH) && defined(_AUTH)