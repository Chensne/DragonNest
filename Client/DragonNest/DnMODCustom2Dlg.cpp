#include "StdAfx.h"
#include "DnMODCustom2Dlg.h"
#include "DnTrigger.h"
#include "DnGameTask.h"

#ifdef PRE_ADD_MONSTER_PARTS_UI_TRIGGER
#include "DnPartsMonsterActor.h"
#endif

CDnMODCustom2Dlg::CDnMODCustom2Dlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback, bool bAutoCursor )
: CDnMODDlgBase( dialogType, pParentDialog, nID, pCallback, bAutoCursor )
{
}

CDnMODCustom2Dlg::~CDnMODCustom2Dlg()
{
	SAFE_DELETE_VEC( m_pVecStaticText );
	SAFE_DELETE_VEC( m_pVecStaticProgress );
}

void CDnMODCustom2Dlg::InitialUpdate()
{
	char szStr[64];
	for( int i=0; ; i++ ) {
		bool bExist;
		CEtUIStatic *pStatic;

		sprintf_s( szStr, "ID_TEXT%d",  i+1 );
		pStatic = GetControl<CEtUIStatic>( szStr, &bExist );

		if( !bExist ) break;
		m_pVecStaticText.push_back( pStatic );
	}

	for( int i=0; ; i++ ) {
		bool bExist;
		CEtUIProgressBar *pStatic;

		sprintf_s( szStr, "ID_PROGRESS%d",  i+1 );
		pStatic = GetControl<CEtUIProgressBar>( szStr, &bExist );

		if( !bExist ) break;
		m_pVecStaticProgress.push_back( pStatic );
	}

}

void CDnMODCustom2Dlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( m_szUIFileName ).c_str(), bShow );
}

void CDnMODCustom2Dlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	CDnMODDlgBase::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnMODCustom2Dlg::Show( bool bShow )
{
	CDnMODDlgBase::Show( bShow );
}
// 여기서 파츠ID 값 넘겨주고 파츠를 얻어오는 함수 만들고.
// 해당 링크 Value 값으로 파츠 몬스터의 HP 와 SUPERARMOR 를 넣어주게 하자.

void CDnMODCustom2Dlg::Process( float fElapsedTime )
{
	int nValueCount = (int)GetLinkValueCount();
	if( nValueCount % 2 != 0 ) {
		for( DWORD i=0; i<m_pVecStaticText.size(); i++ ) m_pVecStaticText[i]->Show( false );
		for( DWORD i=0; i<m_pVecStaticProgress.size(); i++ ) m_pVecStaticProgress[i]->Show( false );
		return;
	}

	for( int i=0; i<nValueCount; i+=2 ) {
		LinkValueStruct *pStruct1 = GetLinkValue(i);
		LinkValueStruct *pStruct2 = GetLinkValue(i+1);

		std::wstring wszText = L"";
		INT64 nValue = 0, nValueMax = 0;
		DnActorHandle hActor = CDnActor::FindActorFromUniqueID( (DWORD)pStruct1->nDefineValueIndex );
#ifdef PRE_ADD_MONSTER_PARTS_UI_TRIGGER	
		if( hActor ) 
		{	
			switch( pStruct2->nDefineValueIndex )
			{
			case 0 : 
				if( pStruct1->nGaugeType == eHP )
				{
					nValueMax = hActor->GetMaxHP(); 
					nValue = hActor->GetHP();
					wszText = hActor->GetName();	
				}
				else if( pStruct1->nGaugeType == ePartsHP )
				{
					CDnPartsMonsterActor* pPartsMonsterActor = NULL;
					MonsterParts* pParts = NULL;
					if( hActor->IsPartsMonsterActor() )
					{
						pPartsMonsterActor = static_cast<CDnPartsMonsterActor*>(hActor.GetPointer());
						pParts = pPartsMonsterActor->GetPartsByIndex( pStruct1->nPartsIndex );
						nValueMax = pParts->GetMaxHP();
						nValue = pParts->GetHP();
						wszText = pParts->GetPartsName();	
					}
				}
				else if( pStruct1->nGaugeType == eSuperArmor )
				{
					nValueMax = hActor->GetMaxSuperArmor();
					nValue = hActor->GetCurrentSuperArmor();
					wszText = hActor->GetName();	
				}
				break;
			}
		}
#else
		if( hActor ) {
			wszText = hActor->GetName();
			switch( pStruct2->nDefineValueIndex ) {
				case 0: 
					nValueMax = hActor->GetMaxHP(); 
					nValue = hActor->GetHP();
					break;
			}
		}
#endif 
		int nControlIndex = i / 2;
		CEtUIStatic *pStatic = ( nControlIndex >= (int)m_pVecStaticText.size() ) ? NULL : m_pVecStaticText[nControlIndex];
		CEtUIProgressBar *pProgress = ( nControlIndex >= (int)m_pVecStaticProgress.size() ) ? NULL : m_pVecStaticProgress[nControlIndex];
		if( pStatic ) pStatic->SetText( wszText );
		if( pProgress ) pProgress->SetProgress( ( 100.f / (float)nValueMax ) * (float)nValue );
	}
	CDnMODDlgBase::Process( fElapsedTime );
}

void CDnMODCustom2Dlg::Render( float fElapsedTime )
{
	CDnMODDlgBase::Render( fElapsedTime );
}