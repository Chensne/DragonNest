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



// Rotha Ż�� TASK �߰�

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

	if( pData->VehicleEquip.Vehicle[Vehicle::Slot::Body].nSerial != 0 ) // ó������Ʈ ������ Ÿ���ִٸ� ���� Ÿ���ִ� Ż���� �ø����� ����Ѵ�.
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

void CDnVehicleTask::OnRecvChangeVehicleColorMessagse( SCChangeVehicleColor *pData ) // ItemTask ���� �����մϴ�.
{
	// ���� ���� �÷��̾ ������ �ٲܰ�� �������־���Ѵ�.
}

void CDnVehicleTask::OnRecvRefreshVehicleInvenMessage( SCRefreshVehicleInven *pData )
{
	for( int i=0; i<pData->nCount; ++i )
	{
		if( pData->ItemList[i].Vehicle->wCount > 0 ) // ����
		{
			// ���� �������� ���� ��� ���� �� ����
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
		else // �Ϲ������� �̷���찡 ������ , �ν���Ʈ �����۰�����쿡�� Ż�� �κ����� �����Ǵ� ��찡 �����.
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
	// ���� ���� �÷��̾ ������ �ٲܰ�� �������־���Ѵ�.
}

int CDnVehicleTask::GetVehicleIndexFromSerial( INT64 nSerialID )
{
	for( int i=0; i<(int)m_VehicleInfoList.size(); i++ )
	{
		if( m_VehicleInfoList[i].Vehicle[Vehicle::Slot::Body].nSerial == nSerialID )
			return i; // �ø��� �ѹ��� ������ ã�Ҵ�!
	}
	return -1; // ��ã��
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
