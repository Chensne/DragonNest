
//note
//유저를 검증하는 용도로 사용은 금함 살짜꿍 확인정도는 가능
//커뮤니티관련 해서 상대방의 상태를 얻는 용도로 한정

#pragma once

#include <map>
#include "DNServerDef.h"
#include "Util.h"

class CDNWorldUserState
{
#if defined( PRE_WORLDUSERSTATE_OPTIMIZE )
public:
	virtual ~CDNWorldUserState();
protected:
	CDNWorldUserState();
public:
#else
public:
	CDNWorldUserState();
	~CDNWorldUserState();
#endif // #if defined( PRE_WORLDUSERSTATE_OPTIMIZE )

	virtual bool AddUserState(const WCHAR * pName, INT64 biCharacterDBID, int nLocation, int nCummunity, int nChannelID, int nMapIdx);
	virtual bool DelUserState(const WCHAR * pName, INT64 biCharacterDBID);

	//WCHAR Ver
	virtual bool UpdateUserState(const WCHAR * pName, INT64 biCharacterDBID, int nLocation = -1, int nCummunity = -1, int nChannelID = -1, int nMapIdx = -1);
	virtual bool GetUserState(const WCHAR * pName, INT64 biCharacterDBID, sWorldUserState *pWorldUserState);
	virtual bool GetUserState( INT64 biCharacterDBID, sWorldUserState *pWorldUserState );
	virtual bool GetUserState( INT64 biCharacterDBID, TCommunityLocation& Location );
	virtual int GetUserLocation(const WCHAR * pName, INT64 biCharacterDBID);
	virtual int GetUserCummunityState(const WCHAR * pName, INT64 biCharacterDBID);

#if defined( PRE_WORLDUSERSTATE_OPTIMIZE )
	virtual void SnapShotUserState(){}
#endif // #if defined( PRE_WORLDUSERSTATE_OPTIMIZE )
protected:

#if defined( PRE_WORLDUSERSTATE_OPTIMIZE )
	typedef std::map<std::wstring, sWorldUserState> TCharNameUserState;
	typedef std::map<INT64,sWorldUserState> TCharDBIDUserState;
#else
	typedef std::map<std::wstring, sWorldUserState*> TCharNameUserState;
	typedef std::map<INT64,sWorldUserState*> TCharDBIDUserState;
#endif // #if defined( PRE_WORLDUSERSTATE_OPTIMIZE )
	TCharNameUserState m_WorldUserStateList;
	TCharDBIDUserState m_WorldUserStateListByCharDBID;

#if defined( PRE_WORLDUSERSTATE_OPTIMIZE )
	TCharNameUserState* m_pGetOnlyCharNameUserState;
	TCharDBIDUserState* m_pGetOnlyCharDBIDUserState;
#else
	CSyncLock m_Sync;
#endif // #if defined( PRE_WORLDUSERSTATE_OPTIMIZE )
};

extern CDNWorldUserState * g_pWorldUserState;
