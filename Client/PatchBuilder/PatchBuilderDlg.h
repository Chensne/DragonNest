#pragma once
#include "afxwin.h"

class CDnAttachFile;

// CPatchBuilderDlg 대화 상자
class CPatchBuilderDlg : public CDialog
{
public:
	CPatchBuilderDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

	enum { IDD = IDD_PATCHBUILDER_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();

public:
	void RefreshProfileList();
	void SetCurProfile( int nIndex );
	void GetModuleRoot();
	void CleanupDir();
	void CheckModifyList();
	void GetModifyList();
	void SaveListLog( LPCTSTR strFileName );
	void SVNCleanUp();
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
	void RollBackRTPatch();
	void RollBackCheckVersion();
	void RollBackCopyFile();
	void RollCreateDeleteList();
	void RollPacking();
	void RollSaveVersion();
	void LoadPatchInfo();	

	//수동패치 
	void ManualPatch();
	BOOL FileExistName(char *Path);

	// RTPatch 관련
	void MakeTempFiles();	// modify list에서 삭제되거나 변경된 사항들을 temp폴더에 저장한다.
	void CreateRTPatch( std::vector<std::string>& vecFileList, LPCTSTR strPakFileName, 
		LPCTSTR strNewFilePath, LPCTSTR strOldFilePath, LPCTSTR strMessage );	// RTPatch생성 및 Pak파일로 패킹

public:
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
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
	afx_msg void OnBnClickedLauncherFolderBrowse();
	afx_msg void OnBnClickedManualPatch();
	afx_msg void OnBnClickedIdcManualFolderBrowse2();
	afx_msg void OnBnClickedCheckManualTargetVer();

	DECLARE_MESSAGE_MAP()

protected:
	HICON m_hIcon;

public:
	std::vector< std::string > m_vecModifyFile;
	std::vector< char > m_vecAction;
	std::vector< std::string > m_vecDeleteFile;

	std::vector< std::string > m_vecRealAddFile;	//추가된 데이터
	//RealModify 와 RealDeleteFile 복구해야할 파일이다.
	std::vector< std::string > m_vecRollDeleteFile;	//삭제된 데이터
	std::vector< std::string > m_vecRestoreFile;	//복구할 데이터
	
	char m_szModuleRoot[MAX_PATH];
	
	CString m_szSVNUrl;
	CString m_szSVNUpdateFolder;
	CString m_szPatchFolder;
	CString m_szRMakeCmd;
	CString m_szCountryCode;
	CComboBox m_SelectProfile;
	CString m_szSVNUrlServer;
	CString m_szRollTempFolder;
	CString m_szRollPatchFolder;
	CString m_szManualPatchExe;

	int m_nDefaultProfile;
	int m_nCurrentVersion;
	int m_nLastRevision;
	int m_nStartRevision;
	CString m_szMainFolder;

	BOOL m_bMakeFullVersion;
	BOOL m_bCopyServerData;
	BOOL m_bAutoStart;
	BOOL m_bRollBack;
	
	CDnAttachFile *m_pDnAttachFile;
	BOOL m_bManualPatch;

	BOOL m_bManualTargetVer;
	int m_nManualPatchTargetStartVer;
	int m_nManualPatchTargetEndVer;

	std::vector<int> m_SkipNumber;

	BOOL m_bUseRTPatch;
};
