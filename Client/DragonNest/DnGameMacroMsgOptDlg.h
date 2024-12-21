#pragma once
#include "DnOptionDlg.h"
#include "GameOption.h"

class CDnGameMacroMsgOptDlg : public CDnOptionDlg, public CEtUICallback
{
public:
	CDnGameMacroMsgOptDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnGameMacroMsgOptDlg();

private:
	typedef CDnOptionDlg BaseClass;

protected:
	CEtUIIMEEditBox *m_pEditBox[ CGameOption::NUM_COMM_MACROS ];
	int m_nDefaultMacroUIStringID[ CGameOption::NUM_COMM_MACROS ];

	// 리턴용 임시 변수
	WCHAR m_wszTemp[MAX_PATH];

public:
	// 매크로ui파일에 국가별 글자수가 다를 수 있기때문에,
	// 그리고 디폴트매크로 로딩을 GameOption에서 못하기때문에, 여기서 로딩하는 등 여러가지 이유로
	// 실제 매크로 데이터를 얻을땐 매크로 옵션 다이얼로그에서 가져온다.
	LPCWSTR GetMacroString( int nIndex );	// 0~3 범위

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

public:
	virtual void ExportSetting();
	virtual void ImportSetting();
	virtual bool IsChanged();
	void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 ) override;

	void ResetDefault();	// 기본값 복구
};
