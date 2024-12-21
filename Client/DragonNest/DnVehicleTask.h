#pragma once

#include "Task.h"
#include "MessageListener.h"
#include "DNProtocol.h"
#include "DNPacket.h"


// Rotha 

// Ż�Ϳ� ���� ��Ŷ�� �ַ� Ż�� ��ü�� �����۰� ������ ���谡 �ֱ⶧���� ItemTask ���� �޾Ƽ� ����ϰ� �˴ϴ�. 
// ������ Ż�Ϳ� ���� ������ ������ ������Task �� ����ϴ� ��İ��� ���̰� �ֱ⶧���� , ������ Task �ȿ� Ż�Ϳ� ���� ���ܻ����� �� �ֱ�����ϸ� ���������� ���Ƽ�
// ���� Ż���� Task���� ������ �մϴ� , ������ �̿��� �߰����� �޼����� ����� , �׺κп� ���ؼ��� �̰����� ����� �ֽø� �˴ϴ�.
// ����� Ż���� ����� Vehicle_Task �� ������ �ֵ��� �����ص� �����Դϴ�.


// Rotha Ż�� �ý��� TASK �߰�

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
	virtual void OnDispatchMessage( int nMainCmd, int nSubCmd, char *pData, int nSize ); // Ż�����θ� ������ ��� �߻��ϴ� �޼��� ó�� : ���� ���� 

	void OnConnectTcp() {}
	void OnDisconnectTcp(bool bValidDisconnect) {}

protected:
	void OnRecvVehicleItemMessage( int nSubCmd, char *pData, int nSize ); 

	void OnRecvVehicleEquipListMessage(SCVehicleEquipList *pData); // Ż�Ϳ� ���� ������ ���� �޾Ƽ� �����صӴϴ�.
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
	VEHICLE_LIST_VECTOR m_VehicleInfoList; // Ż���� ����Ʈ�� ������ �ְ� �˴ϴ�. <�ε���/Ż������>
	INT64 m_nCurrentVehicleSerial;         // ���� Ÿ�� �ִ� Ż���� �ø����� ����� �Ӵϴ�.
	UINT m_nCurrentVehicleID;
};

#define GetVehicleTask()		CDnVehicleTask::GetInstance()

