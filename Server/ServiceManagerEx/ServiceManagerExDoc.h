// ServiceManagerExDoc.h : CServiceManagerExDoc Ŭ������ �������̽�
//
#pragma once

class CServiceManagerExDoc : public CDocument
{
protected: // serialization������ ��������ϴ�.
	CServiceManagerExDoc();
	DECLARE_DYNCREATE(CServiceManagerExDoc)

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
	virtual ~CServiceManagerExDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ������ �޽��� �� �Լ�
protected:
	DECLARE_MESSAGE_MAP()
};


