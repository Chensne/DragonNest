
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

	// 보정 값 까지 설정하려면 GetUseRegulation() 을 사용해야하는데 그럴려면 게임룸에서 게임테스크및 여러가지 설정을 해야하는데
	// 다 설정하려고하니 너무 많아서 이 테스트 사용하실때는 해당함수에서 원하는 보정값을 강제로 리턴하는게 더 편할것 같습니다
	// GetUseRegulation() 함수내에서 { return 원하는값; } 설정하고 테스트 하세요

	m_pPlayerActor->RefreshState( CDnPlayerState::RefreshAll , CDnPlayerState::ST_AttackP );

	int nMinAttackP = m_pPlayerActor->GetAttackPMin();
	int nMaxAttackP = m_pPlayerActor->GetAttackPMax();

	// 계산순서 = 베이스 값(스텟에의한 값) +장비값(보정시켜서 절충) + ( 보정된 장비값 * 버프값 ) + 스킬 계산값


	int nBaseValue = nOriginalAttackMin;
	int nEquipLevel = ( nBaseValue + 100 ) + (int)( (nBaseValue + 100) * 0.2f ); // 힘민지체는 보정연산 안함 
	int nBuffLevel = ( nEquipLevel + 100 ) + (int)( (nEquipLevel + 100) * -0.2f ); 

	EXPECT_TRUE(nMinAttackP == nBuffLevel);

	nBaseValue = nOriginalAttackMax;
	nEquipLevel = ( nBaseValue + 100 ) + (int)( (nBaseValue + 100) * 0.2f ); // 힘민지체는 보정연산 안함 
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

	// 보정 값 까지 설정하려면 GetUseRegulation() 을 사용해야하는데 그럴려면 게임룸에서 게임테스크및 여러가지 설정을 해야하는데
	// 다 설정하려고하니 너무 많아서 이 테스트 사용하실때는 해당함수에서 원하는 보정값을 강제로 리턴하는게 더 편할것 같습니다
	// GetUseRegulation() 함수내에서 { return 원하는값; } 설정하고 테스트 하세요

	m_pPlayerActor->RefreshState( CDnPlayerState::RefreshAll , CDnPlayerState::ST_Strength );

	int nFinalValue = m_pPlayerActor->GetStrength();

	// 계산순서 = 베이스 값(스텟에의한 값) +장비값(보정시켜서 절충) + ( 보정된 장비값 * 버프값 ) + 스킬 계산값
	//  이큅(#기본# 절대 * 비율) + 버프(#이큅# 버프절대 * 비율)

	int nBaseValue = nOriginalStrength;
	int nEquipLevel = ( nBaseValue + 100 ) + (int)( (nBaseValue + 100) * 0.2f ); // + 보정 연산
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


	// 보정 값 까지 설정하려면 GetUseRegulation() 을 사용해야하는데 그럴려면 게임룸에서 게임테스크및 여러가지 설정을 해야하는데
	// 다 설정하려고하니 너무 많아서 이 테스트 사용하실때는 해당함수에서 원하는 보정값을 강제로 리턴하는게 더 편할것 같습니다
	// GetUseRegulation() 함수내에서 { return 원하는값; } 설정하고 테스트 하세요

	m_pPlayerActor->RefreshState( CDnPlayerState::RefreshAll , CDnPlayerState::ST_ElementAttack );
	float fFinalValue = m_pPlayerActor->GetElementAttack( CDnState::Fire );

	// 계산순서 = 베이스 값(스텟에의한 값) +장비값(보정시켜서 절충) + ( 보정된 장비값 * 버프값 ) + 스킬 계산값

	float fBaseValue = fOriginalElementAttack;
	float fEquipLevel = ( fBaseValue + 0.3f ); // + 보정 연산
	float fBuffLevel =  ( fEquipLevel + 0.3f ); 

	EXPECT_TRUE( fFinalValue == fBuffLevel );
}

#endif
