#include "StdAfx.h"
#include "DnVehicleTask.h"
#include "PvPSendPacket.h"
#include "DnPlayerActor.h"
#include "DnPlayerCamera.h"
#include "DnInterface.h"
#include "DnMainDlg.h"
#include "DnTableDB.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 



// Rotha 탈것 TASK 추가

CDnVehicleTask::CDnVehicleTask() : CTaskListener( true )
{
	ClearVehicleInfo();
	m_VehicleInfoList.clear();
	m_nCurrentVehicleID = 0;
	m_nCurrentVehicleSerial = 0;
}

CDnVehicleTask::~CDnVehicleTask()
{
	SAFE_DELETE_VEC(m_VehicleInfoList);
}

bool CDnVehicleTask::Initialize()
{
	m_VehicleInfoList.clear();
	return true;
}

void CDnVehicleTask::Process( LOCAL_TIME LocalTime, float fDelta )
{
}

void CDnVehicleTask::OnDispatchMessage( int nMainCmd, int nSubCmd, char *pData, int nSize )
{
	switch (nMainCmd)
	{
		case SC_ITEM:    OnRecvVehicleItemMessage( nSubCmd, pData, nSize ); break;
	}
}

void CDnVehicleTask::OnRecvVehicleItemMessage( int nSubCmd, char *pData, int nSize )
{
	switch(nSubCmd)
	{
		case eItem::SC_VEHICLEEQUIPLIST: OnRecvVehicleEquipListMessage( (SCVehicleEquipList*)pData ); break;
		case eItem::SC_VEHICLEINVENLIST: OnRecvVehicleInvenListMessage( (SCVehicleInvenList*)pData ); break;
		case eItem::SC_CHANGEVEHICLEPARTS: OnRecvChangeVehiclePartsMessage( (SCChangeVehicleParts*)pData ); break;
		case eItem::SC_CHANGEVEHICLECOLOR: OnRecvChangeVehicleColorMessagse( (SCChangeVehicleColor*)pData ); break;
		case eItem::SC_REFRESHVEHICLEINVEN: OnRecvRefreshVehicleInvenMessage( (SCRefreshVehicleInven*)pData ); break;
		case eItem::SC_PETEQUIPLIST: OnRecvPetEquipListMessage( (SCVehicleEquipList*)pData ); break;
	}
}

void CDnVehicleTask::RemoveInstantVehicleItem()
{
	VEHICLE_LIST_VECTOR vecTempList;
	DNTableFileFormat* pInstantItemSox = GetDNTable( CDnTableDB::TINSTANTITEM );

	for( int i=0; i<(int)m_VehicleInfoList.size(); i++ )
	{
		bool bInstant = false;
		if( pInstantItemSox )
		{
			for( int k=0; k<pInstantItemSox->GetItemCount() ; k++ )
			{
				int nTableID = pInstantItemSox->GetItemID(k);
				int nInstatnItemIndex = -1;
				nInstatnItemIndex =	pInstantItemSox->GetFieldFromLablePtr( nTableID, "_ItemID" )->GetInteger();

				if(m_VehicleInfoList[i].Vehicle[Vehicle::Slot::Body].nItemID == nInstatnItemIndex)
					bInstant = true;
			}
		}

		if( !bInstant )
			vecTempList.push_back(m_VehicleInfoList[i]);
	}

	m_VehicleInfoList.clear();
	m_VehicleInfoList.swap(vecTempList);

	if(GetInterface().GetMainBarDialog())
		GetInterface().GetMainBarDialog()->RefreshLifeSkillQuickSlot();
}

void CDnVehicleTask::OnRecvVehicleEquipListMessage( SCVehicleEquipList* pData )
{
	m_VehicleInfoList.push_back(pData->VehicleEquip);

	if( pData->VehicleEquip.Vehicle[Vehicle::Slot::Body].nSerial != 0 ) // 처음리스트 받을때 타고있다면 현재 타고있는 탈것의 시리얼을 기억한다.
	{
		m_nCurrentVehicleSerial = pData->VehicleEquip.Vehicle[Vehicle::Slot::Body].nSerial;
		m_nCurrentVehicleID = pData->VehicleEquip.Vehicle[Vehicle::Slot::Body].nItemID;
	}
}

void CDnVehicleTask::OnRecvPetEquipListMessage( SCVehicleEquipList* pData )
{
	m_VehicleInfoList.push_back( pData->VehicleEquip );
}

void CDnVehicleTask::OnRecvVehicleInvenListMessage( SCVehicleInvenList* pData )
{
	int nItemCount = pData->nTotalInventoryCount;
	
	if( nItemCount > VEHICLEINVENTORYPAGEMAX )
		nItemCount = VEHICLEINVENTORYPAGEMAX;

	for( int i=0; i<nItemCount; i++ )
	{
		if(pData->VehicleInven[i].Vehicle[Vehicle::Slot::Body].nSerial != 0)
			m_VehicleInfoList.push_back(pData->VehicleInven[i]);
	}
}

void CDnVehicleTask::OnRecvChangeVehiclePartsMessage( SCChangeVehicleParts *pData )
{
	if( pData->cSlotIndex == Vehicle::Slot::Body )
	{
		m_nCurrentVehicleSerial = pData->Equip.nSerial;
		m_nCurrentVehicleID = pData->Equip.nItemID;
	}
}

void CDnVehicleTask::OnRecvChangeVehicleColorMessagse( SCChangeVehicleColor *pData ) // ItemTask 에서 관리합니다.
{
	// 만약 로컬 플레이어가 정보를 바꿀경우 갱신해주어야한다.
}

void CDnVehicleTask::OnRecvRefreshVehicleInvenMessage( SCRefreshVehicleInven *pData )
{
	for( int i=0; i<pData->nCount; ++i )
	{
		if( pData->ItemList[i].Vehicle->wCount > 0 ) // 삽입
		{
			// 같은 아이템이 있을 경우 삭제 후 삽입
			VEHICLE_LIST_VECTOR_ITER iter = m_VehicleInfoList.begin();
			for( ; iter != m_VehicleInfoList.end(); iter++ )
			{
				if( (*iter).Vehicle[Vehicle::Slot::Body].nSerial == pData->ItemList[i].Vehicle[Vehicle::Slot::Body].nSerial )
				{
					m_VehicleInfoList.erase( iter );
					break;
				}
			}

			m_VehicleInfoList.push_back( pData->ItemList[i] );
		}
		else // 일반적으로 이런경우가 없지만 , 인스턴트 아이템같은경우에는 탈것 인벤에서 삭제되는 경우가 생긴다.
		{
			int nIndex = -1;
			nIndex = GetVehicleIndexFromSerial( pData->ItemList[i].Vehicle[Vehicle::Slot::Body].nSerial );
			if( nIndex > 0 && nIndex < (int)m_VehicleInfoList.size() )
			{
				VEHICLE_LIST_VECTOR_ITER iter;
				iter = m_VehicleInfoList.begin() + nIndex;
				m_VehicleInfoList.erase( iter );
			}
		}
	}

	if( GetInterface().GetMainBarDialog() )
		GetInterface().GetMainBarDialog()->RefreshLifeSkillQuickSlot();
	// 만약 로컬 플레이어가 정보를 바꿀경우 갱신해주어야한다.
}

int CDnVehicleTask::GetVehicleIndexFromSerial( INT64 nSerialID )
{
	for( int i=0; i<(int)m_VehicleInfoList.size(); i++ )
	{
		if( m_VehicleInfoList[i].Vehicle[Vehicle::Slot::Body].nSerial == nSerialID )
			return i; // 시리얼 넘버가 같으면 찾았다!
	}
	return -1; // 몬찾음
}

TVehicleCompact* CDnVehicleTask::GetVehicleInfoFromSerial( INT64 nSerialID )
{
	for( int i=0; i<(int)m_VehicleInfoList.size(); i++ )
	{
		if( m_VehicleInfoList[i].Vehicle[Vehicle::Slot::Body].nSerial == nSerialID )
			return &m_VehicleInfoList[i];
	}
	return NULL;
}

void CDnVehicleTask::ClearVehicleInfo()
{
	m_VehicleInfoList.clear();
}

bool CDnVehicleTask::IsExistVehicle( INT64 nItemSerial )
{
	bool bIsExist = false;
	for( int i=0; i<(int)m_VehicleInfoList.size(); i++ )
	{
		if( m_VehicleInfoList[i].Vehicle[Vehicle::Slot::Body].nSerial == nItemSerial )
			bIsExist = true;
	}

	return bIsExist;
}
