// NetTestDoc.h : CNetTestDoc Ŭ������ �������̽�
//


#pragma once


class CNetTestDoc : public CDocument
{
protected: // serialization������ ��������ϴ�.
	CNetTestDoc();
	DECLARE_DYNCREATE(CNetTestDoc)

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
	virtual ~CNetTestDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ������ �޽��� �� �Լ�
protected:
	DECLARE_MESSAGE_MAP()
};


