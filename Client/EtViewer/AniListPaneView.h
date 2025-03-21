#pragma once
#include "afxwin.h"
#include "afxcmn.h"



// CAniListPaneView 폼 뷰입니다.

class CObjectAnimation;
class CAniListPaneView : public CFormView
{
	DECLARE_DYNCREATE(CAniListPaneView)

protected:
	CAniListPaneView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CAniListPaneView();

	bool m_bActivate;
	CObjectAnimation *m_pObject;
	
	void EnableControl( bool bEnable );
	int GetLoopCount();
public:
	enum { IDD = IDD_ANILISTPANEVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();

	CXTButton m_Rewind;
	CXTButton m_Prev;
	CXTButton m_Play;
	CXTButton m_Pause;
	CXTButton m_Next;
	CXTButton m_Forward;
	CXTButton m_Loop;
	CXTButton m_NoLoop;
	CStatic m_StaticFrame;
	CEdit m_EditFps;
	CSliderCtrl m_Slider;
	int m_nEditFPS;

	afx_msg LRESULT OnRefresh( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnUpdateFrame( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnAccelKey( WPARAM wParam, LPARAM lParam );
	afx_msg void OnBnClickedButtonRewind();
	afx_msg void OnBnClickedButtonPrev();
	afx_msg void OnBnClickedButtonPlay();
	afx_msg void OnBnClickedButtonNext();
	afx_msg void OnBnClickedButtonForward();
	afx_msg void OnBnClickedButtonLoop();
	afx_msg void OnBnClickedButtonNoLoop();
	afx_msg void OnBnClickedButtonPause();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnEnChangeEdit1();
};


