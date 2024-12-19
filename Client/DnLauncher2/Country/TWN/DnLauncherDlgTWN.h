#pragma once

#if defined(_TWN)

#include "../../DnLauncherDlg.h"


enum emDlgPosIndexEx
{
	em_CheckBox_UseAgreement = em_MaxNum,
	em_Text_UseAgreement,
	em_Text_UseAgreementLink,
	em_MaxNumEx,
};


class CDnLauncherDlgTWN : public CDnLauncherDlg
{
public:
	CDnLauncherDlgTWN( CWnd* pParent = NULL );
	virtual ~CDnLauncherDlgTWN() {}

public:
	afx_msg void OnDestroy();
	afx_msg BOOL OnSetCursor( CWnd* pWnd, UINT nHitTest, UINT message );
	afx_msg HBRUSH OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor );
	afx_msg void OnClickedCheckUseAgreement();
	afx_msg void OnClickedUseAgreementLink();

	DECLARE_MESSAGE_MAP()

protected:
	virtual void DoDataExchange( CDataExchange* pDX );
	virtual void InitControl();			// UI Control 초기화
	virtual void MakeFont();			// 폰트 생성
	virtual void EnableStartButton();	// 시작 버튼 활성화

protected:
	CButton			m_BtnCheckUseAgreement;
	CFont			m_FontLink;
	BOOL			m_bUseAgreement;
	BOOL			m_UseAgreementLinkClicked;

	CColorStatic	m_StaticUseAgreement;
	CColorStatic	m_StaticUseAgreementLink;
};

#endif // _TWN