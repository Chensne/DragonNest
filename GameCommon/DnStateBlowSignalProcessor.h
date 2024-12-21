#pragma once

/********************************************************************
	created:	2011/03/23
	filename: 	DnStateBlowSignalProcessor.h
	author:		semozz
	
	purpose:	상태효과의 Process함수에서 OnSignal처리 부분을 따로 분리해서 여기서 처리 되도록함
*********************************************************************/
class CDnStateBlowSignalProcessor
{
public:
	CDnStateBlowSignalProcessor();
	~CDnStateBlowSignalProcessor();

	// 새로운 Blow를 등록 리스트에 추가
	void AddBlow(DnBlowHandle hBlow);
	// 제거할 Blow를 제거 리스트에 추가
	void RemoveBlow(DnBlowHandle hBlow);
	
	// 추가 할것들 추가 하고, 메인 리스트 돌고, 제거 할것들 제거
	void Process(LOCAL_TIME localTime, float fDelta);

protected:
	// 등록 리스트에 있던 Blow를 메인 리스에 옮긴다
	void AddListProcess();
	// 제거 리스트에 있던 Blow를 메인 리스트에서 찾아서 지운다
	void RemoveListProcess();

	// 메인 리스트를 순회 하면서 Blow의 SignalProcess호출
	void OnSignal(LOCAL_TIME localTime, float fDelta);

private:
	typedef std::map<DWORD, DnBlowHandle> STATE_BLOW_LIST;
	
	STATE_BLOW_LIST m_AddList;
	STATE_BLOW_LIST m_MainList;
	STATE_BLOW_LIST m_DeleteList;

};