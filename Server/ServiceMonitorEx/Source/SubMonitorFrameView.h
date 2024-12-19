#pragma once


#include "OXLayoutManager.h"
#include "CriticalSection.hpp"
#include "ControlDialog.h"
#include "Define.h"
#include <vector>

class CSubMonitorFrameView : public CFormView
{
	DECLARE_DYNCREATE(CSubMonitorFrameView)

public:
	typedef	CCriticalSection				TP_LOCK;
	typedef	CLockAutoEx<TP_LOCK>			TP_LOCKAUTO;
	typedef	std::vector<CHAR>			TP_LISTAUTO;
	typedef	TP_LISTAUTO::iterator			TP_LISTAUTO_ITR;
	typedef	TP_LISTAUTO::const_iterator		TP_LISTAUTO_CTR;

public:
	enum EF_IMAGELISTTYPE	// �̹��� ����Ʈ Ÿ��
	{
		EV_ILT_SMALL,		// SMALL
		EV_ILT_LARGE,		// LARGE
		EV_ILT_CNT,
	};

protected:
	CSubMonitorFrameView();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~CSubMonitorFrameView();

public:
	enum { IDD = IDD_SUBMNTFRM };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

	BOOL RebuildWorldView();

	int GetActivePageIndex() const;
	void SetPageImageIndex(int pPageIndex, int pImageIndex);

	TP_LOCK* GetLock() const { return(&m_Lock); }
#if defined(_DEBUG)
	BOOL IsLock() const { return(m_Lock.IsLock()); }
#endif	// _DEBUG

	void SetBaseInfo(CBasePartitionView* pView);

	CControlDialog& GetControlDlg () {return m_ControlDialog;}

private:
	void AddWorldView(INT nWorldID);
	void ClearAllWorldView();
	
	LONG64 GetServiceInfoUpdateNo() const { return m_ServiceInfoUpdateNo; }
//	LONG64 IncServiceInfoUpdateNo() { return(++m_ServiceInfoUpdateNo); }
	void SetServiceInfoUpdateNo(LONG64 pServiceInfoUpdateNo) { m_ServiceInfoUpdateNo = pServiceInfoUpdateNo; }

private:
	BOOL m_IsInitialized;	// CView ���� ���̾�α��� WM_INITDIALOG �޽����� ���� �����Ƿ� OnUpdate() ���� ����
	BOOL m_IsSized;			// COX3DTabViewContainer ��� ����

	mutable TP_LOCK m_Lock;

	CImageList m_ImageList[EV_ILT_CNT];
	COX3DTabViewContainer m_SubTabView;

	LONG64 m_ServiceInfoUpdateNo;

	BOOL m_PlayAlert;

	HACCEL m_hAccel;

	CServiceInfo* m_pServiceInfo;
	CBasePartitionView* m_pPartitionView;
	CControlDialog m_ControlDialog;


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnPickAllCurViewServer();
	afx_msg void OnRefsCurWorldView();
};

