#pragma once


#include "BaseView.h"
#include "OXLayoutManager.h"
#include "OXGridList.h"
#include "OXGridEdit.h"
#include "ServiceInfo.h"


// CBaseInformationView 폼 뷰입니다.

class CBaseInformationView : public CBaseView
{
	DECLARE_DYNCREATE(CBaseInformationView)

public:
	enum EF_SVCINFOTYPE		// 서비스 정보 타입
	{
		EV_SFT_MRT,			// MERIT
		EV_SFT_NLC,			// NET LUNCHER
		EV_SFT_CNT,
	};
	enum EF_UI_SIZE			// UI 크기 정의
	{
		EV_UIS_SVCINFOVIEW_PAD_A			= 10,

		EV_UIS_SVCINFOMRTVIEW_PAD_Y			= 50,
		EV_UIS_SVCINFOMRTVIEW_WTH			= 450,
		EV_UIS_SVCINFOMRTVIEW_HGT			= 200,

		EV_UIS_SVCINFONLCVIEW_PAD_Y			= 50,
		EV_UIS_SVCINFONLCVIEW_WTH			= 240,
//		EV_UIS_SVCINFONLCVIEW_HGT			= 300,

		EV_UIS_TITLELINE_PAD_X				= 10,		// 제목 외부 간격 X
		EV_UIS_TITLELINE_PAD_Y				= 34,		// 제목 외부 간격 Y
		EV_UIS_TITLELINE_HGT				= 18,		// 제목 높이

		EV_UIS_TITLETEXT_PAD_X				= 10,		// 제목 외부 간격 X
	};

public:
	CBaseInformationView();

protected:
//	CBaseInformationView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CBaseInformationView();

public:
	enum { IDD = IDD_BASEINFOFRM };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

private:
	void DrawTitle(CDC* pDC);

public:
	void AddGridList(const struct MERITINFO* pMeritInfo);
	void AddGridList(const struct NETLUNCHERINFO* pNetLuncherInfo);

	void ClearGridList(EF_SVCINFOTYPE pType) { if(0 <= pType && EV_SFT_CNT > pType) m_GridList[pType].DeleteAllItems(); }

private:
	BOOL m_IsInitialized;	// CView 에는 다이얼로그의 WM_INITDIALOG 메시지가 오지 않으므로 OnUpdate() 에서 관리

	CImageList m_ImageList;
	COXLayoutManager m_LayoutManager;
	COXGridList	m_GridList[EV_SFT_CNT];
	COXGridEdit	m_GridEdit[EV_SFT_CNT];

	BOOL m_IsErrorOccured;
	int m_TabIconTick;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
protected:
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
};

