#pragma once

#include "EtUIDialog.h"

class CEtUITextBox;
class CDnChatOptDlg;

class CDnChatDlg : public CEtUIDialog, public CEtUINameLinkInterface
{
public:
	CDnChatDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnChatDlg(void);

protected:
	CEtUITextBox *m_pChatTextBox;
	CDnChatOptDlg *m_pChatOptDlg;
	CEtUIButton *m_pButtonDummy;
	int m_nChatType;

	// 현재 텍스트박스는 해상도 변경시 완전히 새로 등록해야 제대로 표시된다.
	// (AddText에서 GetCaretPos로 위치를 구해서 워드 영역을 구하는데 이걸 다시 해야 아주 정확하게 텍스트영역에 맞는 워드영역이 나온다.
	//  그냥 폰트만 교체하면 폰트는 일정단계별로 크기조절이 되서 워드영역을 넘어가게 된다.
	//  워드 영역을 넘어가면 개행되서 찍히거나 하는 문제때문에 이렇게 새로 텍스트박스에 넣을 수 밖에 없다.)
	// 다른 텍스트박스들은 새로 보여지거나 할때 새로 만들기때문에 괜찮지만,
	// 채팅창같은 경우엔 이렇게 복구용 데이터를 따로 가지고 있다가, 복구될때 재설정 하는 것으로 하겠다.
	struct SAddChatData
	{
		std::wstring wszName;
		std::wstring szMessage;
		int nChatType;
		DWORD dwFormat;
		bool isAppend;
		bool hasColor;
		DWORD colorValue;
		DWORD bgColorValue;
		bool bIgnoreCarrageReturn;
	};
	std::list<SAddChatData> m_listAddChatData;

	bool m_bIsShowScrollBar;

public:
	void InitOption( int nChatType );
	void AddChat( LPCWSTR wszName, LPCWSTR szMessage, int nChatType, DWORD dwFormat = UITEXT_NONE, bool isAppend = false, bool hasColor = false, DWORD colorValue = 0xffffffff, DWORD bgColorValue = 0, bool bIgnoreCarrageReturn = false, bool bOnResetDevice = false );
	bool IsTextBoxEmpty();

	CDnChatOptDlg &GetChatOptDlg()			{ return *m_pChatOptDlg; }
	const CDnChatOptDlg &GetChatOptDlg() const		{ return *m_pChatOptDlg; }
	int GetChatType()						{ return m_nChatType; }

	bool IsShowOptDlg();
	void ShowOptDlg( bool bShow );
	void ShowScrollBar( bool bShow );
	bool IsShowScrollBar(){return m_bIsShowScrollBar;}

#ifdef PRE_ADD_MULTILANGUAGE
	static void ChangeItemNameLinkByLanguage( LPCWSTR szMessage, std::wstring& wstrMessage );
#endif // PRE_ADD_MULTILANGUAGE

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void UpdateDlgCoord( float fX, float fY, float fWidth, float fHeight );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	virtual void OnChangeResolution();

	virtual bool OnParseTextItemInfo(const std::wstring& argString);
	virtual bool OnParseTextUserName(const std::wstring& name);

private:
	void ShowChatContextMenuDlg(bool bShow, float fX, float fY, const std::wstring& name);
};
