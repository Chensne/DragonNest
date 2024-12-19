#include "StdAfx.h"

#if defined( PRE_ADD_DWC )

#include "DNDWCChannelManager.h"
#include "DNExtManager.h"
#include "DNSQLWorldManager.h"

CDNDWCChannelManager* g_pDWCChannelManager;

CDNDWCChannelManager::CDNDWCChannelManager()
{
	memset(&m_ChannelInfo, 0, sizeof(m_ChannelInfo));
}

CDNDWCChannelManager::~CDNDWCChannelManager()
{
}

bool CDNDWCChannelManager::Init()
{
#if defined( PRE_MOD_SELECT_CHAR )
	int nDWCWorldSetID = g_pExtManager->GetWorldSetID( DWC::DWCWorldID );
#else // #if defined( PRE_MOD_SELECT_CHAR )
	int nDWCWorldSetID = DWC::DWCWorldID;
#endif // #if defined( PRE_MOD_SELECT_CHAR )

	if (-1 == nDWCWorldSetID)   
	{
		printf("[Err]CDNDWCChannelManager::Init()  error. -1 == nDWCWorldSetID\n");
        nDWCWorldSetID = 1; 
	}

	int nResult = g_pSQLWorldManager->QueryGetDWCChannelInfo( nDWCWorldSetID, &m_ChannelInfo );
#if defined(_WORK)
#else // #if defined(_WORK)
	if( ERROR_NONE != nResult )
		return false;
#endif // #if defined(_WORK)

	return true;
}

void CDNDWCChannelManager::UpdateInfo()
{
#if defined( PRE_MOD_SELECT_CHAR )
	int nDWCWorldSetID = g_pExtManager->GetWorldSetID( DWC::DWCWorldID );
#else // #if defined( PRE_MOD_SELECT_CHAR )
	int nDWCWorldSetID = DWC::DWCWorldID;
#endif // #if defined( PRE_MOD_SELECT_CHAR )

	int nResult = g_pSQLWorldManager->QueryGetDWCChannelInfo( nDWCWorldSetID, &m_ChannelInfo );
	if( ERROR_NONE != nResult )
	{
		g_Log.Log(LogType::_ERROR, L"[QueryGetDWCChannelInfo] Fail!!!(result:%d)\r\n", nResult);
		return;
	}

	// 캐릭터 리스트를 새로 받을 때 마다 ChannelInfo를 받기 때문에 
	// 전체 유저에게 갱신된 정보를 보내주는 것은 부담스러워서 스킵한다.
	// 보내려면 여기서 Lock 걸고 UserConnectionManager에 함수 만들어서 호출
}

bool CDNDWCChannelManager::CheckValidDate()
{
	if( DWC::STATUS_NORMAL != m_ChannelInfo.cStatus && DWC::STATUS_PAUSE != m_ChannelInfo.cStatus )
		return false;

	__time64_t tCurrentDate;
	time(&tCurrentDate);

	if( tCurrentDate < m_ChannelInfo.tStartDate || tCurrentDate > m_ChannelInfo.tEndDate )
		return false;

	return true;
}

void CDNDWCChannelManager::GetChannelInfo( OUT TDWCChannelInfo& rChannelInfo )
{
	memcpy(&rChannelInfo, &m_ChannelInfo, sizeof(TDWCChannelInfo));
}

#endif // #if defined( PRE_ADD_DWC )
