// PatchBuilderDlg.h : 헤더 파일
//

#pragma once
#include "afxwin.h"

class CDnAttachFile;

// CPatchBuilderDlg 대화 상자
class CPatchBuilderDlg : public CDialog
{
// 생성입니다.
public:
	CPatchBuilderDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_PATCHBUILDER_DIALOG };

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
	std::vector<std::string> m_vecModifyFile;
	std::vector<std::string> m_vecDeleteFile;

	std::vector<std::string> m_vecRollDeleteFile;	//삭제된 데이터
	std::vector<std::string> m_vecRestoreFile;		//복구할 데이터

	std::map<std::string, std::string> m_mapIgnoreFileList;
	std::vector<std::string> m_vecIncludeMapList;
	
	char m_szModuleRoot[MAX_PATH];
	
	CString m_szSVNUrl;
	CString m_szSVNUrlBuildSet;
	CString m_szSVNUrlServer;
	CString m_szSVNUpdateFolder;
	CString m_szPatchFolder;
	CString m_szRMakeCmd;
	CString m_szCountryCode;
	CComboBox m_SelectProfile;
	CString m_szRollTempFolder;
	CString m_szRollPatchFoloer;
	CString m_szManualPatchExe;

	int m_nDefaultProfile;
	int m_nCurrentVersion;
	int m_nLastRevision;
	int m_nStartRevision;

	int m_nBuildSetLastRevision;
	int m_nBuildSetStartRevision;

	BOOL m_bManualTargetVer;
	int m_nManualPatchTargetStartVer;
	int m_nManualPatchTargetEndVer;

	CString m_szMainResourceFolder;
	CString m_szMainBuildSetFolder;

	BOOL m_bMakeFullVersion;
	BOOL m_bCopyServerData;
	BOOL m_bAutoStart;
	BOOL m_bRollBack;
	
	CDnAttachFile *m_pDnAttachFile;
	BOOL m_bManualPatch;
	
	std::vector<int> m_SkipNumber;
	
	void RefreshProfileList();
	void SetCurProfile( int nIndex );
	void GetModuleRoot();
	void CleanupDir();
	void CheckModifyList();
	void GetModifyList();
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
	BOOL CheckSVNInfo( int nProfileIndex );
	void CheckPatchFolder();
	
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

	BOOL FileExistName(char *Path);

	void CopyBuildSetToClient();
	void ApplyPatchResourceToClientFolder();
	void LoadFilteringCfg();
	void ClassifyModifyList( std::string& szOutput, CString& szMainFolder );
	void FilteringModifyList( std::vector<std::string>& vecFileList );
	CString RemoveCountryPath( const char* szPath );
	void SaveListLog( LPCTSTR strFileName );

	afx_msg void OnBnClickedPatchFolderBrowse();
	afx_msg void OnBnClickedAddProfile();
	afx_msg void OnBnClickedModifyProfile();
	afx_msg void OnBnClickedDeleteProfile();
	afx_msg void OnCbnSelchangeSelectProfile();
	afx_msg void OnBnClickedBuild();
	afx_msg void OnBnClickedRmakeFolderBrowse();
	afx_msg void OnBnClickedUpdateFolderBrowse();
	afx_msg void OnBnClickedFullVersion();
	afx_msg void OnBnClickedCopyServerData();
	afx_msg void OnBnClickedExit();
	afx_msg void OnBnClickedRollback();
protected:
	virtual void OnOK();
public:
	afx_msg void OnBnClickedManualPatch();
	afx_msg void OnBnClickedIdcManualFolderBrowse2();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedCheckManualTargetVer();
};
