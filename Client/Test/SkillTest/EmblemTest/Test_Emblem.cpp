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
	CPPUNIT_ASSERT_MESSAGE( "Emblem Factory �ʱ�ȭ ����. ���̺� ������ ����", true == bResult );

}

CTest_Emblem::~CTest_Emblem(void)
{
	SAFE_DELETE( m_pItemCompounder );
}



void CTest_Emblem::test_composition_Jewel_Plate( void )
{
	// ������ ���� �׽�Ʈ
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

	// �ʿ� ������ ���Կ� ���� �ȵ� �� �׽�Ʈ
	SettingInfo.aiItemID[ 0 ] = 0;
	m_pItemCompounder->ValidateEmblemCompound( 1, SettingInfo, &Output );
	CPPUNIT_ASSERT_MESSAGE( "���� �ϳ��� ����ִ� ����!", CDnItemCompounder::E_NOT_ENOUGH_ITEM == Output.eErrorCode &&
														  CDnItemCompounder::R_ERROR == Output.eResultCode );

	// ���Կ� ���� �ʴ� �������� �� �׽�Ʈ
	SettingInfo.aiItemID[ 0 ] = 2;
	m_pItemCompounder->ValidateEmblemCompound( 1, SettingInfo, &Output );
	CPPUNIT_ASSERT_MESSAGE( "���Կ� ���� �ʴ� ������ ���� ����!", CDnItemCompounder::E_NOT_MATCH_SLOT == Output.eErrorCode &&
																  CDnItemCompounder::R_ERROR == Output.eResultCode );

	// �ʿ� ������ ���� ���ڶ� �� �׽�Ʈ
	SettingInfo.aiItemID[ 0 ] = 521010001;
	SettingInfo.aiItemCount[ 0 ] = 0;
	m_pItemCompounder->ValidateEmblemCompound( 1, SettingInfo, &Output );
	CPPUNIT_ASSERT_MESSAGE( "�ʿ� ������ ���� ���ڶ� ����!", CDnItemCompounder::E_NOT_ENOUGH_ITEM_COUNT == Output.eErrorCode &&
															 CDnItemCompounder::R_ERROR == Output.eResultCode );

	// �ʿ��ڱ� ���õ� ���� ���� 0��
	//// �� ������ �� �׽�Ʈ.
	//SettingInfo.aiItemID[ 0 ] = 521010001;
	//SettingInfo.aiItemCount[ 0 ] = 1;
	//SettingInfo.iHasMoney = 50;
	//m_pItemCompounder->ValidateEmblemCompound( 1, SettingInfo, &Output );
	//CPPUNIT_ASSERT_MESSAGE( "���� ������ ����!", CDnItemCompounder::E_NOT_ENOUGH_MONEY == Output.eErrorCode &&
	//											 CDnItemCompounder::R_ERROR == Output.eResultCode );

	// �ش� �÷���Ʈ�� �������� �ʴ� ������ ����
	SettingInfo.iCompoundTableID = 10;
	SettingInfo.iHasMoney = 100;
	m_pItemCompounder->ValidateEmblemCompound( 620000001, SettingInfo, &Output );
	CPPUNIT_ASSERT_MESSAGE( "�ش� �÷���Ʈ�� �������� �ʴ� ����!", CDnItemCompounder::E_NOT_MATCH_PLATE_WITH_ITEM_COMPOUND == Output.eErrorCode &&
																   CDnItemCompounder::R_ERROR == Output.eResultCode );

	// ����� ����, Ȥ�� �������� �� ����� ������ �׽�Ʈ
	SettingInfo.iCompoundTableID = 1;
	SettingInfo.aiItemCount[ 0 ] = 1;
	m_pItemCompounder->EmblemCompound( 620000001, SettingInfo, &Output );
	CPPUNIT_ASSERT_MESSAGE( "���������� ���� �Ǿ���!", CDnItemCompounder::E_NONE == Output.eErrorCode &&
													   CDnItemCompounder::R_SUCCESS == Output.eResultCode &&
													   //Output.fTimeRequired == 3.0f &&
													   Output.iCost == 0 &&
													   Output.vlItemID.front() == 600000001 &&
													   Output.vlItemCount.front() == -1 );		// ������ -1 ������ �ȵ�. ������ �ӽ� ������
	
	// Ȯ�� ���� ���� �׽�Ʈ


	// ���� ���� ���� ��ȸ �׽�Ʈ

	// ���� ���� ���� �׽�Ʈ

	//// �׽�Ʈ!
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
	//CPPUNIT_ASSERT_MESSAGE( "���� ���� ���� �׽�Ʈ ����!", iResultItemID == 100 );
	//CPPUNIT_ASSERT_MESSAGE( "���� ���� ���� ����� �߸��� ����!", iCost == 500 );

	//setJewels.clear();
	//setJewels.insert( 2 );
	//setJewels.insert( 2 );
	//setJewels.insert( 2 );
	//iCost = 0;
	//iResultItemID = m_pEmblemFactory->Compound( 200003, setJewels, &iCost );
	//CPPUNIT_ASSERT_MESSAGE( "���� ���� ���� �׽�Ʈ ����� ����, ���� �����ؾ� ��.", iResultItemID == 0 );

	////int iNumUse = 0;
	////iResultItemID = m_pEmblemFactory->UpgradeJewel( 200000, 14, &iCost, &iNumUse );
	////CPPUNIT_ASSERT_MESSAGE( "���� ���׷��̵� �׽�Ʈ ����!", iResultItemID == 200001 );
	////CPPUNIT_ASSERT_MESSAGE( "���� ���׷��̵� ���� ����� �߸��� ����!", iCost == 30 );
	////CPPUNIT_ASSERT_MESSAGE( "���� ���׷��̵�� ��� ������ �߸��� ����!", iNumUse == 10 );
}


// ������ ���� �׽�Ʈ
void CTest_Emblem::test_item_decomposition( void )
{
	//m_pItemCompounder->Decompose(  );
}