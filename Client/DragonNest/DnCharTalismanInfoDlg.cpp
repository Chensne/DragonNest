#include "Stdafx.h"
#include "DnItemTask.h"
#include "DnInterfaceString.h"
#include "DnUIString.h"
#include "DnTableDB.h"

#if defined(PRE_ADD_TALISMAN_SYSTEM)
#include "DnCharTalismanInfoDlg.h"
#include "DnTalisman.h"
using namespace DN_INTERFACE::STRING;

CDnCharTalismanInfoDlg::CDnCharTalismanInfoDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
: CDnCustomDlg(dialogType, pParentDialog, nID, pCallback)
, m_pTextBox( NULL )
{
}

CDnCharTalismanInfoDlg::~CDnCharTalismanInfoDlg()
{
}

void CDnCharTalismanInfoDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "CharTalismanInfoDlg.ui" ).c_str(), bShow );
}

void CDnCharTalismanInfoDlg::InitialUpdate()
{
	m_pTextBox = GetControl<CEtUITextBox>( "ID_TEXTBOX_INFO" );
}

void CDnCharTalismanInfoDlg::SetText()
{
	m_pTextBox->ClearText();
	m_vFloatTypeList.clear();
	m_vIntTypeList.clear();
	m_vDoubleFloatTypeList.clear();
	m_vDoubleIntTypeList.clear();

	SetStatsInfo();
	//SetMagicChanceInfo();
	//SetGoldChanceInfo();
}

void CDnCharTalismanInfoDlg::SetText(CDnPlayerActor* pActor)
{
	m_pTextBox->ClearText();
	m_vFloatTypeList.clear();
	m_vIntTypeList.clear();
	m_vDoubleFloatTypeList.clear();
	m_vDoubleIntTypeList.clear();

	SetStatsInfo(pActor);
	//SetMagicChanceInfo();
	//SetGoldChanceInfo();
}

void CDnCharTalismanInfoDlg::SetStatsInfo(CDnPlayerActor* pActor)
{
	m_pTextBox->AddText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8044 ), textcolor::FONT_ORANGE );	// UISTRING : 능력치 증가

	for(int i = 0 ; i < TALISMAN_MAX ; ++i)
	{
		CDnTalisman* pTalismanItem = pActor->GetTalisman(i);
		if(pTalismanItem)
			SetItemStateText(pTalismanItem);
	}

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

void CDnCharTalismanInfoDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return ;

	if( bShow )
	{
		RequestFocus( m_pTextBox );
	}

	CEtUIDialog::Show( bShow );
}

void CDnCharTalismanInfoDlg::SetStatsInfo()
{
	m_pTextBox->AddText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8044 ), textcolor::FONT_ORANGE );	// UISTRING : 능력치 증가

	for(int i = 0 ; i < TALISMAN_MAX ; ++i)
	{
		CDnTalisman* pTalismanItem = (CDnTalisman*)GetItemTask().GetTalismanItem(i);

		if(pTalismanItem) {
			int nSlotIndex = pTalismanItem->GetSlotIndex();
			if( nSlotIndex < TALISMAN_BASIC_START ) // CashSlot이라면
			{
				if(GetItemTask().IsOpenCashTalismanSlot())
					SetItemStateText(pTalismanItem);
			}
			else
				SetItemStateText(pTalismanItem);
		}
	}

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

void CDnCharTalismanInfoDlg::SetMagicChanceInfo()
{
	m_pTextBox->AddText(L"");
	m_pTextBox->AddText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1321), textcolor::FONT_ORANGE );// 임시: 매직 아이템
	for(int i = 0 ; i < TALISMAN_MAX ; ++i)
	{
		CDnTalisman* pTalismanItem = (CDnTalisman*)GetItemTask().GetTalismanItem(i);
		if(pTalismanItem) {
			CDnTalisman::eTalismanType eItemType = pTalismanItem->GetTalismanType();
			if(eItemType == CDnTalisman::eTalismanType::TYPE_DROP_PERCENT_UP)
			{
				AddDescription(pTalismanItem->GetClassID());
			}
		}
	}
}

void CDnCharTalismanInfoDlg::SetGoldChanceInfo()
{
	m_pTextBox->AddText(L"");
	m_pTextBox->AddText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8222), textcolor::FONT_ORANGE );// 임시: 수령 가능 골드
	for(int i = 0 ; i < TALISMAN_MAX ; ++i)
	{
		CDnTalisman* pTalismanItem = (CDnTalisman*)GetItemTask().GetTalismanItem(i);
		if(pTalismanItem) {
			CDnTalisman::eTalismanType eItemType = pTalismanItem->GetTalismanType();
			if(eItemType == CDnTalisman::eTalismanType::TYPE_GOLD_PERCENT_UP)
			{
				AddDescription(pTalismanItem->GetClassID());
			}
		}
	}
}

void CDnCharTalismanInfoDlg::AddDescription( int nItemID )
{
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TTALISMANITEM );
	if( !pSox ) return;

	// 어떻게 할지 안정해져서, 골찬/매찬은 툴팁에 안써요.

	//pSox->GetFieldFromLablePtr( nItemID, "_SummaryDescription" )->GetString();
	//MakeUIStringUseVariableParam( wszString, nStringID, szParam );
	//m_pTextBox->AddColorText( wszString.c_str() );
}

void CDnCharTalismanInfoDlg::AddStateText( int nStringID, int eType, float fFirstData, float fSecondData)
{
	bool IsExist = false;
	switch( eType )
	{
	case FLOAT_TYPE :
		{
			for( int itr = 0; itr < (int)m_vFloatTypeList.size(); ++itr )
			{
				if( m_vFloatTypeList[itr].first == nStringID )
				{
					m_vFloatTypeList[itr].second += fFirstData;
					IsExist = true;
					break;
				}
			}

			if( !IsExist )
				m_vFloatTypeList.push_back(std::make_pair(nStringID, fFirstData) );
			break;
		}

	case DOUBLE_FLOAT_TYPE :
		{
			for( int itr = 0; itr < (int)m_vDoubleFloatTypeList.size(); ++itr )
			{
				if( m_vDoubleFloatTypeList[itr].first == nStringID )
				{
					m_vDoubleFloatTypeList[itr].second.first += fFirstData;
					m_vDoubleFloatTypeList[itr].second.second += fSecondData;
					IsExist= true;
					break;
				}
			}

			if( !IsExist )
				m_vDoubleFloatTypeList.push_back(std::make_pair( nStringID, std::make_pair(fFirstData, fSecondData) ) );
			break;
		}

	case INT_TYPE :
		{
			for( int itr = 0; itr < (int)m_vIntTypeList.size(); ++itr )
			{
				if( m_vIntTypeList[itr].first == nStringID )
				{
					m_vIntTypeList[itr].second += (int)fFirstData;
					IsExist = true;
					break;
				}
			}

			if( !IsExist )
				m_vIntTypeList.push_back(std::make_pair(nStringID, (int)fFirstData) );
			break;
		}

	case DOUBLE_INT_TYPE :
		{
			for( int itr = 0; itr < (int)m_vDoubleIntTypeList.size(); ++itr )
			{
				if( m_vDoubleIntTypeList[itr].first == nStringID )
				{
					m_vDoubleIntTypeList[itr].second.first += (int)fFirstData;
					m_vDoubleIntTypeList[itr].second.second += (int)fSecondData;
					IsExist= true;
					break;
				}
			}

			if( !IsExist )
				m_vDoubleIntTypeList.push_back(std::make_pair( nStringID, make_pair<int, int>((int)fFirstData, (int)fSecondData) ) );
			break;
		}
	}
}

void CDnCharTalismanInfoDlg::SetItemStateText( CDnState *pState )
{
	wchar_t szTemp[256]={0};

	if( !pState )
	{
		ASSERT(pState&&"State가 없음");
		return;
	}

	// 공격력/방어력
	if( pState->GetAttackPMin() > 0 || pState->GetAttackPMax() > 0 )
		AddStateText( 5018, DOUBLE_INT_TYPE, (float)pState->GetAttackPMin(), (float)pState->GetAttackPMax() );
	if( pState->GetAttackMMin() > 0 || pState->GetAttackMMax() > 0 )
		AddStateText( 5019, DOUBLE_INT_TYPE, (float)pState->GetAttackMMin(), (float)pState->GetAttackMMax() );
	if( pState->GetDefenseP() > 0 )
		AddStateText( 5020, INT_TYPE, (float)pState->GetDefenseP() );
	if( pState->GetDefenseM() > 0 )
		AddStateText( 5062, INT_TYPE, (float)pState->GetDefenseM() );

	// 기본 스택 정보(랜덤 최고치의 능력치)
	if( pState->GetStrength() > 0 )
		AddStateText( ITEM::STATIC_2_INDEX(emTOOLTIP_STATIC::ITEM_STRENGTH), INT_TYPE, (float)pState->GetStrength() );
	if( pState->GetAgility() > 0 )
		AddStateText( ITEM::STATIC_2_INDEX(emTOOLTIP_STATIC::ITEM_AGILITY), INT_TYPE, (float)pState->GetAgility() );
	if( pState->GetIntelligence() > 0 )
		AddStateText( ITEM::STATIC_2_INDEX(emTOOLTIP_STATIC::ITEM_INTELLIGENCE), INT_TYPE, (float)pState->GetIntelligence() );
	if( pState->GetStamina() > 0 )
		AddStateText( ITEM::STATIC_2_INDEX(emTOOLTIP_STATIC::ITEM_STAMINA), INT_TYPE, (float)pState->GetStamina() );
	if( pState->GetSpirit() > 0 )
		AddStateText( ITEM::STATIC_2_INDEX(emTOOLTIP_STATIC::ITEM_SPIRIT), INT_TYPE, (float)pState->GetSpirit() );

	int nTemp(0);
	INT64 nTempHP = 0;
	// 내부 스탯 증가
	nTemp = pState->GetStiff();
	if( nTemp > 0 )
		AddStateText( ITEM::STATIC_2_INDEX(emTOOLTIP_STATIC::ITEM_STIFFNESS_P), INT_TYPE, (float)pState->GetStiff() );

	nTemp = pState->GetStiffResistance();
	if( nTemp > 0 )
		AddStateText( ITEM::STATIC_2_INDEX(emTOOLTIP_STATIC::ITEM_STIFFNESS_R), INT_TYPE, (float)pState->GetStiffResistance() );

	nTemp = pState->GetStun();
	if( nTemp > 0 )
		AddStateText( ITEM::STATIC_2_INDEX(emTOOLTIP_STATIC::ITEM_STUN_P), INT_TYPE, (float)pState->GetStun() );

	nTemp = pState->GetStunResistance();
	if( nTemp > 0 )
		AddStateText( ITEM::STATIC_2_INDEX(emTOOLTIP_STATIC::ITEM_STUN_R), INT_TYPE, (float)pState->GetStunResistance() );

	nTemp = pState->GetCritical();
	if( nTemp > 0 )
		AddStateText( ITEM::STATIC_2_INDEX(emTOOLTIP_STATIC::ITEM_CRITICAL_P), INT_TYPE, (float)pState->GetCritical() );

	nTemp = pState->GetCriticalResistance();
	if( nTemp > 0 )
		AddStateText( ITEM::STATIC_2_INDEX(emTOOLTIP_STATIC::ITEM_CRITICAL_R), INT_TYPE, (float)pState->GetCriticalResistance() );

	nTemp = pState->GetMoveSpeed();
	if( nTemp > 0 )
		AddStateText( ITEM::STATIC_2_INDEX(emTOOLTIP_STATIC::ITEM_MOVE), INT_TYPE, (float)pState->GetMoveSpeed() );

	nTemp = pState->GetSafeZoneMoveSpeed();
	if( nTemp > 0 )
		AddStateText( ITEM::STATIC_2_INDEX(emTOOLTIP_STATIC::ITEM_SAFEZONEMOVE), INT_TYPE, (float)pState->GetSafeZoneMoveSpeed() );

	nTempHP = pState->GetMaxHP();
	if( nTempHP > 0 )
		AddStateText( 2254, INT_TYPE, (float)pState->GetMaxHP() );

	nTemp = pState->GetMaxSP();
	if( nTemp > 0 )
		AddStateText( 2255, INT_TYPE, (float)pState->GetMaxSP() );

	nTemp = pState->GetRecoverySP();
	if( nTemp > 0 )
		AddStateText( 2256, INT_TYPE, (float)pState->GetRecoverySP() );

	// 슈퍼아머, 파이널데미지
	nTemp = pState->GetSuperAmmor();
	if( nTemp > 0 )
		AddStateText( 2279, INT_TYPE, (float)pState->GetSuperAmmor() );

	nTemp = pState->GetFinalDamage();
	if( nTemp > 0 )
		AddStateText( 2280, INT_TYPE, (float)pState->GetFinalDamage() );

	// 아이템 능력치 표시 두번째. Ratio관련.
	// 공격력/방어력
	if( pState->GetAttackPMinRatio() > 0 || pState->GetAttackPMaxRatio() > 0 )
		AddStateText( 5018, DOUBLE_FLOAT_TYPE, pState->GetAttackPMinRatio() * 100.0f, pState->GetAttackPMaxRatio() *100.0f );
	if( pState->GetAttackMMinRatio() > 0 || pState->GetAttackMMaxRatio() > 0 )
		AddStateText( 5019, DOUBLE_FLOAT_TYPE, pState->GetAttackMMinRatio() * 100.0f, pState->GetAttackMMaxRatio() *100.0f );

	if( pState->GetDefensePRatio() > 0 )
		AddStateText( 5020, FLOAT_TYPE, pState->GetDefensePRatio() * 100.0f );
	if( pState->GetDefenseMRatio() > 0 )
		AddStateText( 5062, FLOAT_TYPE, pState->GetDefenseMRatio() * 100.0f );

	// 기본 스택 정보(랜덤 최고치의 능력치)
	if( pState->GetStrengthRatio() > 0 )
		AddStateText( ITEM::STATIC_2_INDEX(emTOOLTIP_STATIC::ITEM_STRENGTH), FLOAT_TYPE, pState->GetStrengthRatio() * 100.0f );
	if( pState->GetAgilityRatio() > 0 )
		AddStateText( ITEM::STATIC_2_INDEX(emTOOLTIP_STATIC::ITEM_AGILITY), FLOAT_TYPE, pState->GetAgilityRatio() * 100.0f );
	if( pState->GetIntelligenceRatio() > 0 )
		AddStateText( ITEM::STATIC_2_INDEX(emTOOLTIP_STATIC::ITEM_INTELLIGENCE), FLOAT_TYPE, pState->GetIntelligenceRatio() * 100.0f );
	if( pState->GetStaminaRatio() > 0 )
		AddStateText( ITEM::STATIC_2_INDEX(emTOOLTIP_STATIC::ITEM_STAMINA), FLOAT_TYPE, pState->GetStaminaRatio() * 100.0f );
	if( pState->GetSpiritRatio() > 0 )
		AddStateText( ITEM::STATIC_2_INDEX(emTOOLTIP_STATIC::ITEM_SPIRIT), FLOAT_TYPE, pState->GetSpiritRatio() * 100.0f );

	float fTemp(0);

	// 내부 스탯 증가
	fTemp = pState->GetStiffRatio();
	if( fTemp > 0 )
		AddStateText( ITEM::STATIC_2_INDEX(emTOOLTIP_STATIC::ITEM_STIFFNESS_P), FLOAT_TYPE, pState->GetStiffRatio() * 100.0f );

	fTemp = pState->GetStiffResistanceRatio();
	if( fTemp > 0 )
		AddStateText( ITEM::STATIC_2_INDEX(emTOOLTIP_STATIC::ITEM_STIFFNESS_R), FLOAT_TYPE, pState->GetStiffResistanceRatio() * 100.0f );

	fTemp = pState->GetStunRatio();
	if( fTemp > 0 )
		AddStateText( ITEM::STATIC_2_INDEX(emTOOLTIP_STATIC::ITEM_STUN_P), FLOAT_TYPE, pState->GetStunRatio() * 100.0f );

	fTemp = pState->GetStunResistanceRatio();
	if( fTemp > 0 )
		AddStateText( ITEM::STATIC_2_INDEX(emTOOLTIP_STATIC::ITEM_STUN_R), FLOAT_TYPE, pState->GetStunResistanceRatio() * 100.0f );

	fTemp = pState->GetCriticalRatio();
	if( fTemp > 0 )
		AddStateText( ITEM::STATIC_2_INDEX(emTOOLTIP_STATIC::ITEM_CRITICAL_P), FLOAT_TYPE, pState->GetCriticalRatio() * 100.0f );

	fTemp = pState->GetCriticalResistanceRatio();
	if( fTemp > 0 )
		AddStateText( ITEM::STATIC_2_INDEX(emTOOLTIP_STATIC::ITEM_CRITICAL_R), FLOAT_TYPE, pState->GetCriticalResistanceRatio() * 100.0f );

	fTemp = pState->GetMoveSpeedRatio();
	if( fTemp > 0 )
		AddStateText( ITEM::STATIC_2_INDEX(emTOOLTIP_STATIC::ITEM_MOVE), FLOAT_TYPE, pState->GetMoveSpeedRatio() * 100.0f );

	fTemp = pState->GetSafeZoneMoveSpeedRatio();
	if( fTemp > 0 )
		AddStateText( ITEM::STATIC_2_INDEX(emTOOLTIP_STATIC::ITEM_SAFEZONEMOVE), FLOAT_TYPE, pState->GetSafeZoneMoveSpeedRatio() * 100.0f );

	fTemp = pState->GetMaxHPRatio();
	if( fTemp > 0 )
		AddStateText( 2254, FLOAT_TYPE, pState->GetMaxHPRatio() * 100.0f );

	fTemp = pState->GetMaxSPRatio();
	if( fTemp > 0 )
		AddStateText( 2255, FLOAT_TYPE, pState->GetMaxSPRatio() * 100.0f );

	fTemp = pState->GetRecoverySPRatio();
	if( fTemp > 0 )
		AddStateText( 2256, FLOAT_TYPE, pState->GetRecoverySPRatio() * 100.0f );

	// 슈퍼아머, 파이널데미지
	fTemp = pState->GetSuperAmmorRatio();
	if( fTemp > 0 )
		AddStateText( 2279, FLOAT_TYPE, pState->GetSuperAmmorRatio() * 100.0f );

	fTemp = pState->GetFinalDamageRatio();
	if( fTemp > 0 )
		AddStateText( 2280, FLOAT_TYPE, pState->GetFinalDamageRatio() * 100.0f );

	// 속성 공격력/저항 만 별도의 Ratio가 없으므로 아래 출력한다.
	//
	fTemp = ( pState->GetElementAttack( CDnState::Light ) * 100.f );
	if( fTemp > 0 )
		AddStateText( ITEM::STATIC_2_INDEX(emTOOLTIP_STATIC::ITEM_LIGHT_A), FLOAT_TYPE, pState->GetElementAttack(CDnState::Light) * 100.0f );

	fTemp = ( pState->GetElementAttack( CDnState::Dark ) * 100.f );
	if( fTemp > 0 )
		AddStateText( ITEM::STATIC_2_INDEX(emTOOLTIP_STATIC::ITEM_DARK_A), FLOAT_TYPE, pState->GetElementAttack(CDnState::Dark) * 100.0f );

	fTemp = ( pState->GetElementAttack( CDnState::Fire ) * 100.f );
	if( fTemp > 0 )
		AddStateText( ITEM::STATIC_2_INDEX(emTOOLTIP_STATIC::ITEM_FIRE_A), FLOAT_TYPE, pState->GetElementAttack(CDnState::Fire) * 100.0f );

	fTemp = ( pState->GetElementAttack( CDnState::Ice ) * 100.f );
	if( fTemp > 0 )
		AddStateText( ITEM::STATIC_2_INDEX(emTOOLTIP_STATIC::ITEM_ICE_A), FLOAT_TYPE, pState->GetElementAttack(CDnState::Ice) * 100.0f );

	fTemp = ( pState->GetElementDefense( CDnState::Light ) * 100.f );
	if( fTemp > 0 )
		AddStateText( ITEM::STATIC_2_INDEX(emTOOLTIP_STATIC::ITEM_LIGHT_R), FLOAT_TYPE, pState->GetElementDefense(CDnState::Light) * 100.0f );

	fTemp = ( pState->GetElementDefense( CDnState::Dark ) * 100.f );
	if( fTemp > 0 )
		AddStateText( ITEM::STATIC_2_INDEX(emTOOLTIP_STATIC::ITEM_DARK_R), FLOAT_TYPE, pState->GetElementDefense(CDnState::Dark) * 100.0f );

	fTemp = ( pState->GetElementDefense( CDnState::Fire ) * 100.f );
	if( fTemp > 0 )
		AddStateText( ITEM::STATIC_2_INDEX(emTOOLTIP_STATIC::ITEM_FIRE_R), FLOAT_TYPE, pState->GetElementDefense(CDnState::Fire) * 100.0f );

	fTemp = ( pState->GetElementDefense( CDnState::Ice ) * 100.f );
	if( fTemp > 0 )
		AddStateText( ITEM::STATIC_2_INDEX(emTOOLTIP_STATIC::ITEM_ICE_R), FLOAT_TYPE, pState->GetElementDefense(CDnState::Ice) * 100.0f );
}

void CDnCharTalismanInfoDlg::SetSkillInfo()
{
	m_pTextBox->AddText( L"" );
	m_pTextBox->AddText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8045 ), textcolor::FONT_ORANGE );	// UISTRING : 스킬효과 증가

	for( int itr = CDnGlyph::Skill1; itr <= CDnGlyph::Skill4; ++itr )
	{
		CDnGlyph * pGlyph = dynamic_cast<CDnGlyph *>( GetItemTask().GetGlyphItem(itr) );
		if( pGlyph )
			AddDescription( pGlyph->GetClassID() );
	}

	for( int itr = CDnGlyph::Cash1; itr <= CDnGlyph::Cash3; ++itr )
	{
		CDnGlyph * pGlyph = dynamic_cast<CDnGlyph *>( GetItemTask().GetGlyphItem(itr) );
		if( pGlyph && pGlyph->GetGlyphType() == CDnGlyph::PassiveSkill && GetItemTask().IsCashGlyphOpen( itr - CDnGlyph::Cash1 ) )
			AddDescription( pGlyph->GetClassID() );
	}
}

void CDnCharTalismanInfoDlg::SetSpecialSkillInfo()
{
	m_pTextBox->AddText( L"" );
	m_pTextBox->AddText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8046 ), textcolor::FONT_ORANGE );

	CDnGlyph * pGlyph = dynamic_cast<CDnGlyph *>( GetItemTask().GetGlyphItem( CDnGlyph::SpecialSkill1 ) );
	if( pGlyph )
		AddDescription( pGlyph->GetClassID() );

	for( int itr = CDnGlyph::Cash1; itr <= CDnGlyph::Cash3; ++itr )
	{
		CDnGlyph * pGlyph = dynamic_cast<CDnGlyph *>( GetItemTask().GetGlyphItem(itr) );
		if( pGlyph && pGlyph->GetGlyphType() == CDnGlyph::ActiveSkill && GetItemTask().IsCashGlyphOpen( itr - CDnGlyph::Cash1 ) )
			AddDescription( pGlyph->GetClassID() );
	}
}

#endif // PRE_ADD_TALISMAN_SYSTEM
