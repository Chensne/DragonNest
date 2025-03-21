#pragma once

#include "DnHtmlView.h"

#ifdef _USE_PARTITION_SELECT

// --- Base Class CDnHtmlViewEx / CWebpage / CDnExplorerDlg ---

// Fit Size 관련 확장 HtmlView Class
class CDnHtmlViewEx : public CDHtmlViewSpec
{
protected: // create from serialization only
	CDnHtmlViewEx();
	DECLARE_DYNCREATE(CDnHtmlViewEx)

	//to support html's elements event
	DECLARE_DHTML_EVENT_MAP()

protected:
	virtual void OnInitialUpdate(); // called first time after construct
	void FitWindowSize();

	// Implementation
public:
	virtual ~CDnHtmlViewEx();
	virtual void OnDocumentComplete( LPCTSTR lpszURL );

protected:
	BOOL m_bDocumentComplete;

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};

// Webpage Class
class CWebPage  
{
public:
	CWebPage();
	virtual ~CWebPage();

	bool SetDocument(IDispatch* pDisp);
	inline LPDISPATCH GetHtmlDocument() const { return m_spDoc; }
	inline const CString GetLastError() const { return m_strError; }
	bool GetJScript(CComPtr<IDispatch>& spDisp);
	bool GetJScripts(CComPtr<IHTMLElementCollection>& spColl);

	CString ScanJScript(CString& strAText, CStringArray& args);

	bool CallJScript(const CString strFunc,CComVariant* pVarResult = NULL);
	bool CallJScript(const CString strFunc,const CString strArg1,CComVariant* pVarResult = NULL);
	bool CallJScript(const CString strFunc,const CString strArg1,const CString strArg2,CComVariant* pVarResult = NULL);
	bool CallJScript(const CString strFunc,const CString strArg1,const CString strArg2,const CString strArg3,CComVariant* pVarResult = NULL);
	bool CallJScript(const CString strFunc,const CStringArray& paramArray,CComVariant* pVarResult = NULL);

protected:
	inline void ShowError(CString lpszText) { m_strError.Format( L"JSCall Error:\n", lpszText ); }

protected:
	CComPtr<IHTMLDocument2>	m_spDoc;
	CString	m_strError;
};

CString GetNextToken( CString& strSrc, const CString strDelim, bool bTrim = false, bool bFindOneOf = true );

// Explorer Dialog Class
class CDnExplorerDlg : public CDialog
{
	DECLARE_DYNAMIC(CDnExplorerDlg)

public:
	CDnExplorerDlg( CWnd* pParent = NULL );   // standard constructor
	virtual ~CDnExplorerDlg();

	// Dialog Data
	enum { IDD = IDD_DLG_EXPLORER };

protected:
	CString			m_strURL;
	CDnHtmlViewEx*	m_pHtmlView;
	CCreateContext	m_pContext;
	CWnd*			m_pFrameWnd;
	RECT			m_Rect;
	BOOL			m_bShowFlag;

	virtual BOOL OnInitDialog();
	BOOL ShowWindowEx( int nCmdShow );

public:
	void SetURL( CString strURL ) { m_strURL = strURL; }
	virtual void OnDocumentComplete();

public:
	afx_msg void OnWindowPosChanging( WINDOWPOS* lpwndpos );
	afx_msg void OnBnClickedCancel();

	DECLARE_MESSAGE_MAP()
};


// --- Child Class ---

#define WM_SELECT_PARTITION					WM_APP+102	// 파티션 선택 윈도우 메세지

// 파티션 선택 Html View
class CDnSelectPartitionHtmlView : public CDnHtmlViewEx
{
protected: // create from serialization only
	CDnSelectPartitionHtmlView();
	DECLARE_DYNCREATE(CDnSelectPartitionHtmlView)

	//to support html's elements event
	DECLARE_DHTML_EVENT_MAP()

	HRESULT OnButtonSelectPartition( IHTMLElement *pElement );
#ifdef _CHN
private:
	int m_nChannelIndex;
	int m_nPartitionIndex;
#endif // _CHN

	// Implementation
public:
	virtual ~CDnSelectPartitionHtmlView();
	virtual void OnDocumentComplete( LPCTSTR lpszURL );
#ifdef _CHN
	void ResetPartitionState();
	CString TestPartitionState( CString strStateTestUrl );
#endif // _CHN

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};

class CDnSelectPartitionDlg : public CDnExplorerDlg
{
	DECLARE_DYNAMIC(CDnSelectPartitionDlg)

public:
	CDnSelectPartitionDlg( CWnd* pParent = NULL );
	virtual ~CDnSelectPartitionDlg();

	enum { IDD = IDD_DLG_EXPLORER };

	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage( MSG* pMsg );

public:
#ifdef _CHN
	afx_msg void OnTimer( UINT_PTR nIDEvent );
#endif // _CHN

	DECLARE_MESSAGE_MAP()
};

#ifdef _CHN
#define WM_CLOSE_LAUNCHER					WM_USER+103	// 런처 종료 메세지

// 중국에서 요청한 점검중인 서버에서 시작 시 웹페이지 안내 다이얼로그 띄우기
class CDnRestrictiveGuideHtmlView : public CDnHtmlViewEx
{
protected: // create from serialization only
	CDnRestrictiveGuideHtmlView();
	DECLARE_DYNCREATE(CDnRestrictiveGuideHtmlView)

	//to support html's elements event
	DECLARE_DHTML_EVENT_MAP()

	HRESULT OnButtonCloseWindow( IHTMLElement *pElement );

	// Implementation
public:
	virtual ~CDnRestrictiveGuideHtmlView();

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};

class CDnRestrictiveGuideDlg : public CDnExplorerDlg
{
	DECLARE_DYNAMIC(CDnRestrictiveGuideDlg)

public:
	CDnRestrictiveGuideDlg( CWnd* pParent = NULL );
	virtual ~CDnRestrictiveGuideDlg();

	enum { IDD = IDD_DLG_EXPLORER };

	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage( MSG* pMsg );

	DECLARE_MESSAGE_MAP()
};
#endif // _CHN

#endif // _USE_PARTITION_SELECT