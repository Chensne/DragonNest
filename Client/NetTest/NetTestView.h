// NetTestView.h : CNetTestView 클래스의 인터페이스
//


#pragma once

#include "VillageClient.h"
#include "GameClient.h"
#include "afxwin.h"
#include "SingleLineEdit.h"

class CNetTestView : public CFormView
{
protected: // serialization에서만 만들어집니다.
	CNetTestView();
	DECLARE_DYNCREATE(CNetTestView)

public:
	enum{ IDD = IDD_NETTEST_FORM };

// 특성입니다.
public:
	CNetTestDoc* GetDocument() const;

// 작업입니다.
public:
	void Connect( CString &szAddress, int nPort, CString &szID, CString &szPassword );
	void IdleProcess();

// 재정의입니다.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual void OnInitialUpdate(); // 생성 후 처음 호출되었습니다.

// 구현입니다.
public:
	virtual ~CNetTestView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
//	CVillageClient m_VillageSession;
	CGameClient m_Session;
	char m_szAddress[ 1024 ];
	int m_nPort;

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnConnectAddress();
	CSingleLineEdit m_ChatLine;
	afx_msg LRESULT OnEditChange( WPARAM wParam, LPARAM lParam );
	CEdit m_ChatWindow;
};

#ifndef _DEBUG  // NetTestView.cpp의 디버그 버전
inline CNetTestDoc* CNetTestView::GetDocument() const
   { return reinterpret_cast<CNetTestDoc*>(m_pDocument); }
#endif

