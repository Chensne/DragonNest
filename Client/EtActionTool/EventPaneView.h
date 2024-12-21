#pragma once


#define _SELECT_MULTIPLE_SIGNAL

// CEventPaneView 폼 뷰입니다.
#include "SequenceView.h"
#include "ActionSignal.h"
#ifdef _SELECT_MULTIPLE_SIGNAL
#include "./boost/shared_ptr.hpp"
#endif

#define _CHECK_CUSTOMFLY


class CActionBase;
class CActionElement;
class CActionSignal;
class CEventGridItemBase;
#ifdef _SELECT_MULTIPLE_SIGNAL
class CUnionValueProperty;
#endif
class CEventPaneView : public CSequenceView
{
	DECLARE_DYNCREATE(CEventPaneView)

protected:
	CEventPaneView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CEventPaneView();

public:
	enum { IDD = IDD_EVENTPANEVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
public:
#ifdef _SELECT_MULTIPLE_SIGNAL
	struct SCopyCutGridItemInfo
	{
		int nStartGridX;
		int	nEndGridX;
		int nYOrder;
		CActionSignal* pSignal;

		SCopyCutGridItemInfo() : nStartGridX(0), nEndGridX(0), nYOrder(0), pSignal(NULL) {}
		bool operator==(const SCopyCutGridItemInfo& rhs) const
		{
			return ((nStartGridX == rhs.nStartGridX) && (nEndGridX == rhs.nEndGridX) && (nYOrder == rhs.nYOrder));
		}
		bool operator==(const CEventGridItemBase& rhs) const;
	};

	struct SCopyCutInfo
	{
	public:
		SCopyCutInfo() : wholeArea(-1, -1, -1, -1) {}
		BOOL IsEmpty() const { return gridItemList.empty(); }
		void Clear() { wholeArea.SetRect(-1, -1, -1, -1); gridItemList.clear(); }
		int  GetBufferSize() const { return (int)gridItemList.size(); }
		BOOL Push( SCopyCutGridItemInfo * pAddItem );
		BOOL Push(CEventGridItemBase* pAddItem);
		BOOL Erase(const CEventGridItemBase* pEraseItemIter);

		std::list<SCopyCutGridItemInfo> & GetGridItemList();
		const SCopyCutGridItemInfo&	GetMostLeftGridItem() const;
		const SCopyCutGridItemInfo&	GetMostTopGridItem() const;

	public:
		CRect wholeArea;

	private:
		std::list<SCopyCutGridItemInfo> gridItemList;
	};
#endif

	virtual CSequenceSignalBase *CreateSignal();
	virtual bool InsertSignal( CSequenceSignalBase *pItem );
	virtual void OnSelectSignal( CSequenceSignalBase *pItem );
	virtual bool OnRemoveSignal( CSequenceSignalBase *pItem );
	virtual void OnModifySignal( CSequenceSignalBase *pItem );
#ifdef _SELECT_MULTIPLE_SIGNAL
	void ResetPrevSelectItemSignal();
	virtual void OnSelectSignalArray( std::vector<CSequenceSignalBase *>& pItemArray );
	virtual void OnClearSelectedSignals();
	BOOL IsExistCopyBuffer() const { return (m_CopyCutBuffer.IsEmpty() == FALSE); }
	BOOL PasteSignal(const int& nGridStart, const int& nGridEnd, const int& nYOrder, const int& signalIndex, const std::vector<CUnionValueProperty *>* pPropertyList);
	void PasteCopyCutBuffer();
#else
	bool IsExistCopyBuffer() { return (m_pCopyCutBuffer != NULL); }
#endif
	void SetContextMousePos( POINT &pt ) { m_ContextMousePos = pt; }

	void ChangeValueSelectedArraySignal( CUnionValueProperty *pVariable, DWORD dwIndex );


	// ---- 시그널 길이변경은 하지않도록 한다. 
	// 이유는 시그널이 종료된 이후에도 카메라는 keyframe 만큼 계속 적용될 것이고,
	// 카메라 Keyframe 이 Element의 최대길이를 넘어가는 경우가 많기 때문이다.	
	// bintitle - STE_CameraEffect_KeyFrame.
	// .cam 데이타를 읽어들여 카메라KeyFrame길이만큼 Signal의 EndFrame 을 설정한다.
	//void ChangeValueSelectedSignal( CUnionValueProperty *pVariable, DWORD dwIndex );
	// ----

	void ExportSignal( const char *szFileName );
	void ImportSignal( const char *szFileName );

protected:
	bool m_bActivate;
	CActionBase *m_pObject;
	CMenu *m_pContextMenu;
#ifdef _SELECT_MULTIPLE_SIGNAL
	std::vector<CEventGridItemBase*> m_pPrevSelectItems;
	SCopyCutInfo m_CopyCutBuffer;
#else
	CEventGridItemBase *m_pPrevSelectItem;
	CActionSignal *m_pCopyCutBuffer;
#endif
	CPoint m_ContextMousePos;
	CString m_CopyAndPastePath;

#ifdef _SELECT_MULTIPLE_SIGNAL
	BOOL IsInPrevSelectItems(const CEventGridItemBase* pCurItem) const;
	void PushBackPrevSelectItems(CEventGridItemBase* pItem);
	void RemovePrevSelectItem(CEventGridItemBase* pItem);
	BOOL OnRemoveSignal(CEventGridItemBase* pItem);
	void CopySignalFromPrevSelectItem();
#endif

	bool IsSelectedArrayEqualValue();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual bool IsValidPropertyPaneState() const;

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnRefresh( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnUpdateFrame( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnResetCopyCut( WPARAM wParam, LPARAM lParam );
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnEventpanecontextInsertsignal();
	afx_msg void OnEventpanecontextRemovesignal();
	afx_msg void OnEventpanecontextCopysignal();
	afx_msg void OnEventpanecontextCutsignal();
	afx_msg void OnEventpanecontextPaste();
	afx_msg void OnEventpanecontextExportSignal();
	afx_msg void OnEventpanecontextImportSignal();
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnEventpanecontextFullrange();

#ifdef _EXPORT_EXCEL_ACTION_SIGNAL
	afx_msg void OnEventpanecontextExportexcel();
#endif 
};

class CEventGridItemBase : public CSequenceView::CSequenceSignalBase {
public:
	CEventGridItemBase();
	virtual ~CEventGridItemBase();

	CActionSignal *pSignal;

};

