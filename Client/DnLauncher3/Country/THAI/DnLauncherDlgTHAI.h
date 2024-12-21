#pragma once

#if defined(_THAI)

#include "../../DnLauncherDlg.h"

enum emTHAIDlgPosIndex 
{
	em_StaticText_HomepageLink = 17,
	em_ExMaxNum,
};

class CDnLauncherDlgTHAI : public CDnLauncherDlg
{
public:
	CDnLauncherDlgTHAI( CWnd* pParent = NULL );
	virtual ~CDnLauncherDlgTHAI() {}

public:
	afx_msg void OnDestroy();
	afx_msg BOOL OnSetCursor( CWnd* pWnd, UINT nHitTest, UINT message );
	afx_msg void OnClickedHomepageLink();

	DECLARE_MESSAGE_MAP()

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual void MakeFont();							// ��Ʈ ����
	virtual void SetStaticText();						// �ؽ�Ʈ ����
	virtual BOOL InitWebpage();							// ���̵� �������� �ʱ�ȭ
	virtual BOOL InitBackground();						// ����̹��� �ʱ�ȭ

protected:
	CFont			m_FontStatic;						// ����ƽ ��Ʈ
	CColorStatic	m_StaticHomepageLink;
};

#endif // _THAI