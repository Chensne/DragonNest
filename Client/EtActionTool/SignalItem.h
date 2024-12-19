#pragma once

#include "../MFCUtility/Common.h"
class CUnionValueProperty;
class CSignalItem {
public:
	CSignalItem( int nUniqueIndex );
	virtual ~CSignalItem();


protected:
	CString m_szName;
	int m_nInitialsColor;
	int m_nBackgroundColor;
	bool m_bHasLength;
	int m_nUniqueIndex;
	int m_nParamUniqueIndex;
	bool m_bEnableCopyAndPaste;

//	irange m_Range;
	std::vector<CUnionValueProperty *> m_pVecList;
	std::vector<int> m_nVecUniqueList;

public:
	void SetName( CString &szStr ) { m_szName = szStr; }
	CString GetName() { return m_szName; }
	void SetInitialsColor( int nColor ) { m_nInitialsColor = nColor; }
	int GetInitialsColor() { return m_nInitialsColor; }
	void SetBackgroundColor( int nColor ) { m_nBackgroundColor = nColor; }
	int GetBackgroundColor() { return m_nBackgroundColor; }
	void SetHasLength( bool bHasLength ) { m_bHasLength = bHasLength; }
	bool IsHasLength() { return m_bHasLength; }

	int GetUniqueIndex() { return m_nUniqueIndex; }
	void SetUniqueIndex( int nValue ) { m_nUniqueIndex = nValue; }

	bool IsExistParameter( CString &szStr, CString &szOriginalName = CString("") );

	bool AddParameter( CUnionValueProperty *pVariable, bool bAllowUnique );
	void RemoveParameter( CString &szStr );
	DWORD GetParameterCount() { return (DWORD)m_pVecList.size(); }
	CUnionValueProperty *GetParameter( DWORD dwIndex ) { 
		if( dwIndex < 0 || dwIndex >= m_pVecList.size() ) return NULL;
		return m_pVecList[dwIndex]; 
	}
	CUnionValueProperty *GetParameterFromName( CString &szName );
	int GetParameterUniqueIndex( DWORD dwIndex ) { return m_nVecUniqueList[dwIndex]; }

	bool Save( FILE *fp );
	bool Load( FILE *fp );
	bool ExportHeader( FILE *fp, int& nUsingBuffCount, int& nUsingBuffCount64 );

	bool IsEnableCopyAndPaste() { return m_bEnableCopyAndPaste; }
	void SetEnableCopyAndPaste( bool bEnableCopyAndPaste ) { m_bEnableCopyAndPaste = bEnableCopyAndPaste; }
};