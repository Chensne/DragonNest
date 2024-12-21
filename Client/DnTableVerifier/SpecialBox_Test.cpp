#include "StdAfx.h"
#include "SpecialBox_Test.h"

DNTableFileFormat* SpecialBox_Test::s_pMailTable = NULL;
DNTableFileFormat* SpecialBox_Test::s_pKeepBoxProvideItemTable = NULL;

SpecialBox_Test::SpecialBox_Test(void)
{

}

SpecialBox_Test::~SpecialBox_Test(void)
{

}

void SpecialBox_Test::SetUpTestCase( void )
{
	s_pMailTable = LoadExtFile( "mailtable.dnt" );
	s_pKeepBoxProvideItemTable = LoadExtFile( "keepboxprovideitemtable.dnt");
}

void SpecialBox_Test::TearDownTestCase( void )
{
	SAFE_DELETE( s_pMailTable );
	SAFE_DELETE( s_pKeepBoxProvideItemTable );
}


void SpecialBox_Test::SetUp( void )
{
	for( int i=0; i<s_pMailTable->GetItemCount(); i++ )
	{
		int nItemID = s_pMailTable->GetItemID( i );
		int nKeepBoxRecevieType = s_pMailTable->GetFieldFromLablePtr( nItemID, "_KeepBoxReceive" )->GetInteger();

		if( nKeepBoxRecevieType > 0 )
		{
			bool bSizeCheck = false;
			int nItemSize = 0;

			int nKeepBoxType = s_pMailTable->GetFieldFromLablePtr( nItemID, "_KeepBoxType" )->GetInteger();

			for( int k=0; k<s_pKeepBoxProvideItemTable->GetItemCount(); k++ )
			{
				int nKeepBoxItemIndex = s_pKeepBoxProvideItemTable->GetItemID( k );
				int nType = s_pKeepBoxProvideItemTable->GetFieldFromLablePtr( nKeepBoxItemIndex, "_Type" )->GetInteger();

				if( nKeepBoxType == nType )
				{
					nItemSize++;
				}
			}

			s_pMailTable->GetFieldFromLablePtr( nItemID, "_KeepBoxReceive" )->GetInteger();

			if( nKeepBoxRecevieType == SpecialBox::ReceiveTypeCode::Select )
			{
				bSizeCheck = (nItemSize > SpecialBox::Common::ListMax ) == false;
			}

			if( nKeepBoxRecevieType == SpecialBox::ReceiveTypeCode::All )
			{
				bSizeCheck = (nItemSize > SpecialBox::Common::RewardAllMax ) == false;
			}

			EXPECT_TRUE( bSizeCheck );
		}
	}
}


void SpecialBox_Test::TearDown( void )
{

}

TEST_F( SpecialBox_Test, SPECIALBOX_VALIDCHECK )
{

}
