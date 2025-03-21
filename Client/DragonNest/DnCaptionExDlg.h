#pragma once
#include "EtUIDialog.h"


// DnCaptionDlg 와 동일한 기능의 CaptionDlg.
// : 추가된것은 Caption 으로 쓰일 CEtUIStatic 이 벡터에 담겨져서 UI 정의한 개수만큼 처리된다.
class CDnCaptionExDlg : public CEtUIDialog
{
public:
	CDnCaptionExDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnCaptionExDlg(void);

protected:

	std::vector< CEtUIStatic * > m_vecCaptions;	
	float m_fShowTime;
	float m_fShowRatio;
	float m_fElapsedTime;
	float m_fConst;
	EtColor m_TextColor;
	
public:
	void SetCaption( std::vector< const wchar_t * > & vecStrs, DWORD dwColor, float fFadeTime );	
	void CloseCaption();


public:
	virtual void InitialUpdate();
	virtual void Process( float fElapsedTime );
	virtual void Render( float fElapsedTime );
};
