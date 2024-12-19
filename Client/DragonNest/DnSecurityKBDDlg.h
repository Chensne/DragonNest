#pragma once
#include "EtUIDialog.h"
#include "DnSecurityBase.h"

class CDnSecurityKBDDlg : public CEtUIDialog , public CDnSecurityBase
{
public:
	CDnSecurityKBDDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnSecurityKBDDlg(void);


	struct SSpecialChar 
	{
		SSpecialChar(WCHAR _LowerChar, WCHAR _UpperChar )
		{
			LowerChar = _LowerChar;
			UpperChar = _UpperChar;
		}
		WCHAR LowerChar;
		WCHAR UpperChar;
	};

	struct KeyName
	{
		enum
		{
			emKeyShuffle,
			emKeyReturn,
			emKeyDelete,
			emKeyLShift,
			emKeyRshift,
			emKeyOk,
			emKeyCancel,
			emKeyClose,
			emkeyMax ,
		};
	};



protected:
	
	std::vector<std::wstring> m_vecKeyPadStr;
	std::vector<CEtUIButton*> m_pBtnKeyBodyPad;
	std::vector<SSpecialChar> m_SpecialChar;

	std::wstring m_BotChar;
	std::wstring m_TopChar;


	CEtUIButton* m_BtnKey[KeyName::emkeyMax];

	CEtUIEditBox* m_pEditBox;
	std::wstring m_wszCurStr;
	

	int  m_State;
	bool bStateCapslock;
	bool bPushShift; 
	
	
	DWORD m_dwNowTime;
	float m_fTime;
	float m_fOldTime; 
	bool bClickDelSec; 
	/*
	CEtUIButton* m_BtnKeyShuffle;
	CEtUIButton* m_BtnKeyRetrun;
	CEtUIButton* m_BtnKeyDelete;
	CEtUIButton* m_BtnCapslock;
	CEtUIButton* m_BtnLShift; 
	CEtUIButton* m_BtnRShift; 
	CEtUIButton* m_BtnLShift; 
	*/




public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Process( float fElapsedTime );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Show( bool bShow , int State );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	WCHAR ChangeChar(WCHAR *ch , bool bFlag = true);
	void InitSpecialChar();
	void ShuffleNumber();
	void KeyDefault();
	void KeyDelete();
	void KeyUpper();
	void KeyLower();




};

