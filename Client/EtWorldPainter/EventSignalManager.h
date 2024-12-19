#pragma once

#include "Singleton.h"
class CEventSignalItem;
class CEventSignalManager : public CSingleton<CEventSignalManager> {
public:
	CEventSignalManager();
	virtual ~CEventSignalManager();

	static int s_nUniqueIndex;
protected:
	CString m_szMyName;
	CString m_szWorkingFolder;
	CString m_szFileName;
	std::vector<CEventSignalItem *> m_pVecList;

	void BackupFiles( CString szFileName );
	void ExportEnum( FILE *fp );

public:
	void GetHostName( CString &szHost );
	void GetLocalIP( CString &szIP );

	bool AddSignalItem( CEventSignalItem *pSignalItem );
	bool RemoveSignalItem( CString &szName );
	DWORD GetSignalItemCount();
	CEventSignalItem *GetSignalItem( DWORD dwIndex );
	CEventSignalItem *GetSignalItemFromName( CString &szSignalName );
	CEventSignalItem *GetSignalItemFromUniqueIndex( int nValue );

	bool IsExistSignalItem( CString &szName, CString &szOriginalName = CString("") );

	bool Save();
	bool Load();
	bool ExportHeader( CString szFileName );

	void ChangeWorkingFolder( CString szPath );
	CString GetWorkingFolder() { return m_szWorkingFolder; }

	bool IsCanCheckOut();
	bool CheckOut();
	bool CheckIn();

};