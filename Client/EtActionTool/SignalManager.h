#pragma once

#include "Singleton.h"
class CSignalItem;
class CSignalManager : public CSingleton<CSignalManager> {
public:
	CSignalManager();
	virtual ~CSignalManager();

	static int s_nUniqueIndex;
protected:
	CString m_szMyName;
	CString m_szWorkingFolder;
	CString m_szFileName;
	std::vector<CSignalItem *> m_pVecList;

	struct DefineStringStruct {
		CString szFileName;
		std::vector<CString> szVecList;
	};
	std::vector<DefineStringStruct> m_VecDefineList;
	std::vector<int> m_vEnableSaveAndPasteSignal;

	void BackupFiles( CString szFileName );
	void LoadDefineList();
	bool ParseDefineString( const char *szFileName, std::vector<CString> &szVecList );

	void ExportEnum( FILE *fp );

public:
	void GetHostName( CString &szHost );
	void GetLocalIP( CString &szIP );

	bool AddSignalItem( CSignalItem *pSignalItem );
	bool RemoveSignalItem( CString &szName );
	DWORD GetSignalItemCount();
	CSignalItem *GetSignalItem( DWORD dwIndex );
	CSignalItem *GetSignalItemFromName( CString &szSignalName );
	CSignalItem *GetSignalItemFromUniqueIndex( int nValue );

	bool IsExistSignalItem( CString &szName, CString &szOriginalName = CString("") );

	bool Save();
	bool Load();
	bool ExportHeader( CString szFileName );

	void ChangeWorkingFolder( CString szPath );
	CString GetWorkingFolder() { return m_szWorkingFolder; }

	int GetDefineCategoryIndex( CString szStr );
	DWORD GetDefineCategoryCount() { return (DWORD)m_VecDefineList.size(); }
	CString GetDefineCategoryName( DWORD dwIndex ) { return m_VecDefineList[dwIndex].szFileName; }
	DWORD GetDefineStringCount( DWORD dwCategory ) { return (DWORD)m_VecDefineList[dwCategory].szVecList.size(); }
	CString GetDefineString( DWORD dwCategory, DWORD dwIndex ) { return m_VecDefineList[dwCategory].szVecList[dwIndex]; }

	bool IsCanCheckOut();
	bool CheckOut();
	bool CheckIn();
	void Refresh();

	void InitEnableSaveAndPasteSignal();
	void SetEnableSaveAndPasteSignal( CSignalItem *pSignalItem );
	bool IsEnableSaveAndPasteSignal( const int nUniqueIndex );
};