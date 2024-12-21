#pragma once

#if defined(_TEST)

#include "../../DnLauncherDlg.h"


enum emDlgPosIndexEx
{
	em_BtnOpenPath = em_MaxNum,
	em_Text_ClientPath,
	em_MaxNumEx,
};


class CDnLauncherDlgTEST : public CDnLauncherDlg
{
public:
	CDnLauncherDlgTEST( CWnd* pParent = NULL );
	virtual ~CDnLauncherDlgTEST() {}

public:
	afx_msg void OnDestroy();
	afx_msg void OnClickButtonOpenPath();

	DECLARE_MESSAGE_MAP()

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange( CDataExchange* pDX );
	virtual void InitControl();			// UI Control 초기화
	virtual void MakeFont();			// 폰트 생성
	virtual void RefreshTextInfo();		// 텍스트 정보 갱신

protected:
	CFont			m_ClientPathFont;	// 클라이언트 위치 표기용 폰트
	CxSkinButton	m_BtnOpenPath;		// 경로열기 버튼
	CString			m_strClientPath;	// 클라이언트 위치 표기
	CColorStatic	m_StaticClientPath;
};

#endif // _TEST