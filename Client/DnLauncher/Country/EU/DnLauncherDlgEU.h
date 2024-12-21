#pragma once

#if defined(_EU)

#include "../../DnLauncherDlg.h"

enum emEUDlgPosIndex 
{
	em_StaticText_CurrentInfo = 17,
	em_StaticText_FileName,
	em_StaticText_FileSize,
	em_StaticText_TimeLeft,
	em_StaticText_HomepageLink,
	em_StaticText_TotalProgress,
	em_StaticText_CurrentProgress,
	em_BtnENG,
	em_BtnFRA,
	em_BtnGER,
	em_BtnSPA,
	em_StaticText_SelectLanguage,
	em_ExMaxNum,
};

enum emLanguage
{
	em_NONE = -1,
	em_ENG = 0,
	em_GER,
	em_FRA,
	em_ESP,
};

class CDnLauncherDlgEU : public CDnLauncherDlg
{
public:
	CDnLauncherDlgEU( CWnd* pParent = NULL );
	virtual ~CDnLauncherDlgEU() {}

public:
	afx_msg void OnDestroy();
	afx_msg BOOL OnSetCursor( CWnd* pWnd, UINT nHitTest, UINT message );
	afx_msg void OnClickedHomepageLink();
	afx_msg void OnClickedENG();
	afx_msg void OnClickedFRA();
	afx_msg void OnClickedGER();
	afx_msg void OnClickedSPA();

	DECLARE_MESSAGE_MAP()

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual void MakeFont();		// ��Ʈ ����
	virtual void SetStaticText();	// �ؽ�Ʈ ����
	virtual BOOL InitWebpage();		// ���̵� �������� �ʱ�ȭ
	virtual BOOL InitBackground();	// ����̹��� �ʱ�ȭ
	virtual void InitControl();		// UI Control �ʱ�ȭ
	virtual void RefreshTextInfo();	// �ؽ�Ʈ ���� ����

public:
	void SetSelectLanguageIndex( emLanguage eLanguage );
	emLanguage GetSelectLanguageIndex() { return m_eSelectLanguage; }

protected:
	COLORREF	m_staticGrayColor;		// ����ƽ ȸ������
	COLORREF	m_staticOrangeColor;	// ����ƽ ����������

	HANDLE		m_hFont;
	CFont		m_NormalFont;	// ������Ʈ
	CFont		m_BoldFont;		// ������Ʈ

	CColorStatic	m_StaticCurrentInfo;
	CColorStatic	m_StaticTextFileName;
	CColorStatic	m_StaticTextFileSize;
	CColorStatic	m_StaticTextTimeLeft;
	CColorStatic	m_StaticHomepageLink;
	CColorStatic	m_StaticTotalProgress;
	CColorStatic	m_StaticCurrentProgress;
	CColorStatic	m_StaticTextSelectLanguage;

	CxSkinButton	m_BtnENG;
	CxSkinButton	m_BtnGER;
	CxSkinButton	m_BtnFRA;
	CxSkinButton	m_BtnSPA;

	CString m_strHomepageLink;
	CString m_strStaticTotalProgress;
	CString m_strStaticCurrentProgress;
	CString m_strStaticCurrentInfo;
	CString m_strStaticFileName;
	CString m_strStaticTextFileSize;
	CString m_strStaticTextTimeLeft;
	CString m_strStaticTextSelectLanguage;

	emLanguage		m_eSelectLanguage;
};

#endif // _EU