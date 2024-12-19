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
	virtual void MakeFont();			// ��Ʈ ����
	virtual void InitControl();			// UI Control �ʱ�ȭ
	virtual void CheckVersion();		// Ŭ���̾�Ʈ, ���� ������ Ȯ���Ͽ� ��ó ����
	virtual void SetStaticText();		// �ؽ�Ʈ ����
	virtual BOOL InitWebpage();			// ���̵� �������� �ʱ�ȭ
	virtual BOOL InitBackground();		// ����̹��� �ʱ�ȭ

private:
	void UpdateRegister();

protected:
	CFont		m_14FontStatic;			// ����ƽ 14��Ʈ
	CFont		m_15FontStatic;			// ����ƽ 15��Ʈ
	CFont		m_15Font;				// 15��Ʈ �ؽ�Ʈ
	COLORREF	m_staticColor;			// ����ƽ ����
	COLORREF	m_ColorPercentage;		// % ����

	CColorStatic	m_StaticCurrentInfo;
	CColorStatic	m_StaticTextFileName;
	CColorStatic	m_StaticTextFileSize;
	CColorStatic	m_StaticTextTimeLeft;
	CColorStatic	m_StaticHomepageLink;
	CColorStatic	m_StaticTotalFile;
	CColorStatic	m_StaticCurrentFile;
};

#endif // _CHN