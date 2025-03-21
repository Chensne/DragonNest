#include "Stdafx.h"
#include "DnDungeonLimitDlg.h"
#include "DnTableDB.h"

#ifdef PRE_ADD_STAGE_LIMIT_INTERFACE
CDnDungeonLimitDlg::CDnDungeonLimitDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback, bool bAutoCursor )
:CEtUIDialog( dialogType, pParentDialog, nID, pCallback, bAutoCursor )
{
	for( int i=0; i<eIconType::TYPE_MAX; i++ )
		m_pStaticIcon[i] = NULL;
}

CDnDungeonLimitDlg::~CDnDungeonLimitDlg()
{

}


void CDnDungeonLimitDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName("DungeonEnterLimitDlg.ui").c_str(), bShow );
}

void CDnDungeonLimitDlg::InitialUpdate()
{
	for( int i=0; i<eIconType::TYPE_MAX; i++ )
	{
		m_pStaticIcon[i] = GetControl<CEtUIStatic>( FormatA( "ID_STATIC_ICON%d" , i).c_str() );
		if( m_pStaticIcon[i] ) 
			m_pStaticIcon[i]->Show( false );
	}
}


void CDnDungeonLimitDlg::SetDungeonLimitInfo(int nMapIndex)
{
	for( int i=0; i<eIconType::TYPE_MAX; i++ )
	{
		if( m_pStaticIcon[i] ) 
			m_pStaticIcon[i]->Show( false );
	}

	SUICoord OriginalCoord = m_pStaticIcon[eIconType::TYPE_DAMAGE]->GetUICoord();
	bool bMatched = false;

	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TSTAGEDAMAGELIMIT );
	if( pSox )
	{
		for( int i=0; i<pSox->GetItemCount(); i++ ) 
		{
			int iItemID = pSox->GetItemID( i );
			int nFieldMapIndex = pSox->GetFieldFromLablePtr( iItemID, "MapID" )->GetInteger();
			if( nFieldMapIndex == nMapIndex )
			{
				int nDamageLimit = pSox->GetFieldFromLablePtr( iItemID, "DamageLimit" )->GetInteger();
				int nAttackPowerLimit = pSox->GetFieldFromLablePtr( iItemID, "AttackPowerLimit" )->GetInteger();
				float nHealLimitType1 = pSox->GetFieldFromLablePtr( iItemID, "HealingLimit_1" )->GetFloat();
				float nHealLimitType2 = pSox->GetFieldFromLablePtr( iItemID, "HealingLimit_2" )->GetFloat();

				int nUnlockCount = 0;

				if( nDamageLimit > 0 || nAttackPowerLimit > 0 )
				{
					m_pStaticIcon[eIconType::TYPE_DAMAGE]->Show( true );
					m_pStaticIcon[eIconType::TYPE_DAMAGE]->SetTooltipText( 
						FormatW(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 842 ), nAttackPowerLimit , nDamageLimit ).c_str()  );

					nUnlockCount++;
				}

				if( nHealLimitType1 > 0 || nHealLimitType2 > 0 )
				{
					m_pStaticIcon[eIconType::TYPE_HEAL]->Show( true );
					m_pStaticIcon[eIconType::TYPE_HEAL]->SetTooltipText( 
						FormatW(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 843 ), (nHealLimitType1 * 100) ).c_str()  );

					m_pStaticIcon[eIconType::TYPE_HEAL]->SetPosition( OriginalCoord.fX + (OriginalCoord.fWidth * nUnlockCount), OriginalCoord.fY );
				}

				bMatched = true;
				break;
			}
		}
	}

	Show( bMatched );
}
#endif
