#include "StdAfx.h"
#include "DNFieldData.h"
#include "EtResourceMng.h"
#include "EtWorldGrid.h"
#include "EtWorldSector.h"
#include "EtWorldEventControl.h"
#include "EtWorldEventArea.h"
#include "EtWorldProp.h"
#include "DNField.h"
#include "DNNpcObject.h"
#include "DNPropNpcObject.h"
#include "DNGameDataManager.h"
#include "Util.h"
#include "Log.h"
#include "PropHeader.h"
#include "DnWorldGateStruct.h"

CDNFieldData::CDNFieldData(void)
{
	m_nMapIndex = 0;
	m_VecGateList.clear();
	m_VecStartPositionList.clear();
}

CDNFieldData::~CDNFieldData(void)
{
}

bool CDNFieldData::Init(char *pszMapName, int nMapIndex)
{
	DNTableFileFormat *pSox = GetDNTable( CDnTableDB::TMAP );
	EWorldEnum::MapTypeEnum MapType = (EWorldEnum::MapTypeEnum)pSox->GetFieldFromLablePtr( nMapIndex, "_MapType" )->GetInteger();

	m_szMapName = pszMapName;
	m_nMapIndex = nMapIndex;

	if (!m_World.Initialize(CEtResourceMng::GetInstance().GetFullPath("MapData").c_str(), pszMapName)) return false;

	int nLoadSectorEnum = CEtWorldSector::LSE_Terrain | CEtWorldSector::LSE_Control | CEtWorldSector::LSE_Prop;
	if( MapType == EWorldEnum::MapTypeVillage ) nLoadSectorEnum |= CEtWorldSector::LSE_GenCollisionHeight;

	m_World.ForceInitialize( nLoadSectorEnum );

	CalcGatePosition();

	return true;
}

void CDNFieldData::GetWidthHeight(UINT &nWidth, UINT &nHeight)
{
	nWidth = m_World.GetGridWidth() * 100;
	nHeight = m_World.GetGridHeight() * 100;
}

void CDNFieldData::CalcGatePosition()
{
	DWORD dwCount = m_World.GetGrid()->GetActiveSectorCount();
	CEtWorldEventControl *pControl = NULL;
	CEtWorldSector *pSector = NULL;
	CEtWorldEventArea *pArea = NULL;
	EventAreaStruct *pStruct = NULL;

	char szStr[32];
	AreaInfo Info;

	SAFE_DELETE_VEC( m_VecGateList );
	SAFE_DELETE_VEC( m_VecStartPositionList );

	for( DWORD i=0; i<dwCount; i++ ) {
		pSector = m_World.GetGrid()->GetActiveSector(i);
		pControl = pSector->GetControlFromUniqueID( ETE_EventArea );
		if( !pControl ) continue;

		for( int j=0; j<DNWORLD_GATE_COUNT; j++ ) {
			sprintf_s( szStr, "Gate %d", j + 1 );
			pArea = pControl->GetAreaFromName( szStr );
			if( pArea ) {
				Info.nIndex = j + 1;
				Info.Min.nX = (int)(pArea->GetMin()->x * 1000.f) - 100000;
				Info.Min.nY = (int)(pArea->GetMin()->y * 1000.f) - 100000;
				Info.Min.nZ = (int)(pArea->GetMin()->z * 1000.f) - 100000;
				Info.Max.nX = (int)(pArea->GetMax()->x * 1000.f) + 100000;
				Info.Max.nY = (int)(pArea->GetMax()->y * 1000.f) + 100000;
				Info.Max.nZ = (int)(pArea->GetMax()->z * 1000.f) + 100000;
				Info.fAngle = pArea->GetRotate();

				pStruct = (EventAreaStruct *)pArea->GetData();
				if (pStruct) Info.nType = pStruct->GateType;

				m_VecGateList.push_back( Info );
			}

			sprintf_s( szStr, "StartPosition %d", j + 1 );
			pArea = pControl->GetAreaFromName( szStr );
			if( pArea ) {
				Info.nIndex = j + 1;
				Info.Min.nX = (int)(pArea->GetMin()->x * 1000.f) ;
				Info.Min.nY = (int)(pArea->GetMin()->y * 1000.f) ;
				Info.Min.nZ = (int)(pArea->GetMin()->z * 1000.f) ;
				Info.Max.nX = (int)(pArea->GetMax()->x * 1000.f) ;
				Info.Max.nY = (int)(pArea->GetMax()->y * 1000.f) ;
				Info.Max.nZ = (int)(pArea->GetMax()->z * 1000.f) ;
				Info.fAngle = pArea->GetRotate();
				m_VecStartPositionList.push_back( Info );
			}
		}
	}

}

int CDNFieldData::CheckGateArea(TPosition &Position)
{
	for (int i = 0; i < (int)m_VecGateList.size(); i++){
		if (Position.nX < m_VecGateList[i].Min.nX || Position.nX > m_VecGateList[i].Max.nX ||
			Position.nZ < m_VecGateList[i].Min.nZ || Position.nZ > m_VecGateList[i].Max.nZ) continue;
		return m_VecGateList[i].nIndex;
	}

	return -1;
}

bool CDNFieldData::GetStartPosition(char cGateNo, TPosition &Pos)
{
	for( DWORD i=0; i<m_VecStartPositionList.size(); i++ ) {
		if( m_VecStartPositionList[i].nIndex == cGateNo ) {
			Pos = m_VecStartPositionList[i].Min;

			int nValue = ( m_VecStartPositionList[i].Max.nX - m_VecStartPositionList[i].Min.nX ) / 1000;
			if( nValue <= 0 ) nValue = 1;
			Pos.nX += ( _rand()%nValue ) * 1000;

			nValue = ( m_VecStartPositionList[i].Max.nY - m_VecStartPositionList[i].Min.nY ) / 1000;
			if( nValue <= 0 ) nValue = 1;
			Pos.nY += ( _rand()%nValue ) * 1000;

			nValue = ( m_VecStartPositionList[i].Max.nZ - m_VecStartPositionList[i].Min.nZ ) / 1000;
			if( nValue <= 0 ) nValue = 1;
			Pos.nZ += ( _rand()%nValue ) * 1000;

			Pos.nY += (int)m_World.GetHeight( Pos.nX / 1000.f, Pos.nZ / 1000.f ) * 1000;
			return true;
		}
	}

	return false;
}

void CDNFieldData::GetRandomStartPosition(TPosition &Pos)
{
	if( m_VecGateList.empty() )
		return;
	int nGateNo = _rand() % (int)m_VecGateList.size();
	GetStartPosition(m_VecGateList[nGateNo].nIndex, Pos);
}

void CDNFieldData::GetStartPositionAngle( char cGateNo, float &fAngle )
{
	for( DWORD i=0; i<m_VecStartPositionList.size(); i++ ) {
		if( m_VecStartPositionList[i].nIndex == cGateNo ) {
			fAngle = m_VecStartPositionList[i].fAngle;
			return;
		}
	}
}

char CDNFieldData::GetRandomGateNo()
{
	if ( m_VecGateList.empty() )
		return -1;

	int nGateNo = _rand() % (int)m_VecGateList.size();
	return m_VecGateList[nGateNo].nIndex;
}

void CDNFieldData::LoadCreateNpc(CDNField *pField)
{
	DWORD dwCount = m_World.GetGrid()->GetActiveSectorCount();
	CEtWorldEventControl *pControl = NULL;
	CEtWorldSector *pSector = NULL;
	CEtWorldEventArea *pArea = NULL;
	
	TNpcData *pNpcData = NULL;
	EtVector3 vPos;
	TPosition Pos;
	int nNpcIndex;
	char szAreaName[256] = { 0, };

	DNTableFileFormat *pSox = GetDNTable( CDnTableDB::TPROP );


	for (DWORD i = 0; i < dwCount; i++){
		pSector = m_World.GetGrid()->GetActiveSector(i);

		// EventArea 설정된 Npc 들.
		pControl = pSector->GetControlFromUniqueID(ETE_UnitArea);
		if( pControl ) {
			char szFirstName[256];

			for (DWORD j = 0; j < pControl->GetAreaCount(); j++){
				pArea = pControl->GetAreaFromIndex(j);

				sprintf_s(szAreaName, pArea->GetName());
				_strlwr_s(szAreaName);
				sscanf_s( szAreaName, "%s %d", szFirstName, sizeof(szFirstName), &nNpcIndex );
				if( strcmp( szFirstName, "npc" ) ) continue;

				pNpcData = g_pDataManager->GetNpcData(nNpcIndex);
				if (!pNpcData){
					g_Log.Log(LogType::_ERROR, L"(NpcName:%S) NpcData not found\r\n", szAreaName);
					continue;
				}

				CDNNpcObject *pNpcObj = new CDNNpcObject;
				if (!pNpcObj) continue;

				vPos.x = pArea->GetMin()->x + ((int)(pArea->GetMax()->x - pArea->GetMin()->x) / 2);
				vPos.z = pArea->GetMin()->z + ((int)(pArea->GetMax()->z - pArea->GetMin()->z) / 2);
				vPos.y = m_World.GetHeight(vPos) + pArea->GetMin()->y;

				memset(&Pos, 0, sizeof(TPosition));
				Pos.nX = (int)(vPos.x * 1000.f);
				Pos.nZ = (int)(vPos.z * 1000.f);
				Pos.nY = (int)(vPos.y * 1000.f);

				pNpcObj->CreateNpc(pNpcData, szAreaName, g_IDGenerator.GetNpcID(), pField->GetChnnelID(), pField->GetMapIndex(), &Pos, pArea->GetRotate());
				pField->AddNpcObject(pNpcObj);
			}
		}

		// Npc Prop 설정된 놈들
		for( DWORD j=0; j<pSector->GetPropCount(); j++ ) {
			CEtWorldProp *pProp = pSector->GetPropFromIndex(j);
			if( !pProp ) continue;
			int nItemID = pSox->GetItemIDFromField( "_Name", pProp->GetPropName() );
			if( nItemID == -1 ) continue;
			if( pSox->GetFieldFromLablePtr( nItemID, "_ClassID" )->GetInteger() != PTE_Npc ) continue;

			NpcStruct *pStruct = (NpcStruct *)pProp->GetData();

			if( pStruct->nNpcID <= 0 ) continue;

			pNpcData = g_pDataManager->GetNpcData(pStruct->nNpcID);
			if (!pNpcData){
				g_Log.Log(LogType::_ERROR, L"(NpcName:%S) NpcData not found\r\n", szAreaName);
				continue;
			}

			CDNPropNpcObject *pNpcObj = new CDNPropNpcObject;
			if (!pNpcObj) continue;

			pNpcObj->CreateNpc( pNpcData, g_IDGenerator.GetNpcID(), pField->GetChnnelID(), pField->GetMapIndex(), pProp );
			pField->AddNpcObject(pNpcObj);
		}
	}
}

float CDNFieldData::GetCollisionHeight(float fX, float fZ)
{
	return m_World.GetCollisionHeight(fX, fZ);
}

DWORD CDNFieldData::GetCurrentTileSize()
{
	return m_World.GetCurrentTileSize();
}

int CDNFieldData::GetGateType(TPosition &Position)
{
	for (int i = 0; i < (int)m_VecGateList.size(); i++){
		if (Position.nX < m_VecGateList[i].Min.nX || Position.nX > m_VecGateList[i].Max.nX ||
			Position.nZ < m_VecGateList[i].Min.nZ || Position.nZ > m_VecGateList[i].Max.nZ) continue;
		return m_VecGateList[i].nType;
	}

	return -1;
}
