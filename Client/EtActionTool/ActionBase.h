#pragma once

#include "Timer.h"
#include "UnionValueProperty.h"
#include "PropertyGridCreator.h"

class CActionBase {
public:
	CActionBase();
	virtual ~CActionBase();

	enum OBJECT_TYPE {
		UNKNOWN			= 0x00,
		OBJECT			= 0x01,
		ELEMENT			= 0x02,
		SIGNAL			= 0x04,
		LIGHTDIR		= 0x08,
		LIGHTPOINT		= 0x10,
		LIGHTSPOT		= 0x20,
		LIGHTFOLDER		= 0x40,
	};

protected:
	OBJECT_TYPE m_Type;
	CString m_szName;

	CActionBase *m_pParent;
	std::vector<CActionBase *> m_pVecChild;
	std::vector<CUnionValueProperty *> m_pVecPropertyList;
	CString m_szModify;

	CTreeCtrl *m_pTreeCtrl;
	HTREEITEM m_ItemID;
	bool m_bModify;
	bool m_bFocus;
	bool m_bShow;

	void AddPropertyInfo( PropertyGridBaseDefine Define[] );
public:
	virtual void Activate() { InitPropertyInfo(); }
	virtual void Process( LOCAL_TIME LocalTime );
	virtual void InitPropertyInfo() {}
	virtual void OnSetPropertyValue( DWORD dwIndex, CUnionValueProperty *pVariable ) {}
	virtual void OnChangePropertyValue( DWORD dwIndex, CUnionValueProperty *pVariable ) {}
	virtual void OnSelectChangePropertyValue( DWORD dwIndex, CUnionValueProperty *pVariable ) {}
	virtual void OnInputReceive( int nReceiverState, LOCAL_TIME LocalTime ) {}
	virtual CString GetClassName() { return CString("BaseObject"); }
	
	virtual bool ExportObject( FILE *fp, int &nCount );
	virtual bool ImportObject( FILE *fp );

	OBJECT_TYPE GetType() { return m_Type; }
	void SetType( OBJECT_TYPE Type ) { m_Type = Type; }

	virtual CString GetName() { return m_szName; }
	void SetName( CString &szName ) { m_szName = szName; }

	DWORD GetChildCount() { return (DWORD)m_pVecChild.size(); }
	CActionBase *GetChild( DWORD dwIndex ) { return m_pVecChild[dwIndex]; }
	CActionBase *GetChildFromName( CString &szName );

	void AddChild( CActionBase *pObject );
	void RemoveChild( CActionBase *pObject );
	void RemoveChild( CString &szName );

	void SetParent( CActionBase *pParent ) { m_pParent = pParent; }
	CActionBase *GetParent() { return m_pParent; }

	void SetTreeItemID( HTREEITEM hItem ) { m_ItemID = hItem; }
	HTREEITEM GetTreeItemID() { return m_ItemID; }
	void SetTreeCtrl( CTreeCtrl *pCtrl ) { m_pTreeCtrl = pCtrl; }

	void SetModify( bool bModify = true, CString szStr = "" );
	void GetChildModifyString( CString &szBuf, int nTabCount = 0 );
	bool IsChildModify();

	virtual void SetFocus( bool bFocus ) { m_bFocus = bFocus; }
	bool IsFocus() { return m_bFocus; }

	virtual void Show( bool bShow ) { m_bShow = bShow; }
	bool IsShow() { return m_bShow; }
	

	std::vector<CUnionValueProperty*> *GetPropertyList() { return &m_pVecPropertyList; }
	const std::vector<CUnionValueProperty*> *GetPropertyList() const { return &m_pVecPropertyList; }
	CUnionValueProperty *GetProperty( DWORD dwIndex ) { return m_pVecPropertyList[dwIndex]; }

	static std::vector<CActionBase *> m_pVecRoot;

	CActionBase &operator = ( CActionBase &e );
};