#include "StdAfx.h"
#include "Test_Emblem.h"
#include "../../../../GameCommon/DnItemCompounder.h"
#include "SundriesFunc.h"
#include <mmsystem.h>

CPPUNIT_TEST_SUITE_REGISTRATION( CTest_Emblem );

CTest_Emblem::CTest_Emblem(void)
{
	srand( timeGetTime() );

	m_pItemCompounder = new CDnItemCompounder;

	bool bResult = m_pItemCompounder->InitializeTable();
	CPPUNIT_ASSERT_MESSAGE( "Emblem Factory 초기화 실패. 테이블 데이터 오류", true == bResult );

}

CTest_Emblem::~CTest_Emblem(void)
{
	SAFE_DELETE( m_pItemCompounder );
}



void CTest_Emblem::test_composition_Jewel_Plate( void )
{
	// 아이템 조합 테스트
	CDnItemCompounder::S_ITEM_SETTING_INFO SettingInfo;
	CDnItemCompounder::S_OUTPUT Output;

	//for( int i = 0; i < NUM_MAX_NEED_ITEM; ++i )
	//{
	//	SettingInfo.aiItemID[ i ] = i+1;
	//	SettingInfo.aiItemCount[ i ] = i+1;
	//}

	SettingInfo.aiItemID[ 0 ] = 521010001;
	SettingInfo.aiItemCount[ 0 ] = 1;
	SettingInfo.aiItemID[ 1 ] = 521010011;
	SettingInfo.aiItemCount[ 1 ] = 1;
	SettingInfo.aiItemID[ 2 ] = 521010021;
	SettingInfo.aiItemCount[ 2 ] = 1;

	SettingInfo.iCompoundTableID = 1;
	SettingInfo.iHasMoney = 100;

	// 필요 아이템 슬롯에 셋팅 안된 거 테스트
	SettingInfo.aiItemID[ 0 ] = 0;
	m_pItemCompounder->ValidateEmblemCompound( 1, SettingInfo, &Output );
	CPPUNIT_ASSERT_MESSAGE( "슬롯 하나가 비어있는 상태!", CDnItemCompounder::E_NOT_ENOUGH_ITEM == Output.eErrorCode &&
														  CDnItemCompounder::R_ERROR == Output.eResultCode );

	// 슬롯에 맞지 않는 아이템인 거 테스트
	SettingInfo.aiItemID[ 0 ] = 2;
	m_pItemCompounder->ValidateEmblemCompound( 1, SettingInfo, &Output );
	CPPUNIT_ASSERT_MESSAGE( "슬롯에 맞지 않는 아이템 셋팅 상태!", CDnItemCompounder::E_NOT_MATCH_SLOT == Output.eErrorCode &&
																  CDnItemCompounder::R_ERROR == Output.eResultCode );

	// 필요 아이템 갯수 모자란 거 테스트
	SettingInfo.aiItemID[ 0 ] = 521010001;
	SettingInfo.aiItemCount[ 0 ] = 0;
	m_pItemCompounder->ValidateEmblemCompound( 1, SettingInfo, &Output );
	CPPUNIT_ASSERT_MESSAGE( "필요 아이템 갯수 모자란 상태!", CDnItemCompounder::E_NOT_ENOUGH_ITEM_COUNT == Output.eErrorCode &&
															 CDnItemCompounder::R_ERROR == Output.eResultCode );

	// 필요자금 셋팅된 것이 현재 0임
	//// 돈 부족한 거 테스트.
	//SettingInfo.aiItemID[ 0 ] = 521010001;
	//SettingInfo.aiItemCount[ 0 ] = 1;
	//SettingInfo.iHasMoney = 50;
	//m_pItemCompounder->ValidateEmblemCompound( 1, SettingInfo, &Output );
	//CPPUNIT_ASSERT_MESSAGE( "돈이 부족한 상태!", CDnItemCompounder::E_NOT_ENOUGH_MONEY == Output.eErrorCode &&
	//											 CDnItemCompounder::R_ERROR == Output.eResultCode );

	// 해당 플레이트가 수행하지 않는 아이템 조합
	SettingInfo.iCompoundTableID = 10;
	SettingInfo.iHasMoney = 100;
	m_pItemCompounder->ValidateEmblemCompound( 620000001, SettingInfo, &Output );
	CPPUNIT_ASSERT_MESSAGE( "해당 플레이트가 수행하지 않는 조합!", CDnItemCompounder::E_NOT_MATCH_PLATE_WITH_ITEM_COMPOUND == Output.eErrorCode &&
																   CDnItemCompounder::R_ERROR == Output.eResultCode );

	// 제대로 조합, 혹은 실패했을 때 결과물 아이템 테스트
	SettingInfo.iCompoundTableID = 1;
	SettingInfo.aiItemCount[ 0 ] = 1;
	m_pItemCompounder->EmblemCompound( 620000001, SettingInfo, &Output );
	CPPUNIT_ASSERT_MESSAGE( "정상적으로 조합 되었음!", CDnItemCompounder::E_NONE == Output.eErrorCode &&
													   CDnItemCompounder::R_SUCCESS == Output.eResultCode &&
													   //Output.fTimeRequired == 3.0f &&
													   Output.iCost == 0 &&
													   Output.vlItemID.front() == 600000001 &&
													   Output.vlItemCount.front() == -1 );		// 원래는 -1 나오면 안됨. 지금은 임시 데이터
	
	// 확률 적용 여부 테스트


	// 문장 보옥 정보 조회 테스트

	// 문장 보옥 조합 테스트

	//// 테스트!
	//multiset<int> setJewels;
	//setJewels.insert( 1 );
	//setJewels.insert( 2 );
	//setJewels.insert( 3 );
	//setJewels.insert( 4 );
	//setJewels.insert( 5 );
	//setJewels.insert( 5 );
	//setJewels.insert( 7 );
	//setJewels.insert( 8 );
	//setJewels.insert( 9 );
	//setJewels.insert( 10 );
	//int iCost = 0;
	//int iResultItemID = m_pEmblemFactory->Compound( 200002, setJewels, &iCost );
	//CPPUNIT_ASSERT_MESSAGE( "문장 보옥 조합 테스트 실패!", iResultItemID == 100 );
	//CPPUNIT_ASSERT_MESSAGE( "문장 보옥 조합 비용이 잘못된 값임!", iCost == 500 );

	//setJewels.clear();
	//setJewels.insert( 2 );
	//setJewels.insert( 2 );
	//setJewels.insert( 2 );
	//iCost = 0;
	//iResultItemID = m_pEmblemFactory->Compound( 200003, setJewels, &iCost );
	//CPPUNIT_ASSERT_MESSAGE( "문장 보옥 조합 테스트 결과값 오류, 조합 실패해야 함.", iResultItemID == 0 );

	////int iNumUse = 0;
	////iResultItemID = m_pEmblemFactory->UpgradeJewel( 200000, 14, &iCost, &iNumUse );
	////CPPUNIT_ASSERT_MESSAGE( "보옥 업그레이드 테스트 실패!", iResultItemID == 200001 );
	////CPPUNIT_ASSERT_MESSAGE( "보옥 업그레이드 조합 비용이 잘못된 값임!", iCost == 30 );
	////CPPUNIT_ASSERT_MESSAGE( "보옥 업그레이드시 사용 갯수가 잘못된 값임!", iNumUse == 10 );
}


// 아이템 분해 테스트
void CTest_Emblem::test_item_decomposition( void )
{
	//m_pItemCompounder->Decompose(  );
}