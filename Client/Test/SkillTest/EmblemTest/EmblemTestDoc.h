// EmblemTestDoc.h : interface of the CEmblemTestDoc class
//


#pragma once


class CEmblemTestDoc : public CDocument
{
protected: // create from serialization only
	CEmblemTestDoc();
	DECLARE_DYNCREATE(CEmblemTestDoc)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CEmblemTestDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};


