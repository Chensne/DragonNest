#pragma once

#include "Task.h"
#include "MessageListener.h"
#include "DNProtocol.h"
#include "DNPacket.h"


// Rotha 

// 탈것에 대한 패킷은 주로 탈것 자체가 아이템과 밀접한 관계가 있기때문에 ItemTask 에서 받아서 사용하게 됩니다. 
// 하지만 탈것에 대한 정보는 기존의 아이템Task 가 사용하던 방식과는 차이가 있기때문에 , 아이템 Task 안에 탈것에 대한 예외사항을 다 넣기시작하면 복잡해질것 같아서
// 따로 탈것의 Task에서 관리를 합니다 , 아이템 이외의 추가적인 메세지가 생기면 , 그부분에 대해서는 이곳에서 사용해 주시면 됩니다.
// 현재는 탈것의 목록을 Vehicle_Task 가 가지고 있도록 설정해둔 상태입니다.


// Rotha 탈것 시스템 TASK 추가

class CDnVehicleTask : public CTask, public CTaskListener, public CSingleton<CDnVehicleTask>
{
public:
	typedef std::vector<TVehicleCompact>		    VEHICLE_LIST_VECTOR;
	typedef VEHICLE_LIST_VECTOR::iterator		    VEHICLE_LIST_VECTOR_ITER;
	typedef VEHICLE_LIST_VECTOR::const_iterator	    VEHICLE_LIST_VECTOR_ITER_CONST;

	CDnVehicleTask();
	~CDnVehicleTask();

	bool Initialize();
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnDispatchMessage( int nMainCmd, int nSubCmd, char *pData, int nSize ); // 탈것으로만 보내는 경우 발생하는 메세지 처리 : 아직 없음 

	void OnConnectTcp() {}
	void OnDisconnectTcp(bool bValidDisconnect) {}

protected:
	void OnRecvVehicleItemMessage( int nSubCmd, char *pData, int nSize ); 

	void OnRecvVehicleEquipListMessage(SCVehicleEquipList *pData); // 탈것에 대한 정보를 따로 받아서 저장해둡니다.
	void OnRecvVehicleInvenListMessage(SCVehicleInvenList *pData);
	void OnRecvChangeVehiclePartsMessage(SCChangeVehicleParts *pData);
	void OnRecvChangeVehicleColorMessagse(SCChangeVehicleColor *pData);
	void OnRecvRefreshVehicleInvenMessage(SCRefreshVehicleInven *pData);
	void OnRecvPetEquipListMessage( SCVehicleEquipList* pData );
	void ClearVehicleInfo();

public:
	void RemoveInstantVehicleItem();
	void SetRide(DnActorHandle hActor,int nVehicleID);
	void SetUnRide(DnActorHandle hActor);
	
	bool IsExistVehicle(INT64 nItemSerial);
	int GetVehicleIndexFromSerial(INT64 nSerialID);
	TVehicleCompact* GetVehicleInfoFromSerial( INT64 nSerialID );
	VEHICLE_LIST_VECTOR GetVehicleList(){return m_VehicleInfoList;}
	
protected:
	VEHICLE_LIST_VECTOR m_VehicleInfoList; // 탈것의 리스트를 가지고 있게 됩니다. <인덱스/탈것정보>
	INT64 m_nCurrentVehicleSerial;         // 현재 타고 있는 탈것의 시리얼을 기억해 둡니다.
	UINT m_nCurrentVehicleID;
};

#define GetVehicleTask()		CDnVehicleTask::GetInstance()

