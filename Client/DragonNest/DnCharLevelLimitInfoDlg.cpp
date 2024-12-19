#include "StdAfx.h"
#include "DnCharLevelLimitInfoDlg.h"
#include "DnTableDB.h"


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 


#ifdef PRE_MOD_SELECT_CHAR

CDnCharLevelLimitInfoDlg::CDnCharLevelLimitInfoDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
{
}

CDnCharLevelLimitInfoDlg::~CDnCharLevelLimitInfoDlg(void)
{
}

void CDnCharLevelLimitInfoDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "SubCharDlg.ui" ).c_str(), bShow );
}

void CDnCharLevelLimitInfoDlg::InitialUpdate()
{
}

void CDnCharLevelLimitInfoDlg::SetLevelLimitCount( int nLevelLimitCount )
{
	if( nLevelLimitCount == 0 )
		return;

	float fExtraExpByMonster = 0.0f;
	float fExtraExpByStageClear = 0.0f;

	DNTableFileFormat*  pSoxLevelPromo = GetDNTable( CDnTableDB::TLEVELPROMO );
	if( pSoxLevelPromo )
	{
		for( int j=0; j<pSoxLevelPromo->GetItemCount(); j++ )
		{
			int nItemID = pSoxLevelPromo->GetItemID( j );

			int nConditionValue = atoi( pSoxLevelPromo->GetFieldFromLablePtr( nItemID, "_UserConditionValue1" )->GetString() );
			int nRewardType = pSoxLevelPromo->GetFieldFromLablePtr( nItemID, "_RewardType1")->GetInteger();

			if( nConditionValue <= nLevelLimitCount	&& nRewardType == ePromotionType::PROMOTIONTYPE_MONSTERKILL )
			{
				fExtraExpByMonster = (float)atof( pSoxLevelPromo->GetFieldFromLablePtr( nItemID, "_RewardValue1" )->GetString() );
			}
			if( nConditionValue <= nLevelLimitCount	&& nRewardType == ePromotionType::PROMOTIONTYPE_STAGECLEAR )
			{
				fExtraExpByStageClear = (float)atof( pSoxLevelPromo->GetFieldFromLablePtr( nItemID, "_RewardValue1" )->GetString() );
			}
		}
	}

	std::wstring strText;
	CEtUIStatic* pStatic = GetControl<CEtUIStatic>( "ID_TEXT0" );
	strText = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 215 ), nLevelLimitCount );
	pStatic->SetText( strText );

	pStatic = GetControl<CEtUIStatic>( "ID_TEXT2" );
	strText = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 217 ), static_cast<int>( fExtraExpByMonster * 100.0f) );
	pStatic->SetText( strText );

	pStatic = GetControl<CEtUIStatic>( "ID_TEXT3" );
	strText = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 218 ), static_cast<int>( fExtraExpByStageClear * 100.0f ) );
	pStatic->SetText( strText );
}

#endif // PRE_MOD_SELECT_CHAR