
#include "Stdafx.h"
#include "DNFarm.h"


CDNFarm * g_pFarm = NULL;

CDNFarm::CDNFarm()
{
	m_vFarmList.clear();
	m_bIsInitWorld = false;
}

CDNFarm::~CDNFarm()
{
	ScopeLock <CSyncLock> sync(m_Sync);

	std::vector <TFarmItem*>::iterator ii;
	for (ii = m_vFarmList.begin(); ii != m_vFarmList.end(); ii++)
		delete (*ii);
	m_vFarmList.clear();
}

#if defined( PRE_ADD_VIP_FARM ) || defined( PRE_ADD_FARM_DOWNSCALE )

bool CDNFarm::AddFarmList( const TFarmItemFromDB* pFarm )
{
	if( pFarm == NULL || wcslen(pFarm->wszFarmName) == 0 ) 
		return false;

	ScopeLock <CSyncLock> sync(m_Sync);

	if( IsExistAsync(pFarm->iFarmDBID) == true )
	{
		_DANGER_POINT();		//이미 있어라~
		return false;
	}

	TFarmItem * pFarmItem = new TFarmItem;
	if( pFarmItem == NULL ) 
		return false;

	memset( pFarmItem, 0, sizeof(TFarmItem) );

	pFarmItem->iFarmDBID		= pFarm->iFarmDBID;
	pFarmItem->iFarmMapID		= pFarm->iFarmMapID;
	pFarmItem->iFarmMaxUser		= pFarm->iFarmMaxUser;
	pFarmItem->bStartActivate	= pFarm->bStartActivate;
#if defined( PRE_ADD_FARM_DOWNSCALE )
	pFarmItem->iAttr			= pFarm->iAttr;
#else
	pFarmItem->Attr				= pFarm->Attr;
#endif // #if defined( PRE_ADD_FARM_DOWNSCALE )
	_wcscpy( pFarmItem->wszFarmName, _countof(pFarmItem->wszFarmName), pFarm->wszFarmName, (int)wcslen(pFarm->wszFarmName) );

	m_vFarmList.push_back(pFarmItem);
	return true;
}

#else

bool CDNFarm::AddFarmList( int nFarmDBID, int nFarmMapID, int nFarmMaxUser, const WCHAR * pFarmName, bool bStart)
{
	if (pFarmName == NULL) return false;

	ScopeLock <CSyncLock> sync(m_Sync);

	if (IsExistAsync(nFarmDBID))
	{
		_DANGER_POINT();		//이미 있어라~
		return false;
	}

	TFarmItem * pFarmItem = new TFarmItem;
	if (!pFarmItem) return false;

	memset(pFarmItem, 0, sizeof(TFarmItem));

	pFarmItem->iFarmDBID	= nFarmDBID;
	pFarmItem->iFarmMapID	= nFarmMapID;
	pFarmItem->iFarmMaxUser	= nFarmMaxUser;
	pFarmItem->bStartActivate = bStart;
	_wcscpy( pFarmItem->wszFarmName, _countof(pFarmItem->wszFarmName), pFarmName, (int)wcslen(pFarmName) );

	m_vFarmList.push_back(pFarmItem);
	return true;
}

#endif // #if defined( PRE_ADD_VIP_FARM ) || defined( PRE_ADD_FARM_DOWNSCALE )

void CDNFarm::GetFarmList(TFarmItemFromDB * pFarm, BYTE &cCount)
{
	ScopeLock <CSyncLock> sync(m_Sync);

	cCount = 0;
	std::vector <TFarmItem*>::iterator ii;
	for (ii = m_vFarmList.begin(); ii != m_vFarmList.end(); ii++)
	{		
		pFarm[cCount].bEnableFlag = (*ii)->bEnableFlag;
		pFarm[cCount].iFarmDBID = (*ii)->iFarmDBID;
		pFarm[cCount].iFarmMapID = (*ii)->iFarmMapID;
		pFarm[cCount].iFarmMaxUser = (*ii)->iFarmMaxUser;
		pFarm[cCount].bStartActivate = (*ii)->bStartActivate;
#if defined( PRE_ADD_FARM_DOWNSCALE )
		pFarm[cCount].iAttr = (*ii)->iAttr;
#elif defined( PRE_ADD_VIP_FARM )
		pFarm[cCount].Attr = (*ii)->Attr;
#endif // #if defined( PRE_ADD_FARM_DOWNSCALE )
		_wcscpy(pFarm[cCount].wszFarmName, _countof(pFarm[cCount].wszFarmName), (*ii)->wszFarmName, (int)wcslen((*ii)->wszFarmName));
		cCount++;
	}
}

void CDNFarm::GetActivateFarmList(TFarmItem * pFarm, BYTE &cCount)
{
	ScopeLock <CSyncLock> sync(m_Sync);

	cCount = 0;
	std::vector <TFarmItem*>::iterator ii;
	for (ii = m_vFarmList.begin(); ii != m_vFarmList.end(); ii++)
	{
		if ((*ii)->bActivate != true) continue;
		memcpy(&pFarm[cCount], (*ii), sizeof(TFarmItem));
		cCount++;
	}
}

bool CDNFarm::GetFarmMapID(UINT nFarmDBID, int &nFarmMapID)
{
	ScopeLock <CSyncLock> sync(m_Sync);

	std::vector <TFarmItem*>::iterator ii;
	for (ii = m_vFarmList.begin(); ii != m_vFarmList.end(); ii++)
	{
		if ((*ii)->iFarmDBID == nFarmDBID)
		{
			nFarmMapID = (*ii)->iFarmMapID;
			return true;
		}
	}
	return false;
}

#if defined( PRE_ADD_FARM_DOWNSCALE )
bool CDNFarm::GetFarmAttr( UINT nFarmDBID, int& iAttr )
{
	ScopeLock <CSyncLock> sync(m_Sync);

	std::vector <TFarmItem*>::iterator ii;
	for (ii = m_vFarmList.begin(); ii != m_vFarmList.end(); ii++)
	{
		if ((*ii)->iFarmDBID == nFarmDBID)
		{
			iAttr = (*ii)->iAttr;
			return true;
		}
	}
	return false;
}
#elif defined( PRE_ADD_VIP_FARM )
bool CDNFarm::GetFarmAttr( UINT nFarmDBID, Farm::Attr::eType& Attr )
{
	ScopeLock <CSyncLock> sync(m_Sync);

	std::vector <TFarmItem*>::iterator ii;
	for (ii = m_vFarmList.begin(); ii != m_vFarmList.end(); ii++)
	{
		if ((*ii)->iFarmDBID == nFarmDBID)
		{
			Attr = (*ii)->Attr;
			return true;
		}
	}
	return false;
}
#endif // #if defined( PRE_ADD_FARM_DOWNSCALE )

void CDNFarm::UpdateFarmInfo(TFarmInfo * pInfo, BYTE cCount)
{
	if (pInfo == NULL) return;

	ScopeLock <CSyncLock> sync(m_Sync);

	std::vector <TFarmItem*>::iterator ii;
	for (ii = m_vFarmList.begin(); ii != m_vFarmList.end(); ii++)
	{
		int nCount = 0;
		for ( ;nCount <= cCount; nCount++)
		{
			if (pInfo[nCount].nFarmDBID == (*ii)->iFarmDBID)
			{
				(*ii)->iFarmCurUser = pInfo[nCount].nFarmCurUserCount;
				(*ii)->bActivate = pInfo[nCount].bActivate;
			}

			if (cCount == nCount)
				break;
		}
	}
}

bool CDNFarm::IsExistAsync(UINT nFarmDBID)
{
	std::vector <TFarmItem*>::iterator ii;
	for (ii = m_vFarmList.begin(); ii != m_vFarmList.end(); ii++)
		if ((*ii)->iFarmDBID == nFarmDBID)
			return true;
	return false;
}

