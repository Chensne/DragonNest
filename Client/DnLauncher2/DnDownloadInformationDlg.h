#pragma once

#include "resource.h"
#include "WLibTorrent.h"

#ifdef _USE_BITTORRENT

#define UPDATE_TIMER  1000

class CDnDownloadInformationDlg : public CDialog
{
	DECLARE_DYNAMIC(CDnDownloadInformationDlg)

public:
	CDnDownloadInformationDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDnDownloadInformationDlg();

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_DOWNLOAD_INFORMATION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnTimer( UINT_PTR nIDEvent );
	afx_msg void OnBnClickedOk();

protected:
	bool CheckPeers( std::vector<peer_info>& peers );
	void CtlEraseBkgnd( int nCtlID );
	void RefreshTextInfo();

private:
	CString m_strTextFile;
	CString m_strTextStatus;
	CString m_strTextDownload;
	CString m_strTextUpload;
	CString m_strTextPeers;
	CString m_strTextSeeds;

	std::vector<peer_info> m_vecPeers;
};

#endif // _USE_BITTORRENT