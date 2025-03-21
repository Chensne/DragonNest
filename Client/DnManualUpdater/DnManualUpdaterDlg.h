// DnManualUpdaterDlg.h : header file
//
#include "afxcmn.h"
#include "afxwin.h"


class CDnAttachFile;
#pragma once

enum MUError
{
	eSuccess,
	eTextFileFail,
};

enum emPackStep
{
	emOutPack,
	emApplyPack,
};


#define RETRY_MAX_COUNT		3
#define SERVER_VERSION_NAME		_T( "PatchInfoServer.cfg" )
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }


// CDnManualUpdaterDlg dialog
class CDnManualUpdaterDlg : public CDialog 
{
	// Construction
public:
	CDnManualUpdaterDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CDnManualUpdaterDlg();

	// Dialog Data
	enum { IDD = IDD_DNMANUALUPDATER_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

public:
	CDnAttachFile*	m_pDnAttachFile;
	CString			m_szClientPath;
	CString			m_szClientUpdatePath;

	CString			m_szPatchFile;
	CString			m_szPatchListFile;
	
	CString			m_szModuleName;
	TCHAR			m_szModulePath[MAX_PATH];

	emPackStep		m_PackStep;

	int				m_iPrevVersion;
	int				m_iLastVersion;

	DWORD			m_dwTotalSize;		//��ü Ǯ
	DWORD			m_dwNowTotalsize;
	DWORD			m_dwFileSize;
	DWORD			m_dwNowFileSize;
	bool			m_bUserCancel;
	bool			m_bCompleted;

	CWinThread*		m_thread;
	HANDLE			m_hThread;
	bool			m_bFinishThread;
	CString			m_strVersion;
	CString			m_strState;
	CProgressCtrl	m_ctlProgressTotal;
	CProgressCtrl	m_ctlProgressFile;

public:
	void	UpdateMain();
	int		FilePacking();

	bool	LoadPatchList( std::vector< std::string > &vecDeleteList, std::vector< std::string > &vecPatchList, std::vector< std::string > &vecCopyList );
	
	bool	CheckPackFile();
	int		GetFileSize(TCHAR *pFileName);
	
	void	GetModulePath();
	void	GetModuleFileName();

#ifdef USE_SERVER_VERSION_CHECK
	int		CheckServerVersion();
#endif // USE_SERVER_VERSION_CHECK

	HRESULT DownloadToFile( LPCTSTR strDownloadUrl, LPCTSTR strSaveFilePath );
	int		LoadVersion( CString& szFilePath );
	int		LoadVersion();
	int		LoadVersionOld();
	BOOL	SaveVersion(int Version);
	BOOL	SaveVersionOld(int Version);

	void	ResourceOptimize();
	void	DeletePakFile(TCHAR *pFileName);
	bool	InitializePatchUpdate();

	bool	IsFinishThread() { return m_bFinishThread; }

protected:
	HICON	m_hIcon;
	bool	m_bTerminateThread;
	
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnStnClickedStateVersion();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnCancel();
	virtual void OnOK();
	
	DECLARE_MESSAGE_MAP()
};
