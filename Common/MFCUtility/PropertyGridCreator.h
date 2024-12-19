#pragma once
#include "UnionValueProperty.h"

struct PropertyGridBaseDefine 
{
	char *szCategory;
	char *szStr;
	CUnionValueProperty::Type Type;
	char *szDescription;
	bool bEnable;
	bool bCustomDialog;
};


class CPropertyGridImp {
public:
	CPropertyGridImp();
	CPropertyGridImp( PropertyGridBaseDefine *pDefineList );
	CPropertyGridImp( std::vector<CUnionValueProperty *> *pVecList );
	virtual ~CPropertyGridImp();

protected:
	CXTPPropertyGrid m_PropertyGrid;
	PropertyGridBaseDefine *m_pDefineList;
	std::vector<CXTPPropertyGridItem *> m_pVecItem;
	std::vector<CUnionValueProperty *> m_pVecVariable;
	std::vector<CUnionValueProperty *> *m_ppVecVariable;
	bool m_bSelfAlloc;
	bool m_bUpdateItem;

	std::string m_szInitDirectory;

	struct CategoryStruct {
		CString szStr;
		CXTPPropertyGridItem *pItem;
	};


	virtual CXTPPropertyGridItem *CreateItem( CUnionValueProperty *pVariable );
	virtual CUnionValueProperty *CreateVariable( PropertyGridBaseDefine *pDefine );
	virtual void ResetPropertyGrid();


	virtual void OnSetValue( CUnionValueProperty *pVariable, DWORD dwIndex ) {}
	virtual void OnChangeValue( CUnionValueProperty *pVariable, DWORD dwIndex ) {} 
	virtual void OnSelectChangeValue( CUnionValueProperty *pVariable, DWORD dwIndex ) {}
	virtual void OnCustomDialogValue( CUnionValueProperty *pVariable, DWORD dwIndex ) {}
public:
	bool RefreshPropertyGrid( PropertyGridBaseDefine *pDefineList = NULL, int nCount = -1 );
	bool RefreshPropertyGrid( std::vector<CUnionValueProperty *> *pVecList = NULL );

	afx_msg LRESULT ProcessNotifyGrid( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT ProcessCustomDialogGrid( WPARAM wParam, LPARAM lParam );

	void ModifyItem( WPARAM wParam, LPARAM lParam );
	void UpdateItem();

	void SetReadOnly( DWORD dwIndex, bool bReadonly );
	void SetReadOnly( CUnionValueProperty *pVariable, bool bReadonly );

	void RefreshPropertyGridVariable();

	void SetInitDirectory( const char * szInitDirectory );
};

