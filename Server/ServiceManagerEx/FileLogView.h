#pragma once

#include "LogInfo.h"

// CFileLogView 뷰입니다.

class CLogList;

class CFileLogView : public CScrollView
{
	DECLARE_DYNCREATE(CFileLogView)

public:
	CFileLogView();
	virtual ~CFileLogView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void OnDraw(CDC* pDC);      // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual void OnInitialUpdate();     // 생성된 후 처음입니다.

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);

public:
	bool OpenFile();
	void InsertLog(LogInfo& log);

private:
	CLogList* m_pList;	
};


