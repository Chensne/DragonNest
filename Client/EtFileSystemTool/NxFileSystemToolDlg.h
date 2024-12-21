// NxFileSystemToolDlg.h : ��� ����
//

#pragma once
#include "FileIOThread.h"

struct SNxFileInfo
{
	char szFileName[ _MAX_PATH ];
	int nFileType;
	DWORD dwCompressSize;
	DWORD dwOriginalSize;
};

// CNxFileSystemToolDlg ��ȭ ����
class CNxFileSystemToolDlg : public CDialog
{
// ����
public:
	CNxFileSystemToolDlg(CWnd* pParent = NULL);	// ǥ�� ������

	// ��ȭ ���� ������
	enum { IDD = IDD_NxFileSystemTool_DIALOG };
protected:
	virtual void		DoDataExchange(CDataExchange* pDX);	// DDX/DDV ����
// ����
protected:
	HICON m_hIcon;

	// �޽��� �� �Լ��� �����߽��ϴ�.
	virtual BOOL		OnInitDialog();
	afx_msg void		OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void		OnPaint();
	afx_msg HCURSOR		OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL		PreTranslateMessage(MSG* pMsg);
	afx_msg void		OnSize(UINT nType, int cx, int cy);
	afx_msg void		OnLoadsystem();
	afx_msg void		OnMenuDivPacking();
	afx_msg void		OnNewsystem();
public:
			void		UpdateFileList();
	afx_msg void		OnFilesystemClose();
	virtual BOOL		DestroyWindow();
	afx_msg void		OnInsertFile();
	afx_msg LRESULT		OnCompleteMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT		OnComplete_Optimize(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT		OnComplete_Export(WPARAM wParam, LPARAM lParam);
	afx_msg void		OnNewFolder();
	afx_msg void		OnRemove();
	afx_msg void		OnNMDblclkFileList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void		OnLvnEndlabeleditFileList(NMHDR *pNMHDR, LRESULT *pResult);

    		void		DropFile(HDROP hDrop);
			BOOL		DropFolder(const TCHAR* strPath);
	afx_msg void		OnIpacExit();
	afx_msg void		OnOptimize();
	afx_msg void		OnRename();
	afx_msg void		OnExport();
			void		FolderExport(tstring FolderName);
	afx_msg void		OnAbout();
	afx_msg void		OnBnClickedButton1();

	void SearchFolderAndAddFile( const char *szFolderName );
	std::string			m_strRootFolder;

public:

	CEdit				m_Edit;
	bool				m_bWorking;
	CString				m_strPath;
	CString				m_CurDirectory;
	CImageList			m_ImageList;
	CListCtrl			m_FileList;
	int					m_iOverwriteType;
	std::vector< SNxFileInfo > m_vecFileInfo;

	afx_msg void OnMenuDivPackingCapacity();
	afx_msg void OnPatch();
	afx_msg LRESULT		OnCompletePatch(WPARAM wParam, LPARAM lParam);
	afx_msg void OnCompare();
	afx_msg void OnInsertFolder();
};
