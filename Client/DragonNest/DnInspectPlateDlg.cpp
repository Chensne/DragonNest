#include "stdafx.h"
#include "DnInspectPlateDlg.h"
#include "DnTooltipPlateDlg.h"
#include "DnTableDB.h"
#include "DnInspectPlayerDlg.h"
#include "DnActor.h"
#include "DnPlayerActor.h"
#include "DnUIString.h"
#include "DnInterfaceString.h"


//////////////////////////////////////////////////////////////////////////
// CDnInspectPlateDlg

CDnInspectPlateDlg::CDnInspectPlateDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback )
, m_pInspectPlateInfoDlg( NULL )
{
	m_VecGlyphSlotButton.reserve( 16 );
}

CDnInspectPlateDlg::~CDnInspectPlateDlg()
{
	SAFE_DELETE( m_pInspectPlateInfoDlg );
}

void CDnInspectPlateDlg::Initialize(bool bShow)
{
	CDnCustomDlg::Initialize( CEtResourceMng::GetInstance().GetFullName( "PlayerInfoPlateDlg.ui" ).c_str(), bShow );
}

void CDnInspectPlateDlg::InitialUpdate()
{
	m_pInspectPlateInfoDlg = new CDnInspectPlateInfoDlg;
	m_pInspectPlateInfoDlg->Initialize( false );

	char szControlName[32];
	for( int itr = 0; itr < GLYPHMAX; ++itr )
	{
		sprintf_s( szControlName, _countof(szControlName), "ID_STATIC_COVER%d", itr );
		m_VecGlyphCover.push_back( GetControl<CEtUIStatic>(szControlName) );
		GetControl<CEtUIStatic>(szControlName)->Show( false );
	}
}

void CDnInspectPlateDlg::InitCustomControl( CEtUIControl *pControl )
{
	CDnItemSlotButton *pItemSlotButton( NULL );
	pItemSlotButton = static_cast<CDnItemSlotButton*>(pControl);

	if( strstr( pControl->GetControlName(), "ID_ITEM_PLATE" ) ) 
	{
		pItemSlotButton->SetSlotIndex( static_cast<int>( m_VecGlyphSlotButton.size() ) );
		pItemSlotButton->SetSlotType( ST_CHARSTATUS );
		m_VecGlyphSlotButton.push_back( pItemSlotButton );
	}
}

void CDnInspectPlateDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	if( bShow )
		RefreshGlyphEquip();
	else
		m_pInspectPlateInfoDlg->Show( false );

	CEtUIDialog::Show( bShow );
}

void CDnInspectPlateDlg::RefreshGlyphEquip()
{
	CDnInspectPlayerDlg* pParentInspectPlayerDlg = dynamic_cast<CDnInspectPlayerDlg*>( GetParentDialog() );
	if( !pParentInspectPlayerDlg ) return;

	DnActorHandle hActor = CDnActor::FindActorFromUniqueID( pParentInspectPlayerDlg->GetSessionID() );
	if( !hActor ) return;

	CDnPlayerActor* pActor = dynamic_cast<CDnPlayerActor*>( hActor.GetPointer() );
	if( !pActor ) return;

	for( DWORD i=0; i<GLYPHMAX; i++ )
	{
		SetGlyphEquipItem( i, pActor->GetGlyph( (CDnGlyph::GlyphSlotEnum)i ) );
	}
}

void CDnInspectPlateDlg::SetGlyphEquipItem( int nEquipIndex, MIInventoryItem *pItem )
{
	int nSlotIndex = GLYPH_EQUIPINDEX_2_SLOTINDEX(nEquipIndex);
	if( nSlotIndex == -1 ) return;

	m_VecGlyphSlotButton[nSlotIndex]->SetItem( pItem, CDnSlotButton::ITEM_ORIGINAL_COUNT );
	m_VecGlyphSlotButton[nSlotIndex]->OnRefreshTooltip();

	CDnInspectPlayerDlg* pParentInspectPlayerDlg = dynamic_cast<CDnInspectPlayerDlg*>( GetParentDialog() );
	if( pParentInspectPlayerDlg )
		m_pInspectPlateInfoDlg->SetText( pParentInspectPlayerDlg->GetSessionID() );
}

int CDnInspectPlateDlg::GLYPH_EQUIPINDEX_2_SLOTINDEX( int nEquipIndex )
{
	// 0, 1, 2, 4, 3, 5, 6, 7, 8, 9
	// 6, 7, 5, 8, 12, 9, 11, 10
	switch( nEquipIndex ) {
		case GLYPH_ENCHANT1: return 5;
		case GLYPH_ENCHANT2: return 6;
		case GLYPH_ENCHANT3: return 7;
		case GLYPH_ENCHANT4: return 8;
		case GLYPH_ENCHANT5: return 9;
		case GLYPH_ENCHANT6: return 10;
		case GLYPH_ENCHANT7: return 11;
		case GLYPH_ENCHANT8: return 12;
		case GLYPH_SKILL1:	 return 1;
		case GLYPH_SKILL2:	 return 2;
		case GLYPH_SKILL3:	 return 3;
		case GLYPH_SKILL4:	 return 4;
		case GLYPH_SPECIALSKILL: return 0;
		case GLYPH_CASH1: return 13;
		case GLYPH_CASH2: return 14;
		case GLYPH_CASH3: return 15;
#if defined(PRE_ADD_DRAGON_FELLOWSHIP_GLYPH)
		case GLYPH_DRAGON1:	return 16;
		case GLYPH_DRAGON2:	return 17;
		case GLYPH_DRAGON3:	return 18;
		case GLYPH_DRAGON4:	return 19;
#endif	// #if defined(PRE_ADD_DRAGON_FELLOWSHIP_GLYPH)
	}
	return 0;
}

void CDnInspectPlateDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED && IsCmdControl( "ID_BUTTON_INFO" ) )
	{
		if( m_pInspectPlateInfoDlg->IsShow() )
			m_pInspectPlateInfoDlg->Show( false );
		else
		{
			CDnInspectPlayerDlg* pParentInspectPlayerDlg = dynamic_cast<CDnInspectPlayerDlg*>( GetParentDialog() );
			if( pParentInspectPlayerDlg )
			{
				m_pInspectPlateInfoDlg->SetText( pParentInspectPlayerDlg->GetSessionID() );
				m_pInspectPlateInfoDlg->Show( true );
			}
		}
	}
}




//////////////////////////////////////////////////////////////////////////
// CDnInspectPlateInfoDlg

CDnInspectPlateInfoDlg::CDnInspectPlateInfoDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCharPlateInfoDlg( dialogType, pParentDialog, nID, pCallback )
{
}

CDnInspectPlateInfoDlg::~CDnInspectPlateInfoDlg()
{
}

void CDnInspectPlateInfoDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "PlayerInfoPlateInfoDlg.ui" ).c_str(), bShow );
}

void CDnInspectPlateInfoDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	if( bShow ) SetRenderPriority( this, true );
	CDnCharPlateInfoDlg::Show( bShow );
}

void CDnInspectPlateInfoDlg::SetText( int nSessionID )
{
	m_pTextBox->ClearText();
	m_vFloatTypeList.clear();
	m_vIntTypeList.clear();
	m_vDoubleFloatTypeList.clear();
	m_vDoubleIntTypeList.clear();

	DnActorHandle hActor = CDnActor::FindActorFromUniqueID( nSessionID );
	if( !hActor ) return;

	CDnPlayerActor* pActor = dynamic_cast<CDnPlayerActor*>( hActor.GetPointer() );
	if( !pActor ) return;

	SetEnchatInfo( pActor );
	SetSkillInfo( pActor );
	SetSpecialSkillInfo( pActor );
}

void CDnInspectPlateInfoDlg::SetEnchatInfo( CDnPlayerActor * pActor )
{
	m_pTextBox->AddText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8044 ), textcolor::FONT_ORANGE );	// UISTRING : 능력치 증가

	for( int itr = CDnGlyph::Enchant1; itr <= CDnGlyph::Enchant8; ++itr )
	{
		CDnGlyph * pGlyph = pActor->GetGlyph( (CDnGlyph::GlyphSlotEnum)itr );
		if( pGlyph )
			SetItemStateText( pGlyph );
	}

	for( int itr = CDnGlyph::Cash1; itr <= CDnGlyph::Cash3; ++itr )
	{
		CDnGlyph * pGlyph = pActor->GetGlyph( (CDnGlyph::GlyphSlotEnum)itr );
		if( pGlyph && pGlyph->GetGlyphType() == CDnGlyph::Enchant )
			SetItemStateText( pGlyph );
	}

#if defined(PRE_ADD_DRAGON_FELLOWSHIP_GLYPH)
	for( int itr = CDnGlyph::Dragon1; itr <= CDnGlyph::Dragon4; ++itr )
	{
		CDnGlyph * pGlyph = pActor->GetGlyph( (CDnGlyph::GlyphSlotEnum)itr );
		if( pGlyph )
			SetItemStateText( pGlyph );
	}
#endif	// #if defined(PRE_ADD_DRAGON_FELLOWSHIP_GLYPH)

	WCHAR szTemp[256] = {0,};
	for( DWORD itr = 0; itr < m_vIntTypeList.size(); ++itr )
	{
		swprintf_s( szTemp, 256, L"%s : %d", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, m_vIntTypeList[itr].first ), m_vIntTypeList[itr].second );
		m_pTextBox->AddText( szTemp );
	}
	for( DWORD itr = 0; itr < m_vDoubleIntTypeList.size(); ++itr )
	{
		swprintf_s( szTemp, 256, L"%s : %d%s%d", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, m_vDoubleIntTypeList[itr].first ), m_vDoubleIntTypeList[itr].second.first, TILDE, m_vDoubleIntTypeList[itr].second.second );
		m_pTextBox->AddText( szTemp );
	}
	for( DWORD itr = 0; itr < m_vFloatTypeList.size(); ++itr )
	{
		swprintf_s( szTemp, 256, L"%s : %.1f%%", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, m_vFloatTypeList[itr].first ), m_vFloatTypeList[itr].second );
		m_pTextBox->AddText( szTemp );
	}
	for( DWORD itr = 0; itr < m_vDoubleFloatTypeList.size(); ++itr )
	{
		swprintf_s( szTemp, 256, L"%s : %.1f%%%s%.1f%%", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, m_vDoubleFloatTypeList[itr].first ), m_vDoubleFloatTypeList[itr].second.first, TILDE, m_vDoubleFloatTypeList[itr].second.second );
		m_pTextBox->AddText( szTemp );
	}
}

void CDnInspectPlateInfoDlg::SetSkillInfo( CDnPlayerActor * pActor )
{
	m_pTextBox->AddText( L"" );
	m_pTextBox->AddText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8045 ), textcolor::FONT_ORANGE );	// UISTRING : 스킬효과 증가

	for( int itr = CDnGlyph::Skill1; itr <= CDnGlyph::Skill4; ++itr )
	{
		CDnGlyph * pGlyph = pActor->GetGlyph( (CDnGlyph::GlyphSlotEnum)itr );
		if( pGlyph )
			AddDescription( pGlyph->GetClassID() );
	}

	for( int itr = CDnGlyph::Cash1; itr <= CDnGlyph::Cash3; ++itr )
	{
		CDnGlyph * pGlyph = pActor->GetGlyph( (CDnGlyph::GlyphSlotEnum)itr );
		if( pGlyph && pGlyph->GetGlyphType() == CDnGlyph::PassiveSkill )
			AddDescription( pGlyph->GetClassID() );
	}
}

void CDnInspectPlateInfoDlg::SetSpecialSkillInfo( CDnPlayerActor * pActor )
{
	m_pTextBox->AddText( L"" );
	m_pTextBox->AddText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8046 ), textcolor::FONT_ORANGE );

	CDnGlyph * pGlyph = pActor->GetGlyph( CDnGlyph::SpecialSkill1 );
	if( pGlyph )
		AddDescription( pGlyph->GetClassID() );

	for( int itr = CDnGlyph::Cash1; itr <= CDnGlyph::Cash3; ++itr )
	{
		CDnGlyph * pGlyph = pActor->GetGlyph( (CDnGlyph::GlyphSlotEnum)itr );
		if( pGlyph && pGlyph->GetGlyphType() == CDnGlyph::ActiveSkill )
			AddDescription( pGlyph->GetClassID() );
	}
}

void CDnInspectPlateInfoDlg::AddDescription( int nTableID )
{
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TGLYPHSKILL );
	if( !pSox ) return;

	std::wstring wszString;
	char * str = pSox->GetFieldFromLablePtr( nTableID, "_SummaryDescription" )->GetString();
	int nStringID = atoi( str );
	char * szParam = pSox->GetFieldFromLablePtr( nTableID, "_SummaryDescriptionParam" )->GetString();

	MakeUIStringUseVariableParam( wszString, nStringID, szParam );
	m_pTextBox->AddColorText( wszString.c_str() );
}