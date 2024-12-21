// PatchBuilderP4Dlg.h : 헤더 파일
//

#pragma once
#include "afxwin.h"

class CDnAttachFile;

// CPatchBuilderP4Dlg 대화 상자
class CPatchBuilderP4Dlg : public CDialog
{
	// 생성입니다.
public:
	CPatchBuilderP4Dlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_PATCHBUILDERP4_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

	// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	std::vector<int> m_vecRevision;
	std::vector<std::string> m_vecModifyFile;
	std::vector<char> m_vecAction;
	std::vector<std::string> m_vecDeleteFile;

	std::vector<std::string> m_vecRealAddFile;	//추가된 데이터
	//RealModify 와 RealDeleteFile 복구해야할 파일이다.
	std::vector<std::string> m_vecRollDeleteFile;	//삭제된 데이터
	std::vector<std::string> m_vecRestoreFile;	//복구할 데이터

	char m_szModuleRoot[MAX_PATH];

	CString m_szP4Url;
	CString m_szP4UrlFolder;
	CString m_szP4UrlServer;
	CString m_szP4UrlServerFolder;
	CString m_szP4Workspace;
	CString m_szP4ClientRoot;
	CString m_szUpdateFolder;
	CString m_szPatchFolder;
	CString m_szRMakeCmd;
	CString m_szCountryCode;
	CComboBox m_SelectProfile;
	CComboBox m_SelectWorkspace;
	CString m_szRollTempFolder;
	CString m_szRollPatchFolder;
	CString m_szManualPatchExe;

	int m_nDefaultProfile;
	int m_nCurrentVersion;
	int m_nLastRevision;
	int m_nStartRevision;

	BOOL m_bMakeFullVersion;
	BOOL m_bCopyServerData;
	BOOL m_bAutoStart;
	BOOL m_bRollBack;

	CDnAttachFile *m_pDnAttachFile;
	BOOL m_bManualPatch;

	std::vector<int> m_SkipNumber;

	BOOL m_bSetWorkspace;

	void SetChangelistInfo( const char* pSeparator, const char* pFileName );
	void RefreshProfileList();
	void SetCurProfile( int nIndex );
	void GetModuleRoot();
	void CleanupDir();
	void CheckModifyList();
	void GetModifyList();
	BOOL SetWorkspace( LPCTSTR strWorkspace );
	void UpdateLastRevision();
	BOOL CopyBuild( bool bDirectCopy );
	void CreateDeleteList();
	void CreatePatch();
	void CopyServerPatch( bool bDirectCopy );
	void UpdatePatchInfo();

	void BuildFullVersion();
	void BuildVersion1();
	void SetWorkStatus( const char *pString );
	void BuildPatch();
	void CheckPatchFolder();
	BOOL CheckPerforceInfo( int nProfileIndex );

	//롤백
	void RollBackPatch(); 
	void RollBackCheckVersion();
	void RollBackCopyFile();
	void RollCreateDeleteList();
	void RollPacking();
	void RollSaveVersion();
	void LoadPatchInfo();	

	//수동패치 
	void ManualPatch();
	BOOL FileExistName( char *Path );
	bool P4ExecCommand( char *pCommand, std::string *pszOutput );

#ifdef _USE_RTPATCH
	// RTPatch 관련
	void MakeTempFiles();	// modify list에서 삭제되거나 변경된 사항들을 temp폴더에 저장한다.
	void CreateRTPatch( std::vector<std::string>& vecFileList, LPCTSTR strPakFileName, 
						LPCTSTR strNewFilePath, LPCTSTR strOldFilePath, LPCTSTR strMessage );	// RTPatch생성 및 Pak파일로 패킹
#endif // _USE_RTPATCH

	afx_msg void OnBnClickedPatchFolderBrowse();
	afx_msg void OnBnClickedAddProfile();
	afx_msg void OnBnClickedModifyProfile();
	afx_msg void OnBnClickedDeleteProfile();
	afx_msg void OnCbnSelchangeSelectProfile();
	afx_msg void OnCbnSelchangeSelectWorkspace();
	afx_msg void OnBnClickedBuild();
	afx_msg void OnBnClickedRmakeFolderBrowse();
	afx_msg void OnBnClickedFullVersion();
	afx_msg void OnBnClickedCopyServerData();
	afx_msg void OnBnClickedExit();
	afx_msg void OnBnClickedRollback();
protected:
	virtual void OnOK();
public:
	afx_msg void OnBnClickedLauncherFolderBrowse();
	afx_msg void OnBnClickedManualPatch();
	afx_msg void OnBnClickedIdcManualFolderBrowse2();
};
