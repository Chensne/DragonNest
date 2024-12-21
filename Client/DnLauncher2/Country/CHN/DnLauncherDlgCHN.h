#pragma once

#if defined(_CHN)

#include "../../DnLauncherDlg.h"

enum emCHNDlgPosIndex 
{
	em_StaticText_CurrentInfo = 22,
	em_StaticText_FileName,
	em_StaticText_FileSize,
	em_StaticText_TimeLeft,
	em_StaticText_HomepageLink,
	em_StaticText_TotalFile,
	em_StaticText_CurrentFile,
	em_ExMaxNum,
};

class CDnLauncherDlgCHN : public CDnLauncherDlg
{
public:
	CDnLauncherDlgCHN( CWnd* pParent = NULL );
	virtual ~CDnLauncherDlgCHN() {}

public:
	afx_msg void OnDestroy();
	afx_msg BOOL OnSetCursor( CWnd* pWnd, UINT nHitTest, UINT message );
	afx_msg void OnClickButtonGameStart();
	afx_msg void OnClickButtonClose();
	afx_msg void OnClickedHomepageLink();

	DECLARE_MESSAGE_MAP()

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void MakeFont();			// 폰트 생성
	virtual void InitControl();			// UI Control 초기화
	virtual void CheckVersion();		// 클라이언트, 서버 버전을 확인하여 런처 셋팅
	virtual void SetStaticText();		// 텍스트 설정
	virtual BOOL InitWebpage();			// 가이드 웹페이지 초기화
	virtual BOOL InitBackground();		// 배경이미지 초기화

private:
	void UpdateRegister();

protected:
	CFont		m_14FontStatic;			// 스태틱 14폰트
	CFont		m_15FontStatic;			// 스태틱 15폰트
	CFont		m_15Font;				// 15폰트 텍스트
	COLORREF	m_staticColor;			// 스태틱 색상
	COLORREF	m_ColorPercentage;		// % 색상

	CColorStatic	m_StaticCurrentInfo;
	CColorStatic	m_StaticTextFileName;
	CColorStatic	m_StaticTextFileSize;
	CColorStatic	m_StaticTextTimeLeft;
	CColorStatic	m_StaticHomepageLink;
	CColorStatic	m_StaticTotalFile;
	CColorStatic	m_StaticCurrentFile;
};

#endif // _CHN