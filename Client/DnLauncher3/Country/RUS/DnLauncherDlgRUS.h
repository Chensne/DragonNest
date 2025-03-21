#pragma once

#if defined(_RUS)

#include "../../DnLauncherDlg.h"

enum emKORDlgPosIndex 
{
	em_StaticText_HomepageLink = 17,
	em_ExMaxNum,
};

class CDnLauncherDlgRUS : public CDnLauncherDlg
{
public:
	CDnLauncherDlgRUS( CWnd* pParent = NULL );
	virtual ~CDnLauncherDlgRUS() {}

public:
	afx_msg void OnDestroy();
	afx_msg BOOL OnSetCursor( CWnd* pWnd, UINT nHitTest, UINT message );
	afx_msg void OnClickedHomepageLink();

	DECLARE_MESSAGE_MAP()

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual void MakeFont();							// 폰트 생성
	virtual void SetStaticText();						// 텍스트 설정
	virtual BOOL InitWebpage();							// 가이드 웹페이지 초기화
	virtual BOOL InitBackground();						// 배경이미지 초기화

protected:
	CFont			m_FontStatic;						// 스태틱 폰트
	CColorStatic	m_StaticHomepageLink;
};

#endif // _RUS