#pragma once

/********************************************************************
	created:	2011/03/23
	filename: 	DnStateBlowSignalProcessor.h
	author:		semozz
	
	purpose:	����ȿ���� Process�Լ����� OnSignaló�� �κ��� ���� �и��ؼ� ���⼭ ó�� �ǵ�����
*********************************************************************/
class CDnStateBlowSignalProcessor
{
public:
	CDnStateBlowSignalProcessor();
	~CDnStateBlowSignalProcessor();

	// ���ο� Blow�� ��� ����Ʈ�� �߰�
	void AddBlow(DnBlowHandle hBlow);
	// ������ Blow�� ���� ����Ʈ�� �߰�
	void RemoveBlow(DnBlowHandle hBlow);
	
	// �߰� �Ұ͵� �߰� �ϰ�, ���� ����Ʈ ����, ���� �Ұ͵� ����
	void Process(LOCAL_TIME localTime, float fDelta);

protected:
	// ��� ����Ʈ�� �ִ� Blow�� ���� ������ �ű��
	void AddListProcess();
	// ���� ����Ʈ�� �ִ� Blow�� ���� ����Ʈ���� ã�Ƽ� �����
	void RemoveListProcess();

	// ���� ����Ʈ�� ��ȸ �ϸ鼭 Blow�� SignalProcessȣ��
	void OnSignal(LOCAL_TIME localTime, float fDelta);

private:
	typedef std::map<DWORD, DnBlowHandle> STATE_BLOW_LIST;
	
	STATE_BLOW_LIST m_AddList;
	STATE_BLOW_LIST m_MainList;
	STATE_BLOW_LIST m_DeleteList;

};