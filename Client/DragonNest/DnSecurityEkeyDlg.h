#pragma once
#include "EtUIDialog.h"

/*
#if defined(PRE_ADD_CH_SNDAAUTH_02)
#else	// #if defined(PRE_ADD_CH_SNDAAUTH_02)
class CDnSecurityEkeyDlg : public CEtUIDialog
{
public:
	enum emSecurityType{ 
		emEKey , 
		emECradNo,  
		emResvPass, 
	};


	CDnSecurityEkeyDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnSecurityEkeyDlg(void);

protected:

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );

	void SetChallengeKey(WCHAR *pStr);
	void SetType(emSecurityType Stype = emSecurityType::emEKey);

	emSecurityType m_SecurityType; 
	WCHAR m_Str[20];
	std::wstring m_ChallengeKey;



};
#endif	// #if defined(PRE_ADD_CH_SNDAAUTH_02)

*/