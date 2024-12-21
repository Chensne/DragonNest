// EtEffect2ToolDoc.h : CEtEffect2ToolDoc 클래스의 인터페이스
//


#pragma once


class CEtEffect2ToolDoc : public CDocument
{
protected: // serialization에서만 만들어집니다.
	CEtEffect2ToolDoc();
	DECLARE_DYNCREATE(CEtEffect2ToolDoc)

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
	virtual ~CEtEffect2ToolDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CString m_currentFile;

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnFileOpen();
	/*afx_msg void OnPrev();
	afx_msg void OnPlay();*/
	afx_msg void OnPlay();
	afx_msg void OnPrev();
	afx_msg void OnNext();
	afx_msg void OnFileSave();
	afx_msg void OnParticleFileSave();
	afx_msg void OnFileNew();
	afx_msg void OnGridToggle();
	afx_msg void OnPlayPlayLoop();
	afx_msg void OnOptionUp();
	afx_msg void OnOptionDown();
};


