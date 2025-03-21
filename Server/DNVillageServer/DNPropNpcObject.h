#pragma once

#include "DNNpcObject.h"
class CEtWorldProp;
class CDNPropNpcObject : public CDNNpcObject
{
public:
	CDNPropNpcObject(void);
	virtual ~CDNPropNpcObject(void);

protected:
	int m_nPropCreateUniqueID;

public:
	bool CreateNpc( TNpcData *pNpcData, UINT nUID, int nChannelID, int nMapIndex, CEtWorldProp *pProp );

	int GetPropCreateUniqueID() { return m_nPropCreateUniqueID; }
};