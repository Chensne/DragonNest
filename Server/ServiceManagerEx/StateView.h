#pragma once

#include "PropertyGrid.h"

class ItemData;

class CStateView : public CView
{
	enum PropertyItem
	{
		Type,
		Name,
		Id,
		World,
		State,
		Version,
		Final,
		Max
	};

	DECLARE_DYNCREATE(CStateView)

protected:
	CStateView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CStateView();

public:
	virtual void OnDraw(CDC* pDC);      // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual void OnInitialUpdate();

#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);

public:
	void RefreshView();
	void OnSelectedServerItem(const ItemData* pData);

private:
	CPropertyGrid* m_pProperty;
	vector<HITEM> m_propertyItems;
};


