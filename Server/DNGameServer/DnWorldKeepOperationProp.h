#pragma once

#include "DnWorldOperationProp.h"

class CDnPropStateTrigger;
class CDnWorldKeepOperationProp : public CDnWorldOperationProp
{
public:
	CDnWorldKeepOperationProp(CMultiRoom * pRoom);
	virtual ~CDnWorldKeepOperationProp();

	virtual void Process(LOCAL_TIME LocalTime, float fDelta);
	virtual void OnDispatchMessage( CDNUserSession *pSession, DWORD dwProtocol, BYTE *pPacket );

	bool OnClickKeepProp( CDNUserSession * pSession, int nOperationTime, int nStringID, const char * szActionName );
	void OnCancelKeepOperationProp( CDNUserSession * pSession );

	void SendKeepOperationResult( DnActorHandle hActor, bool bRsult );
	void SendKeepOperationStart( DnActorHandle hActor, int nOperationTime, int nStringID, const char * szActionName );
	
protected:
	
	bool m_bClicked;

	LONG m_nPropKeepClickTick;			//클릭한 상태로 얼마나 기댕겨야하는지

	UINT m_nClickedSessionID;			//프랍을 클릭한녀석의 세션아이디
	DWORD m_nClickedUniqueID;			//프랍을 클릭한녀석의 유니크아이디
	LOCAL_TIME m_PropClickedTime;		//프랍을 클릭한 시간

	virtual bool Initialize( CEtWorldSector *pParentSector, const char *szPropName, EtVector3 &vPos, EtVector3 &vRotate, EtVector3 &vScale );

private:
	void _ClearClickedInfo( CDNUserSession * pSession );
	bool _IsActivated(LOCAL_TIME LocalTime);
	void _CheckKeepProp(LOCAL_TIME LocalTime);
};

class IBoostPoolDnWorldKeepOperationProp : public CDnWorldKeepOperationProp, public TBoostMemoryPool<IBoostPoolDnWorldKeepOperationProp>
{
public:
	IBoostPoolDnWorldKeepOperationProp( CMultiRoom *pRoom ):CDnWorldKeepOperationProp(pRoom){}
	virtual ~IBoostPoolDnWorldKeepOperationProp(){}
};