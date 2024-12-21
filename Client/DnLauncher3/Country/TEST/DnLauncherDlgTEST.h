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
	virtual void InitControl();			// UI Control �ʱ�ȭ
	virtual void MakeFont();			// ��Ʈ ����
	virtual void RefreshTextInfo();		// �ؽ�Ʈ ���� ����

protected:
	CFont			m_ClientPathFont;	// Ŭ���̾�Ʈ ��ġ ǥ��� ��Ʈ
	CxSkinButton	m_BtnOpenPath;		// ��ο��� ��ư
	CString			m_strClientPath;	// Ŭ���̾�Ʈ ��ġ ǥ��
	CColorStatic	m_StaticClientPath;
};

#endif // _TEST