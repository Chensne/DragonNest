#pragma once

#if defined(_IDN)

#include "../../DnLauncherDlg.h"

enum emIDNDlgPosIndex 
{
	em_StaticText_CurrentInfo = 17,
	em_StaticText_FileName,
	em_StaticText_FileSize,
	em_StaticText_TimeLeft,
	em_StaticText_HomepageLink,
	em_ExMaxNum,
};

class CDnLauncherDlgIDN : public CDnLauncherDlg
{
public:
	CDnLauncherDlgIDN( CWnd* pParent = NULL );
	virtual ~CDnLauncherDlgIDN() {}

public:
	afx_msg void OnDestroy();
	afx_msg BOOL OnSetCursor( CWnd* pWnd, UINT nHitTest, UINT message );
	afx_msg void OnClickButtonCancel();
	afx_msg void OnClickButtonGameStart();
	afx_msg void OnClickButtonClose();
	afx_msg void OnClickedHomepageLink();

	DECLARE_MESSAGE_MAP()

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual void MakeFont();							// 폰트 생성
	virtual void SetStaticText();						// 텍스트 설정
	virtual BOOL InitWebpage();							// 가이드 웹페이지 초기화
	virtual BOOL InitBackground();						// 배경이미지 초기화

protected:
	CFont		m_11FontStatic;			// 스태틱 11폰트
	CFont		m_12FontStatic;			// 스태틱 12폰트
	CFont		m_12Font;				// 12폰트 텍스트
	COLORREF	m_staticColor;			// 스태틱 색상
	COLORREF	m_ColorPercentage;		// % 색상

	CColorStatic	m_StaticCurrentInfo;
	CColorStatic	m_StaticTextFileName;
	CColorStatic	m_StaticTextFileSize;
	CColorStatic	m_StaticTextTimeLeft;
	CColorStatic	m_StaticHomepageLink;
};

#endif // _IDN