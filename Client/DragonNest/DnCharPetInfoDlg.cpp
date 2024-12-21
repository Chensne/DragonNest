#include "StdAfx.h"
#include "DnCharPetInfoDlg.h"
#include "DnTableDB.h"
#include "DnItem.h"
#include "DnActor.h"
#include "DnPlayerActor.h"
#include "DnPetTask.h"


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnCharPetInfoDlg::CDnCharPetInfoDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
, m_pTextBox( NULL )
, m_nPetLevelTableID( NULL )
{
}

CDnCharPetInfoDlg::~CDnCharPetInfoDlg()
{
}

void CDnCharPetInfoDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "CharPetInfoDlg.ui" ).c_str(), bShow );
}

void CDnCharPetInfoDlg::InitialUpdate()
{
	m_pTextBox = GetControl<CEtUITextBox>( "ID_TEXTBOX_INFO" );
}

void CDnCharPetInfoDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	CEtUIDialog::Show( bShow );

	if( bShow )
	{
		SetPetAddAbility();
	}
}

void CDnCharPetInfoDlg::SetPetAddAbility()
{
	if( m_nPetLevelTableID == 0 )
		return;

	CDnPlayerActor* pPlayer = (CDnPlayerActor *)CDnActor::s_hLocalActor.GetPointer();
	if( pPlayer == NULL ) return;

	m_pTextBox->ClearText();

	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TPETLEVEL );
	if( !pSox ) return;

	m_pTextBox->AddText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8044 ), textcolor::FONT_ORANGE );
	m_pTextBox->AddText( L"" );

	char szLabel[32];
	WCHAR wszTemp[128] = { 0, };

	std::map<int, int> mapAbilityState;
	std::map<int, int>::iterator iterator;

	for( int i=0; i<10; i++ )
	{
		sprintf_s( szLabel, "_State%d", i + 1 );
		int nStateType = pSox->GetFieldFromLablePtr( m_nPetLevelTableID, szLabel )->GetInteger();
		if( nStateType >= 0 )
		{
			sprintf_s( szLabel, "_State%d_Min", i + 1 );
			char *szStateValue = pSox->GetFieldFromLablePtr( m_nPetLevelTableID, szLabel )->GetString();
			
			int nStateValue = 0;
			if( (nStateType < 50 && !(16 <= nStateType && nStateType <= 23)) || nStateType == 100 )
				nStateValue = abs(atoi(szStateValue));
			else
				nStateValue = (int)(abs(atof(szStateValue)) * 100.0f);

			if( pPlayer->GetPetInfo().nType & Pet::Type::ePETTYPE_SATIETY )
				nStateValue = (int)( (float)nStateValue * GetPetTask().GetSatietyApplyStateRatio() );

			mapAbilityState.insert( make_pair( nStateType, nStateValue ) );
		}
	}

	vector<int> vecPartsItemID;
	for( int i=Pet::Slot::Accessory1; i<Pet::Slot::Max; ++i )
	{
		if( !pPlayer->GetPetInfo().Vehicle[i].nItemID ) continue;
		vecPartsItemID.push_back( pPlayer->GetPetInfo().Vehicle[i].nItemID );
	}

	pSox = GetDNTable( CDnTableDB::TITEM );
	if( pSox == NULL ) return;

	for( int i=0; i<static_cast<int>( vecPartsItemID.size() ); i++ )
	{
		for( int j=0; j<10; j++ )
		{
			sprintf_s( szLabel, "_State%d", j + 1 );
			int nStateType = pSox->GetFieldFromLablePtr( vecPartsItemID[i], szLabel )->GetInteger();
			if( nStateType > 0 )
			{
				sprintf_s( szLabel, "_State%d_Min", j + 1 );
				char *szStateValue = pSox->GetFieldFromLablePtr( vecPartsItemID[i], szLabel )->GetString();

				int nStateValue = 0;
				if( (nStateType < 50 && !(16 <= nStateType && nStateType <= 23)) || nStateType == 100 )
					nStateValue = abs(atoi(szStateValue));
				else
					nStateValue = (int)(abs(atof(szStateValue)) * 100.0f);

				iterator = mapAbilityState.find( nStateType );
				if( iterator != mapAbilityState.end() )
				{
					iterator->second += nStateValue;
				}
				else
				{
					mapAbilityState.insert( make_pair( nStateType, nStateValue ) );
				}
			}
		}
	}
	vecPartsItemID.clear();

	// 펫 세트효과
	pSox = GetDNTable( CDnTableDB::TVEHICLEPARTS );
	if( !pSox ) return;
	int nSetItemID1 = pSox->GetFieldFromLablePtr( pPlayer->GetPetInfo().Vehicle[Pet::Slot::Accessory1].nItemID, "_SetItemID" )->GetInteger();
	int nSetItemID2 = pSox->GetFieldFromLablePtr( pPlayer->GetPetInfo().Vehicle[Pet::Slot::Accessory2].nItemID, "_SetItemID" )->GetInteger();
	if( nSetItemID1 > 0 && nSetItemID2 > 0 && nSetItemID1 == nSetItemID2 )
	{
		DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TSETITEM );
		char szLabel[32];
		int nTemp;
		std::vector<int> nVecStateList;
		for( int i=0; i<12; i++ ) 
		{
			sprintf_s( szLabel, "_NeedSetNum%d", i+1 );
			nTemp = pSox->GetFieldFromLablePtr( nSetItemID1, szLabel )->GetInteger();
			if( nTemp < 2 ) continue;
			nVecStateList.push_back(i);
		}

		for( DWORD i=0; i<nVecStateList.size(); i++ ) 
		{
			sprintf_s( szLabel, "_State%d", nVecStateList[i] + 1 );
			int nStateType = pSox->GetFieldFromLablePtr( nSetItemID1, szLabel )->GetInteger();
			if( nStateType > 0 )
			{
				sprintf_s( szLabel, "_State%dValue", nVecStateList[i] + 1 );
				char *szStateValue = pSox->GetFieldFromLablePtr( nSetItemID1, szLabel )->GetString();

				int nStateValue = 0;
				if( (nStateType < 50 && !(16 <= nStateType && nStateType <= 23)) || nStateType == 100 )
					nStateValue = abs(atoi(szStateValue));
				else
					nStateValue = (int)(abs(atof(szStateValue)) * 10000.0f);

				iterator = mapAbilityState.find( nStateType );
				if( iterator != mapAbilityState.end() )
				{
					iterator->second += nStateValue;
				}
				else
				{
					mapAbilityState.insert( make_pair( nStateType, nStateValue ) );
				}
			}
		}
	}
	
	for( iterator = mapAbilityState.begin(); iterator != mapAbilityState.end(); iterator++ ) 
	{
		int nStateType = iterator->first;
		int nStateValue = iterator->second;

		if( (nStateType < 50 && !(16 <= nStateType && nStateType <= 23)) || nStateType == 100 )		// + State
		{
			if( nStateType == 100 ) nStateType = 30;		// 근성
			swprintf_s( wszTemp, L"%s %c%d", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, CDnItem::GetStateValueMatchUIStringIndex(nStateType) ), '+', nStateValue );
		}
		else	// % State
		{
			if( nStateType >= 50 ) nStateType -= 50;
			float fValue = (float)(nStateValue) / 100.0f;
			swprintf_s( wszTemp, L"%s %c%.2f%%\n", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, CDnItem::GetStateValueMatchUIStringIndex(nStateType) ), '+', fValue );
		}
		m_pTextBox->AddText( wszTemp );
	}

	mapAbilityState.clear();
}

