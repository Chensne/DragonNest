#pragma once

#include "DNFieldData.h"

typedef map<string, CDNFieldData*> TMapFieldDatas;

class CDNFieldDataManager
{
private:
	TMapFieldDatas m_FieldDatas;

public:
	CDNFieldDataManager(void);
	virtual ~CDNFieldDataManager(void);

	CDNFieldData* AddFieldData(char *pszMapName, int nMapIndex);
	bool DelFieldData(char *pszMapName);

	CDNFieldData* FindFieldData(char *pszMapName);
	CDNFieldData* FindFieldDataByMapIndex(int nMapIndex);

	char GetRandomGateNo(int nMapIndex);

	int CheckGateArea(int nMapIndex, TPosition &Position);
	bool GetStartPosition(int nMapIndex, char cGateNo, TPosition &Pos);
	void GetRandomStartPosition(int nMapIndex, TPosition &Pos);
	void GetStartPositionAngle( int nMapIndex, char cGateNo, float &fAngle );
	int GetGateType(int nMapIndex, TPosition &Position);
};

extern CDNFieldDataManager* g_pFieldDataManager;