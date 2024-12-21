#pragma once

#include "Singleton.h"
#include "DNTableFile.h"

class CDnCutSceneTable : public CSingleton<CDnCutSceneTable>
{
public:
	enum
	{
		PROP_TABLE,

	};

private:
	vector<DNTableFileFormat* > m_pvlTable;

public:
	CDnCutSceneTable(void);
	virtual ~CDnCutSceneTable(void);

	DNTableFileFormat*  GetTable( int iTableEnum );
};
