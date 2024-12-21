#pragma once
#include "DnCustomDlg.h"

// rotha - PVP , 캡틴모드를 위해서 만들어진 다이얼로그 입니다 //
// 상속받은뒤 m_vec값을 증가시켜서 여러개 사용가능합니다 //

class CDnCaptionDlg_04 : public CDnCustomDlg
{
public:
	CDnCaptionDlg_04( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnCaptionDlg_04(void);

	enum CaptionNum
	{
		FirstCaption,
		SecondCaption,
	};
protected:
	std::vector<CDnMessageStatic*> m_vecStaticMessage;

public:
	void SetCaption( LPCWSTR wszMsg, DWORD dwColor, float fFadeTime , int bCaptionNum);
	void CloseCaption();

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
};