
#pragma once
#include "EtUIDialog.h"
class CEtUITextBox;


// Rotha
// 특정 상황에따라서 툴팁을 표시할때 일일히 툴팁 다이얼로그를 만드려면 작업시간도 많아지고 길어지기때문에
// 공용으로 사용할 수있는 툴팁 다이얼로그를 설정합니다. < 간단하게 대응 혹은 직접 text를 설정할수 잇는 상황에 사용 > 

class CDnCustomTooltipDlg : public CEtUIDialog
{
public:
	CDnCustomTooltipDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnCustomTooltipDlg(void);
	

protected:
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	CEtUITextBox *GetTooltipTextBox() { return m_pTextBox; }

#ifdef PRE_ADD_WEEKLYEVENT
	void SetWeeklyEventText();
#endif

#ifdef PRE_ADD_ACTIVEMISSION
	void SetActiveMissionText( std::wstring & str );
#endif // PRE_ADD_ACTIVEMISSION

protected:
	CEtUITextBox *m_pTextBox;
};
