#pragma once

#include "Singleton.h"
#include "DNTableFile.h"

class CDnGameTable : public CSingleton<CDnGameTable>
{
public:
	enum
	{
		WEAPON_TABLE,
		FILE_TABLE
	};

private:
	std::vector<DNTableFileFormat*> m_pVecList;

	void FindExtFileList( const char *szFileName, std::vector<CFileNameString> &szVecList );

public:
	CDnGameTable(void);
	virtual ~CDnGameTable(void);

	DNTableFileFormat* GetTable( int iTableEnum );
};