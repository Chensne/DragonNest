// EtUIToolDoc.h : interface of the CEtUIToolDoc class
//


#pragma once


class CEtUIToolDoc : public CDocument
{
protected: // create from serialization only
	CEtUIToolDoc();
	DECLARE_DYNCREATE(CEtUIToolDoc)

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
	virtual ~CEtUIToolDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};


