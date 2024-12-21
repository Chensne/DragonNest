#pragma once

#include "Connection.h"
#include "MultiSync.h"

struct _WAITUSER
{
	UINT nAccountDBID;
	ULONG nAddTime;

	//update
	USHORT nWaitTicketNum;
};

class CDNUserConnection;
class CDNMasterConnection: public CConnection
{

public:
	enum eTick
	{
		eTickSendConCountInfo,	// 현재 LO 의 동시접속자 정보를 MA 로 송신
		eTickCnt,
	};

private:
	bool m_bStarted;
	
#ifdef _USE_ACCEPTEX
	CSyncLock	m_Lock;
#endif
	std::vector < std::pair <int, sChannelInfo> > m_vChannelList;
	UINT m_nWorldMaxUser;
	UINT m_nWorldCurUser;

	std::list <_WAITUSER> m_WaitUserList;
	mutable DWORD m_dwTick[eTickCnt+1];		// 틱 카운트 (각 개체의 특정 동작의 시간범위를 관리할 경우가 발생 시 여기에 공통적으로 등록하여 사용할 목적)

public:
	CDNMasterConnection(void);
	virtual ~CDNMasterConnection(void);

	bool GetStarted() { return m_bStarted; }

	// 헷갈릴까봐 worldID 뽑아내는 함수 만듬
	const int GetWorldSetID() { return GetSessionID(); }	
	UINT GetWorldMaxUserCount() { return m_nWorldMaxUser; }
	UINT GetWorldCurUserCount() { return m_nWorldCurUser; }
	UINT GetWaitUSerCount() { return (UINT)m_WaitUserList.size(); }

	bool AddWaitUser(UINT nAccountDBID, USHORT nWaitTicketNum, USHORT &nEstimateTime);
	bool DelWaitUser(UINT nAccountDBID);
	void UpdateWaitTicketNum();
	
	int MessageProcess(int nMainCmd, int nSubCmd, char *pData, int nLen);

	void GetChannelListByMapIdx(TDBListCharData* pSelectCharData, sChannelInfo *ChannelArray, BYTE &cCount, TMapChannel *ChannelList);
#if defined(PRE_ADD_DWC)
	void GetDWCChannelList(TDBListCharData* pSelectCharData, sChannelInfo *ChannelArray, BYTE &cCount, TMapChannel *ChannelList);	
#endif

	bool bIsUseLastMapIndex( const int iMapIndex );
	bool bIsPvPMapIndex( const int iMapIndex );
	bool bIsGMMapIndex( const int iMapIndex );
	bool bIsDarkLairMapIndex( const int iMapIndex );
	bool bIsFarmMapIndex(int nMapIndex);
	
	// Tick
	DWORD GetTick(eTick pTick) const;
	void SetTick(eTick pTick) const;
	void SetTick(eTick pTick, DWORD dwVal) const;
	void ResetTick(eTick pTick) const;
	BOOL IsTick(eTick pTick) const;	

	// Send관련 함수는 이곳에...
	bool SendRegist(int nServerID);
	void SendConCountInfo(int nServerID, int nCurConCount, int nMaxConCount);
#if defined(PRE_MOD_SELECT_CHAR)
	void SendAddUser(CDNUserConnection *pUserCon, bool bForce = false);
	void SendDetachUser(UINT nAccountDBID, bool bIsReconnectLogin = false, bool bIsDuplicate = false, UINT nSessionID=0);	
#else	// #if defined(PRE_MOD_SELECT_CHAR)
	void SendAddUser(CDNUserConnection *pUserCon, int nWorldID, bool bForce=false );
	void SendUserInfo(CDNUserConnection *pUserCon, int nSelectedChannelID, bool bAdult, char *pVirtualIp, char *pIp, bool bTutorial);
	void SendDetachUser(UINT nAccountDBID, BYTE cAccountLevel, bool bIsReconnectLogin = false, const BYTE * pMachineID = NULL, DWORD dwGRC = 0, bool bIsDuplicate = false, UINT nSessionID=0, const char * pszIP = NULL);	
#endif	// #if defined(PRE_MOD_SELECT_CHAR)

	void SendReqTutorialGameID(UINT nAccountDBID, UINT nSessionID, int nMapID, char cGateNo);

	void SendAddWaitUser(int nServerID, UINT nAccountDBID);
	void SendDelWaitUser(int nServerID, UINT nAccountDBID);
	void SendDetachUserOther(UINT nAccountDBID, int nLoginServerID);
#ifdef PRE_MOD_RESTRICT_IDENTITY_IP
	void SendDetachUserbyIP(const char * pszIP);
#endif		//#ifdef PRE_MOD_RESTRICT_IDENTITY_IP
#ifdef PRE_ADD_BEGINNERGUILD
	void SendGuildMemberAdd(MAAddGuildMember * pPacket);
#endif		//#ifdef PRE_ADD_BEGINNERGUILD

private:
	bool _bCheckMapIndexFromAttr( const int iMapIndex, const UINT uiAttr );
};

