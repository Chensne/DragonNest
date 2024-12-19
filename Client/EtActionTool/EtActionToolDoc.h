// EtActionToolDoc.h : CEtActionToolDoc 클래스의 인터페이스
//


#pragma once


class CEtActionToolDoc : public CDocument
{
protected: // serialization에서만 만들어집니다.
	CEtActionToolDoc();
	DECLARE_DYNCREATE(CEtActionToolDoc)

// 특성입니다.
public:

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// 구현입니다.
public:
	virtual ~CEtActionToolDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnFileOpen();
	afx_msg void OnFileSave();
	afx_msg void OnFileNew();
	afx_msg void OnSignalReport();
	afx_msg void OnSaveSignalReport();
	afx_msg void OnUpdateSignalReport(CCmdUI *pCmdUI);
	afx_msg void OnUpdateSaveSignalReport(CCmdUI *pCmdUI);

	afx_msg void OnFileExport();
	afx_msg void OnFileImport();
	afx_msg void OnUpdateOnFileImport(CCmdUI *pCmdUI);
	afx_msg void OnUpdateOnFileExport(CCmdUI *pCmdUI);
	afx_msg void OnCheckWalkfront();
	

};


