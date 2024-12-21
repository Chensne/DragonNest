#include "StdAfx.h"
#include "DNFieldDataManager.h"
#include "DNFieldData.h"
#include "Log.h"

CDNFieldDataManager* g_pFieldDataManager;

CDNFieldDataManager::CDNFieldDataManager(void)
{
	CEtWorld::SetIgnoreInvalidFile( true );
	m_FieldDatas.clear();
}

CDNFieldDataManager::~CDNFieldDataManager(void)
{
	m_FieldDatas.clear();
}

CDNFieldData* CDNFieldDataManager::AddFieldData(char *pszMapName, int nMapIndex)
{
	CDNFieldData *pFieldData = NULL;
	pFieldData = FindFieldData(pszMapName);
	if (pFieldData){
		g_Log.Log( LogType::_FILELOG, L"MapName:%S AddFieldData Duplicated\r\n", pszMapName);
		return pFieldData;	// 이미 있다.
	}

	pFieldData = new CDNFieldData;
	if (!pFieldData) 
		return NULL;

	if (!pFieldData->Init(pszMapName, nMapIndex)) 
	{
		SAFE_DELETE( pFieldData );
		return NULL;
	}
	m_FieldDatas.insert(make_pair(pszMapName, pFieldData));

	return pFieldData;
}

bool CDNFieldDataManager::DelFieldData(char *pszMapName)
{
	if (m_FieldDatas.empty()) return false;

	TMapFieldDatas::iterator iter = m_FieldDatas.find(pszMapName);
	if (iter != m_FieldDatas.end()){
		m_FieldDatas.erase(iter);
		return true;
	}

	return false;
}

CDNFieldData* CDNFieldDataManager::FindFieldData(char *pszMapName)
{
	if (m_FieldDatas.empty()) return NULL;

	TMapFieldDatas::iterator iter = m_FieldDatas.find(pszMapName);
	if (iter != m_FieldDatas.end()){
		return iter->second;
	}

	return NULL;
}

CDNFieldData* CDNFieldDataManager::FindFieldDataByMapIndex(int nMapIndex)
{
	if (m_FieldDatas.empty()) return NULL;

	TMapFieldDatas::iterator iter;
	for (iter = m_FieldDatas.begin(); iter != m_FieldDatas.end(); ++iter){
		if (iter->second->GetMapIndex() == nMapIndex){
			return iter->second;
		}
	}

	return NULL;
}

char CDNFieldDataManager::GetRandomGateNo(int nMapIndex)
{
	CDNFieldData *pFieldData = FindFieldDataByMapIndex(nMapIndex);
	if (!pFieldData) return -1;

	return pFieldData->GetRandomGateNo();
}

int CDNFieldDataManager::CheckGateArea(int nMapIndex, TPosition &Position)
{
	CDNFieldData *pFieldData = FindFieldDataByMapIndex(nMapIndex);
	if (!pFieldData) return -1;

	return pFieldData->CheckGateArea(Position);
}

bool CDNFieldDataManager::GetStartPosition(int nMapIndex, char cGateNo, TPosition &Pos)
{
	CDNFieldData *pFieldData = FindFieldDataByMapIndex(nMapIndex);
	if (!pFieldData) return false;

	return pFieldData->GetStartPosition(cGateNo, Pos);
}

void CDNFieldDataManager::GetRandomStartPosition(int nMapIndex, TPosition &Pos)
{
	CDNFieldData *pFieldData = FindFieldDataByMapIndex(nMapIndex);
	if (!pFieldData) return;

	pFieldData->GetRandomStartPosition(Pos);
}

void CDNFieldDataManager::GetStartPositionAngle( int nMapIndex, char cGateNo, float &fAngle )
{
	CDNFieldData *pFieldData = FindFieldDataByMapIndex(nMapIndex);
	if (!pFieldData) return;

	pFieldData->GetStartPositionAngle(cGateNo, fAngle);
}

int CDNFieldDataManager::GetGateType(int nMapIndex, TPosition &Position)
{
	CDNFieldData *pFieldData = FindFieldDataByMapIndex(nMapIndex);
	if (!pFieldData) return -1;

	return pFieldData->GetGateType(Position);
}
