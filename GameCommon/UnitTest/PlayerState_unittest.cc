
#include "stdafx.h"
#include "./boost/scoped_ptr.hpp"
#include "DNGameRoom.h"
#include "DnPlayerActor.h"
#include "TDnPlayerWarrior.h"

// --gtest_filter=PlayerState_unittest.*

#if !defined( _FINAL_BUILD ) && !defined (__COVERITY__)

class PlayerState_unittest:public testing::Test
{
protected:

	virtual void SetUp() 
	{
		MAGAReqRoomID Packet;
		memset( &Packet, 0, sizeof(Packet) );
		m_pRoom	= new CDNGameRoom( NULL, 0, &Packet );
		m_pPlayerActor = (new TDnPlayerWarrior<CDnPlayerActor>( m_pRoom, 1 ) );
		m_pPlayerActor->InitializeRoom(m_pRoom);
		m_pPlayerActor->SetJobHistory(1);
		m_pPlayerActor->SetLevel(50);
		m_pPlayerActor->RefreshState();
	}

	virtual void TearDown()
	{
		SAFE_DELETE(m_pPlayerActor);
		SAFE_DELETE(m_pRoom);
	}

	static void SetUpTestCase()
	{
		m_pResMng = new CEtResourceMng( false, true );
		EXPECT_TRUE( m_pResMng->AddResourcePath( "r:\\gameres\\resource\\ext" ) );

		m_pTableDB = new CDnTableDB();
		EXPECT_TRUE( m_pTableDB->Initialize() );
	}

	static void TearDownTestCase()
	{
		SAFE_DELETE(m_pTableDB);
		SAFE_DELETE(m_pResMng);
	}

	CDNGameRoom *m_pRoom;
	CDnPlayerActor *m_pPlayerActor;

	static CEtResourceMng*	m_pResMng;
	static CDnTableDB*		m_pTableDB;

};

CEtResourceMng* PlayerState_unittest::m_pResMng		= NULL;
CDnTableDB*		PlayerState_unittest::m_pTableDB		= NULL;

#define USE_POST_UPGRADE_PLAYERSTATUS_VERSION

TEST_F( PlayerState_unittest, Test_AttackP )
{
	int nOriginalAttackMin = m_pPlayerActor->GetAttackPMin();
	int nOriginalAttackMax = m_pPlayerActor->GetAttackPMax();

	CDnState *pEquipStateAbsolute = new CDnState;
	pEquipStateAbsolute->ResetState();
	pEquipStateAbsolute->SetAttackPMin(100);
	pEquipStateAbsolute->SetAttackPMax(100);
	pEquipStateAbsolute->CalcValueType();

#ifdef USE_POST_UPGRADE_PLAYERSTATUS_VERSION
	m_pPlayerActor->AddBlowState( pEquipStateAbsolute , CDnActorState::Equip_Buff_Level , false ); 
#else
	m_pPlayerActor->AddBlowState( pEquipStateAbsolute , CDnActorState::Equip_Buff_Level , false , false );
#endif

	CDnState *pEquipStateRatio = new CDnState;
	pEquipStateRatio->ResetState();
	pEquipStateRatio->SetAttackPMinRatio(0.2f);
	pEquipStateRatio->SetAttackPMaxRatio(0.2f);
	pEquipStateRatio->CalcValueType();

#ifdef USE_POST_UPGRADE_PLAYERSTATUS_VERSION
	m_pPlayerActor->AddBlowState( pEquipStateRatio , CDnActorState::Equip_Buff_Level , false ); 
#else
	m_pPlayerActor->AddBlowState( pEquipStateRatio , CDnActorState::Equip_Buff_Level , false , false );
#endif


	CDnState *pBuffStateAbsolute = new CDnState;
	pBuffStateAbsolute->ResetState();
	pBuffStateAbsolute->SetAttackPMin(100);
	pBuffStateAbsolute->SetAttackPMax(100);
	pBuffStateAbsolute->CalcValueType();

#ifdef USE_POST_UPGRADE_PLAYERSTATUS_VERSION
	m_pPlayerActor->AddBlowState( pBuffStateAbsolute , CDnActorState::Equip_Buff_Level , true ); 
#else
	m_pPlayerActor->AddBlowState( pBuffStateAbsolute , CDnActorState::Equip_Buff_Level , true , true );
#endif

	CDnState *pBuffStateRatio = new CDnState;
	pBuffStateRatio->ResetState();
	pBuffStateRatio->SetAttackPMinRatio(-0.2f);
	pBuffStateRatio->SetAttackPMaxRatio(-0.2f);
	pBuffStateRatio->CalcValueType();

#ifdef USE_POST_UPGRADE_PLAYERSTATUS_VERSION
	m_pPlayerActor->AddBlowState( pBuffStateRatio , CDnActorState::Equip_Buff_Level , true ); 
#else
	m_pPlayerActor->AddBlowState( pBuffStateRatio , CDnActorState::Equip_Buff_Level , true , true );
#endif

	// ���� �� ���� �����Ϸ��� GetUseRegulation() �� ����ؾ��ϴµ� �׷����� ���ӷ뿡�� �����׽�ũ�� �������� ������ �ؾ��ϴµ�
	// �� �����Ϸ����ϴ� �ʹ� ���Ƽ� �� �׽�Ʈ ����ϽǶ��� �ش��Լ����� ���ϴ� �������� ������ �����ϴ°� �� ���Ұ� �����ϴ�
	// GetUseRegulation() �Լ������� { return ���ϴ°�; } �����ϰ� �׽�Ʈ �ϼ���

	m_pPlayerActor->RefreshState( CDnPlayerState::RefreshAll , CDnPlayerState::ST_AttackP );

	int nMinAttackP = m_pPlayerActor->GetAttackPMin();
	int nMaxAttackP = m_pPlayerActor->GetAttackPMax();

	// ������ = ���̽� ��(���ݿ����� ��) +���(�������Ѽ� ����) + ( ������ ��� * ������ ) + ��ų ��갪


	int nBaseValue = nOriginalAttackMin;
	int nEquipLevel = ( nBaseValue + 100 ) + (int)( (nBaseValue + 100) * 0.2f ); // ������ü�� �������� ���� 
	int nBuffLevel = ( nEquipLevel + 100 ) + (int)( (nEquipLevel + 100) * -0.2f ); 

	EXPECT_TRUE(nMinAttackP == nBuffLevel);

	nBaseValue = nOriginalAttackMax;
	nEquipLevel = ( nBaseValue + 100 ) + (int)( (nBaseValue + 100) * 0.2f ); // ������ü�� �������� ���� 
	nBuffLevel = ( nEquipLevel + 100 ) + (int)( (nEquipLevel + 100) * -0.2f ); 
	
	EXPECT_TRUE(nMaxAttackP == nBuffLevel);
}

TEST_F( PlayerState_unittest, Test_Strength )
{
	int nOriginalStrength = m_pPlayerActor->GetStrength();

	CDnState *pEquipStateAbsolute = new CDnState;
	pEquipStateAbsolute->ResetState();
	pEquipStateAbsolute->SetStrength(100);
	pEquipStateAbsolute->CalcValueType();

#ifdef USE_POST_UPGRADE_PLAYERSTATUS_VERSION
	m_pPlayerActor->AddBlowState( pEquipStateAbsolute , CDnActorState::Equip_Buff_Level , false ); 
#else
	m_pPlayerActor->AddBlowState( pEquipStateAbsolute , CDnActorState::Equip_Buff_Level , false , false );
#endif

	CDnState *pEquipStateRatio = new CDnState;
	pEquipStateRatio->ResetState();
	pEquipStateRatio->SetStrengthRatio(0.2f);
	pEquipStateRatio->CalcValueType();

#ifdef USE_POST_UPGRADE_PLAYERSTATUS_VERSION
	m_pPlayerActor->AddBlowState( pEquipStateRatio , CDnActorState::Equip_Buff_Level , false ); 
#else
	m_pPlayerActor->AddBlowState( pEquipStateRatio , CDnActorState::Equip_Buff_Level , false , false );
#endif


	CDnState *pBuffStateAbsolute = new CDnState;
	pBuffStateAbsolute->ResetState();
	pBuffStateAbsolute->SetStrength(100);
	pBuffStateAbsolute->CalcValueType();

#ifdef USE_POST_UPGRADE_PLAYERSTATUS_VERSION
	m_pPlayerActor->AddBlowState( pBuffStateAbsolute , CDnActorState::Equip_Buff_Level , true ); 
#else
	m_pPlayerActor->AddBlowState( pBuffStateAbsolute , CDnActorState::Equip_Buff_Level , true , true );
#endif

	CDnState *pBuffStateRatio = new CDnState;
	pBuffStateRatio->ResetState();
	pBuffStateRatio->SetStrengthRatio(-0.2f);
	pBuffStateRatio->CalcValueType();

#ifdef USE_POST_UPGRADE_PLAYERSTATUS_VERSION
	m_pPlayerActor->AddBlowState( pBuffStateRatio , CDnActorState::Equip_Buff_Level , true ); 
#else
	m_pPlayerActor->AddBlowState( pBuffStateRatio , CDnActorState::Equip_Buff_Level , true , true );
#endif

	// ���� �� ���� �����Ϸ��� GetUseRegulation() �� ����ؾ��ϴµ� �׷����� ���ӷ뿡�� �����׽�ũ�� �������� ������ �ؾ��ϴµ�
	// �� �����Ϸ����ϴ� �ʹ� ���Ƽ� �� �׽�Ʈ ����ϽǶ��� �ش��Լ����� ���ϴ� �������� ������ �����ϴ°� �� ���Ұ� �����ϴ�
	// GetUseRegulation() �Լ������� { return ���ϴ°�; } �����ϰ� �׽�Ʈ �ϼ���

	m_pPlayerActor->RefreshState( CDnPlayerState::RefreshAll , CDnPlayerState::ST_Strength );

	int nFinalValue = m_pPlayerActor->GetStrength();

	// ������ = ���̽� ��(���ݿ����� ��) +���(�������Ѽ� ����) + ( ������ ��� * ������ ) + ��ų ��갪
	//  ��Ţ(#�⺻# ���� * ����) + ����(#��Ţ# �������� * ����)

	int nBaseValue = nOriginalStrength;
	int nEquipLevel = ( nBaseValue + 100 ) + (int)( (nBaseValue + 100) * 0.2f ); // + ���� ����
	int nBuffLevel = ( nEquipLevel + 100 ) + (int)( (nEquipLevel + 100) * -0.2f ); 
	
	EXPECT_TRUE( nFinalValue == nBuffLevel );
}

TEST_F( PlayerState_unittest, Test_ElementAttack )
{
	float fOriginalElementAttack = m_pPlayerActor->GetElementAttack( CDnState::Fire);

	CDnState *pEquipStateAbsolute = new CDnState;
	pEquipStateAbsolute->ResetState();
	pEquipStateAbsolute->SetElementAttack( CDnState::Fire ,0.3f );
	pEquipStateAbsolute->CalcValueType();

#ifdef USE_POST_UPGRADE_PLAYERSTATUS_VERSION
	m_pPlayerActor->AddBlowState( pEquipStateAbsolute , CDnActorState::Equip_Buff_Level , false ); 
#else
	m_pPlayerActor->AddBlowState( pEquipStateAbsolute , CDnActorState::Equip_Buff_Level , false , false );
#endif

	CDnState *pBuffStateAbsolute = new CDnState;
	pBuffStateAbsolute->ResetState();
	pBuffStateAbsolute->SetElementAttack( CDnState::Fire , 0.3f );
	pBuffStateAbsolute->CalcValueType();

#ifdef USE_POST_UPGRADE_PLAYERSTATUS_VERSION
	m_pPlayerActor->AddBlowState( pBuffStateAbsolute , CDnActorState::Equip_Buff_Level , true ); 
#else
	m_pPlayerActor->AddBlowState( pBuffStateAbsolute , CDnActorState::Equip_Buff_Level  , true , true );
#endif


	// ���� �� ���� �����Ϸ��� GetUseRegulation() �� ����ؾ��ϴµ� �׷����� ���ӷ뿡�� �����׽�ũ�� �������� ������ �ؾ��ϴµ�
	// �� �����Ϸ����ϴ� �ʹ� ���Ƽ� �� �׽�Ʈ ����ϽǶ��� �ش��Լ����� ���ϴ� �������� ������ �����ϴ°� �� ���Ұ� �����ϴ�
	// GetUseRegulation() �Լ������� { return ���ϴ°�; } �����ϰ� �׽�Ʈ �ϼ���

	m_pPlayerActor->RefreshState( CDnPlayerState::RefreshAll , CDnPlayerState::ST_ElementAttack );
	float fFinalValue = m_pPlayerActor->GetElementAttack( CDnState::Fire );

	// ������ = ���̽� ��(���ݿ����� ��) +���(�������Ѽ� ����) + ( ������ ��� * ������ ) + ��ų ��갪

	float fBaseValue = fOriginalElementAttack;
	float fEquipLevel = ( fBaseValue + 0.3f ); // + ���� ����
	float fBuffLevel =  ( fEquipLevel + 0.3f ); 

	EXPECT_TRUE( fFinalValue == fBuffLevel );
}

#endif
