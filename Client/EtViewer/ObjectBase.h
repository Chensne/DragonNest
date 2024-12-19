#pragma once

#include "Timer.h"
#include "UnionValueProperty.h"
#include "PropertyGridCreator.h"

class CObjectBase {
public:
	CObjectBase();
	virtual ~CObjectBase();

	enum OBJECT_TYPE {
		UNKNOWN			= 0x00,
		SCENE			= 0x01,
		SKIN			= 0x02,
		SUBMESH			= 0x04,
		ANIMATION		= 0x08,
		LIGHTDIR		= 0x10,
		LIGHTPOINT		= 0x20,
		LIGHTSPOT		= 0x40,
		LIGHTFOLDER		= 0x80,
	};

protected:
	OBJECT_TYPE m_Type;
	CString m_szName;

	CObjectBase *m_pParent;
	std::vector<CObjectBase *> m_pVecChild;
//	std::vector<PropertyGridBaseDefine> m_VecPropertyList;
	std::vector<CUnionValueProperty *> m_pVecPropertyList;
	CString m_szModify;

	CTreeCtrl *m_pTreeCtrl;
	HTREEITEM m_ItemID;
	bool m_bShow;
	bool m_bModify;
	bool m_bFocus;

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

	virtual void ExportObject( FILE *fp, int &nCount );
	virtual void ImportObject( FILE *fp );

	OBJECT_TYPE GetType() { return m_Type; }
	void SetType( OBJECT_TYPE Type ) { m_Type = Type; }

	virtual CString GetName() { return m_szName; }
	void SetName( CString &szName ) { m_szName = szName; }

	DWORD GetChildCount() { return (DWORD)m_pVecChild.size(); }
	CObjectBase *GetChild( DWORD dwIndex ) { return m_pVecChild[dwIndex]; }
	CObjectBase *GetChildFromName( CString &szName );

	void AddChild( CObjectBase *pObject );
	void RemoveChild( CObjectBase *pObject );
	void RemoveChild( CString &szName );

	void SetParent( CObjectBase *pParent ) { m_pParent = pParent; }
	CObjectBase *GetParent() { return m_pParent; }

	void SetTreeItemID( HTREEITEM hItem ) { m_ItemID = hItem; }
	HTREEITEM GetTreeItemID() { return m_ItemID; }
	void SetTreeCtrl( CTreeCtrl *pCtrl ) { m_pTreeCtrl = pCtrl; }

	virtual void GetChildFileName( std::vector<CString> &szVecResult );

	virtual void Show( bool bShow );
	bool IsShow() { return m_bShow; }

	void SetModify( bool bModify = true, CString szStr = "" );
	void GetChildModifyString( CString &szBuf, int nTabCount = 0 );
	bool IsChildModify();

	void SetFocus( bool bFocus ) { m_bFocus = bFocus; }
	bool IsFocus() { return m_bFocus; }
	
	std::vector<CUnionValueProperty*> *GetPropertyList() { return &m_pVecPropertyList; }

	static std::vector<CObjectBase *> m_pVecRoot;
};

#include "ObjectScene.h"
#include "ObjectSkin.h"
#include "ObjectSubmesh.h"
#include "ObjectAnimation.h"
#include "ObjectLightDir.h"
#include "ObjectLightPoint.h"
#include "ObjectLightSpot.h"
#include "ObjectLightFolder.h"
