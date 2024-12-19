// EtEffect2ToolDoc.h : CEtEffect2ToolDoc Ŭ������ �������̽�
//


#pragma once


class CEtEffect2ToolDoc : public CDocument
{
protected: // serialization������ ��������ϴ�.
	CEtEffect2ToolDoc();
	DECLARE_DYNCREATE(CEtEffect2ToolDoc)

// Ư���Դϴ�.
public:

// �۾��Դϴ�.
public:

// �������Դϴ�.
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// �����Դϴ�.
public:
	virtual ~CEtEffect2ToolDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CString m_currentFile;

// ������ �޽��� �� �Լ�
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


