//////////////////////////////////////////////////////////////////////////
// DnLauncherDlg.h : header file
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "xSkinButton.h"
#include "SkinProgress.h"
#include "ColorStatic.h"
#include "DnPatchThread.h"

#define DRAGGING	0x01	// dragging flag

enum emDlgPosIndex 
{
	em_WebPageHtml = 0,
	em_BtnQuit,
	em_BtnStartGame,
	em_BtnOption, 
	em_BtnWinClose,
	em_BtnWinMiniMum,
	em_DownloadProgress,
	em_FileCountProgress,
	em_Text_LauncherVersion,
	em_Text_FileCount,
	em_Text_FileName,
	em_Text_TimeLeft,
	em_Text_FileSize,
	em_Text_DownloadPercentage,
	em_Text_DownloadState,
	em_Text_DirectStart,
	em_CheckBox_DIrectStart,
#ifdef _USE_PARTITION_SELECT
	em_BtnStartPatch,
	em_BtnSelectPartition,
	em_Text_SelectPartition,
	em_Text_SelectPartitionName,
	em_Text_SelectPartitionGuide,
#endif // _USE_PARTITION_SELECT
#ifdef _USE_BITTORRENT
	em_BtnDownloadInformation,
#endif // _USE_BITTORRENT
	em_MaxNum,
};


class CDnHtmlView;

// CDnLauncherDlg dialog
class CDnLauncherDlg : public CDialog
{
// Construction
public:
	CDnLauncherDlg( CWnd* pParent = NULL );	// standard constructor
	virtual ~CDnLauncherDlg() {}

public:
#ifdef _USE_PARTITION_SELECT
	void SetSelectPartition( int nSelectChannelNum, int nSelectPartitionNum );		// ��Ƽ�� ���� Html���� ������ ������ �� ��� ó��
	void InitSelectPartition();
#endif // _USE_PARTITION_SELECT

#ifdef _FIRST_PATCH
	void SetOnlySelectPartition( int nSelectChannelNum, int nSelectPartitionNum );		// ��Ƽ�� ���� Html���� ������ ������ �� ��� ó��
	void CheckVersionAndReboot();
	BOOL SaveModuleVersion( int nVersion );
#endif
	
	static CDnLauncherDlg* CreateLauncherDlg();

	// Generated message map functions
	afx_msg void OnDestroy();
	afx_msg LRESULT OnStatusMsg( WPARAM wParam, LPARAM lParam );
	afx_msg void OnTimer( UINT_PTR nIDEvent );
	afx_msg void OnLButtonDown( UINT nFlags, CPoint point );
	afx_msg void OnLButtonUp( UINT nFlags, CPoint point );
	afx_msg void OnMouseMove( UINT nFlags, CPoint point );
	afx_msg BOOL OnEraseBkgnd( CDC* pDC );
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor );
	afx_msg HCURSOR OnQueryDragIcon();
	// Button Ctrl Click
	afx_msg void OnClickedCheckDirectGamestart();
	afx_msg void OnClickButtonCancel();
	afx_msg void OnClickButtonGameStart();
	afx_msg void OnClickButtonOption();
	afx_msg void OnClickButtonClose();
	afx_msg void OnClickMinimumBtn();
#ifdef _USE_PARTITION_SELECT
	afx_msg void OnClickButtonStartPatch();
	afx_msg void OnClickButtonSelectPartition();
#endif // _USE_PARTITION_SELECT
#ifdef _USE_BITTORRENT
	afx_msg void OnClickButtonDownloadInformation();
#endif // _USE_BITTORRENT
	DECLARE_MESSAGE_MAP()

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual void InitControl();			// UI Control �ʱ�ȭ
	virtual void MakeFont() {}			// ��Ʈ ����
	virtual void SetStaticText();		// �ؽ�Ʈ ����
	virtual void RefreshTextInfo();		// �ؽ�Ʈ ���� ����
	virtual void EnableStartButton();	// ���� ��ư Ȱ��ȭ
	virtual void NavigateHtml();		// �������� �̵�
	virtual BOOL InitWebpage();			// ���̵� �������� �ʱ�ȭ
	virtual BOOL InitBackground();		// ��� �̹��� �ʱ�ȭ
#ifdef _USE_PARTITION_SELECT
	virtual void CheckVersion();		// Ŭ���̾�Ʈ, ���� ������ Ȯ���Ͽ� ��ó ����
#endif // _USE_PARTITION_SELECT

	BOOL InitFail();					// ��ó �ʱ�ȭ ����
	BOOL InitInterface();				// UI �ʱ�ȭ

	void StartPatchDownloadThread();	// ��ġ �ٿ�ε� ������ ����
	void CtlEraseBkgnd( int nCtlID );

	// Update Download Info
	void ResetDownloadInfo();					// �ٿ�ε� ���� �ʱ�ȭ
	void UpdateDownloadInfo();					// �ٿ�ε� ���� ������Ʈ
	CString GetAdjustFileSize( double size );	// ���� ������ ��Ʈ�� �����
	void UpdatProgressInfo();					// ���α׷��� ���� ������Ʈ
	void UpdateDownloadLeftTime();				// �ٿ�ε� ���� �ð� ������Ʈ
	CRect* GetControlPos( emDlgPosIndex index ) { return &m_pRectCtrl[index]; }

// Implementationz
public:
	// Dialog Data
	enum { IDD = IDD_DNLAUNCHER_DIALOG };
	enum { TEVENT_REFRESH_TEXTINFO = 0 };	// Ÿ�̸� �̺�Ʈ
	std::vector<POINT> m_vecDisplayMode;	// ���� �ػ� ���� �����

protected:
	// �������̽� ���� ��� ����
	CRect*	m_pRectCtrl;
	int		m_nMaxCtrlNum;

	HICON	m_hIcon;
	DWORD	m_dwWidth;
	DWORD	m_dwHeight;
	HDC		m_dcBkGrnd;			// background bitmap dc
	DWORD	m_dwFlags;			// flags
	POINT	m_pntMouse;			// Mouse position for dragging
	CFont	m_Font;				// ��Ʈ

	DownloadPatchState	m_emDownloadPatchState;	// �ٿ�ε� ����
	
	// ProgressBar
	CSkinProgress*	m_pDownloadProgress;
	CSkinProgress*	m_pFileCountProgress;
	CBitmap*		m_pDownloadProgressBitmap;
	CBitmap*		m_pFileCountProgressBitmap;
	CStatic			m_StaticDownloadProgress;
	CStatic			m_StaticFileCountProgress;
	__int64			m_nDownloadProgressMin;
	__int64			m_nDownloadProgressMax;
	int				m_nCurrentFileCount;
	int				m_nTotalFileCount;
	float			m_fDownloadRate;
	BOOL			m_bDownloading;

	// Button
	CButton			m_BtnCheckDirectGameStart;
	CxSkinButton	m_BtnCancel;
	CxSkinButton	m_BtnStartGame;
	CxSkinButton	m_BtnGameOption;
	CxSkinButton	m_BtnClose;
	CxSkinButton	m_BtnMinimum;
#ifdef _USE_PARTITION_SELECT
	CxSkinButton	m_BtnStartPatch;
	CxSkinButton	m_BtnSelectPartition;
#endif // _USE_PARTITION_SELECT
#ifdef _USE_BITTORRENT
	CxSkinButton	m_BtnDownloadInformation;
#endif // _USE_BITTORRENT

	// String
	CString		m_strLauncherVersion;		// ��ó ����
	CString		m_strFileName;				// �ٿ�ε� �޴� ���ϸ�
	CString		m_strFileSize;				// �ٿ�ε� �޴� ���� ������
	CString		m_strTimeLeft;				// �ٿ�ε� ���� �ð�
	CString		m_strFileCount;				// �ٿ�ε� ���� ���� �� ��
	CString		m_strDownloadPercentage;	// �ٿ�ε� �ۼ�Ʈ
	CString		m_strDownloadState;			// �ٿ�ε� ����
	CString		m_strDirectGameStart;		// ��ġ�Ϸ� �� �ٷ� ���ӽ���
#ifdef _USE_PARTITION_SELECT
	CString		m_strSelectPartition;		// ������ ��Ƽ��
	CString		m_strSelectPartitionName;	// ������ ��Ƽ�Ǹ�
	CString		m_strSelectPartitionGuide;	// ��Ƽ�� ���� �ȳ�
#endif // _USE_PARTITION_SELECT

	// Static
	CColorStatic	m_StaticLauncherVersion;
	CColorStatic	m_StaticFileName;
	CColorStatic	m_StaticFileSize;
	CColorStatic	m_StaticTimeLeft;
	CColorStatic	m_StaticFileCount;
	CColorStatic	m_StaticDownloadPercentage;
	CColorStatic	m_StaticDownloadState;
	CColorStatic	m_StaticDirectGameStart;
#ifdef _USE_PARTITION_SELECT
	CColorStatic	m_StaticSelectPartition;
	CColorStatic	m_StaticSelectPartitionName;
	CColorStatic	m_StaticSelectPartitionGuide;
#endif // _USE_PARTITION_SELECT
	CColorStatic	m_StaticszDirectStart;	// ��ġ �Ϸ� �� ���� �ٷ� ���� ���� Static
	COLORREF		m_staticBaseColor;		// ����ƽ �⺻ ����

	CDnHtmlView*	m_pDnHtmlView;			// ��ó�� ���� ��ü ��������
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
