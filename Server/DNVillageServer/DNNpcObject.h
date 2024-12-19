#pragma once

#include "DNBaseObject.h"
#include "DNNpc.h"

class CDNUserBase;

class CDNNpcObject: public CDNBaseObject, public CDnNpc
{
public:
	CDNNpcObject(void);
	virtual ~CDNNpcObject(void);

	bool EnterWorld();
	bool LeaveWorld();

	bool InitObject(WCHAR *pName, UINT nUID, int nChannelID, int nMapIndex, TPosition *pCurPos);
	bool FinalObject();

	int FieldProcess(CDNBaseObject *pSender, USHORT wMsg, TBaseData *pSenderData, TParamData *pParamData);
	void DoUpdate(DWORD CurTick);

	bool CreateNpc(TNpcData *pNpcData, char *pName, UINT nUID, int nChannelID, int nMapIndex, TPosition *pCurPos, float fRotate);

	virtual bool Talk(CDNUserBase* pUserBase, UINT nNpcUniqueID, IN std::wstring& wszIndex, IN std::wstring& wszTarget) override;

	inline int GetChannelID() { return m_BaseData.nChannelID; }
};
