#pragma once

class CUnionValueProperty;
class CEventSignalItem {
public:
	CEventSignalItem( int nUniqueIndex );
	virtual ~CEventSignalItem();


protected:
	CString m_szName;
	int m_nBackgroundColor;
	int m_nBackgroundSideColor;
	int m_nSelectColor;
	int m_nSelectSideColor;
	int m_nUniqueIndex;
	int m_nParamUniqueIndex;

	std::vector<CUnionValueProperty *> m_pVecList;
	std::vector<int> m_nVecUniqueList;

public:
	void SetName( CString &szStr ) { m_szName = szStr; }
	CString GetName() { return m_szName; }

	void SetBackgroundColor( int nColor ) { m_nBackgroundColor = nColor; }
	int GetBackgroundColor() { return m_nBackgroundColor; }
	void SetBackgroundSideColor( int nColor ) { m_nBackgroundSideColor = nColor; }
	int GetBackgroundSideColor() { return m_nBackgroundSideColor; }
	void SetSelectColor( int nColor ) { m_nSelectColor = nColor; }
	int GetSelectColor() { return m_nSelectColor; }
	void SetSelectSideColor( int nColor ) { m_nSelectSideColor = nColor; }
	int GetSelectSideColor() { return m_nSelectSideColor; }

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
	bool ExportHeader( FILE *fp, int& nUsingBuffCount, int &nUsing64BuffCount );

};