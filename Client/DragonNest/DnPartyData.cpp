#include "StdAfx.h"
#include "DnPartyData.h"
#include "DNTableFile.h"
#include "DnTableDB.h"
#include "DNIsolate.h"
#include "DnInterfaceString.h"
#include "DnInterface.h"
#include "DnCommonUtil.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

SPartyInfoList::SPartyInfoList()
	: m_Number(0)
	, m_pSecret(NULL)
	, m_bSecret(false)
	, m_pTitle(NULL)
	, m_pCount(NULL)
	, m_pBase(NULL)
	, m_pUserLevel(NULL)
	, m_pTargetStage(NULL)
	, m_pTargetDifficulty(NULL)
	, m_pVoice(NULL)
#ifdef PRE_PARTY_DB
	, m_pBonusExpParty(NULL)
#endif
#ifdef PRE_WORLDCOMBINE_PARTY
	, m_nWorldCombinePartyID(0)
	, m_pWorldCombinePartyBonus(NULL)
#endif
#ifdef PRE_ADD_NEWCOMEBACK
	,m_pComebackUser(NULL)	
#endif // PRE_ADD_NEWCOMEBACK
{
	SecureZeroMemory( m_wszTemp, sizeof(m_wszTemp) );
}

void SPartyInfoList::InitInfoList()
{
	m_Number = 0;

	m_pSecret->Show(false);
	m_bSecret = false;

	m_pTitle->ClearText();
	m_pCount->ClearText();
	m_pBase->Show(true);
	m_pUserLevel->ClearText();
	m_pTargetStage->ClearText();
	m_pTargetDifficulty->ClearText();
	m_pVoice->Show(false);
#ifdef PRE_PARTY_DB
	std::vector<SPartyVillageIconData>::iterator iter = m_VillageIconList.begin();
	for (; iter != m_VillageIconList.end(); ++iter)
	{
		SPartyVillageIconData& data = (*iter);
		data.pIcon->Show(false);
	}
	m_pBonusExpParty->Show(false);
#endif
#ifdef PRE_WORLDCOMBINE_PARTY
	m_pWorldCombinePartyBonus->Show(false);
	m_nWorldCombinePartyID = Party::Constants::INVALID_WORLDCOMBINE_TABLE_INDEX;
#endif
#ifdef PRE_ADD_NEWCOMEBACK
	m_pComebackUser->Show( false );
#endif // PRE_ADD_NEWCOMEBACK
}

#ifdef PRE_WORLDCOMBINE_PARTY
void SPartyInfoList::SetControl( CEtUIStatic* pBase, CEtUIStatic *pSecret, CEtUIStatic *pTitle, CEtUIStatic *pCount, CEtUIStatic *pUserLevel, 
								CEtUIStatic* pStage, CEtUIStatic* pDifficulty, CEtUIStatic* pVoice, CEtUIStatic* pBonusExpParty, 
								CEtUIStatic* pWorldCombinePartyBonus
							#ifdef PRE_ADD_NEWCOMEBACK
								,CEtUIStatic* pComebackUser
							#endif // PRE_ADD_NEWCOMEBACK
)
{
	_ASSERT(pBase&&pSecret&&pTitle&&pCount&&pUserLevel&&"SPartyInfoList::SetControl");
	m_pSecret = pSecret;
	m_pTitle = pTitle;
	m_pCount = pCount;
	m_pBase = pBase;
	m_pUserLevel = pUserLevel;
	m_pTargetStage = pStage;
	m_pTargetDifficulty = pDifficulty;
	m_pVoice = pVoice;
	m_pBonusExpParty = pBonusExpParty;
	m_pBonusExpParty->Show(false); // #58237 보너스 경험치 표시 않기로 결정
	m_pWorldCombinePartyBonus = pWorldCombinePartyBonus;

#ifdef PRE_ADD_NEWCOMEBACK
	m_pComebackUser = pComebackUser;	
#endif // PRE_ADD_NEWCOMEBACK

}
#else // PRE_WORLDCOMBINE_PARTY
		#ifdef PRE_PARTY_DB
		void SPartyInfoList::SetControl( CEtUIStatic* pBase, CEtUIStatic *pSecret, CEtUIStatic *pTitle, CEtUIStatic *pCount, CEtUIStatic *pUserLevel, 
										CEtUIStatic* pStage, CEtUIStatic* pDifficulty, CEtUIStatic* pVoice, CEtUIStatic* pBonusExpParty
									#ifdef PRE_ADD_NEWCOMEBACK
										,CEtUIStatic* pComebackUser
									#endif // PRE_ADD_NEWCOMEBACK)
		#else
		void SPartyInfoList::SetControl( CEtUIStatic* pBase, CEtUIStatic *pSecret, CEtUIStatic *pTitle, CEtUIStatic *pCount, CEtUIStatic *pUserLevel, CEtUIStatic* pStage, CEtUIStatic* pDifficulty, CEtUIStatic* pVoice
										#ifdef PRE_ADD_NEWCOMEBACK
											,CEtUIStatic* pComebackUser
										#endif // PRE_ADD_NEWCOMEBACK
		)
		#endif
		{
			_ASSERT(pBase&&pSecret&&pTitle&&pCount&&pUserLevel&&"SPartyInfoList::SetControl");
			m_pSecret = pSecret;
			m_pTitle = pTitle;
			m_pCount = pCount;
			m_pBase = pBase;
			m_pUserLevel = pUserLevel;
			m_pTargetStage = pStage;
			m_pTargetDifficulty = pDifficulty;
			m_pVoice = pVoice;
		#ifdef PRE_PARTY_DB
			m_pBonusExpParty = pBonusExpParty;
			m_pBonusExpParty->Show(false); // #58237 보너스 경험치 표시 않기로 결정
		#endif

#ifdef PRE_ADD_NEWCOMEBACK
			m_pComebackUser = pComebackUser;			
#endif // PRE_ADD_NEWCOMEBACK
		}
#endif // PRE_WORLDCOMBINE_PARTY

#ifdef PRE_PARTY_DB
void SPartyInfoList::InitializeVillageIconControl(int nVillageMapID, CEtUIStatic* pVillageIcon)
{
	if (pVillageIcon == NULL || nVillageMapID <= 0)
	{
		_ASSERT(0);
		return;
	}

	SPartyVillageIconData data;
	data.nVillageMapID = nVillageMapID;
	data.pIcon = pVillageIcon;

	std::vector<SPartyVillageIconData>::iterator iter = std::find(m_VillageIconList.begin(), m_VillageIconList.end(), data);
	if (iter != m_VillageIconList.end())
	{
		_ASSERT(0);
		return;
	}

	m_VillageIconList.push_back(data);
	pVillageIcon->Show(false);
}

void SPartyInfoList::ShowVillageIcon(int nVillageMapID)
{
	std::vector<SPartyVillageIconData>::iterator iter = m_VillageIconList.begin();
	for (; iter != m_VillageIconList.end(); ++iter)
	{
		SPartyVillageIconData& data = (*iter);
		if (data.pIcon)
			data.pIcon->Show(data.pIcon && (data.nVillageMapID == nVillageMapID));
	}
}
#endif

bool SPartyInfoList::IsInsideItem( float fX, float fY )
{
	SUICoord uiCoords;

	m_pBase->GetUICoord(uiCoords);

	return uiCoords.IsInside( fX, fY );
}

void SPartyInfoList::SetNumber( TPARTYID Number )
{
	m_Number = Number;
}

void SPartyInfoList::SetSecret( bool bSecret )
{
	m_bSecret = bSecret;
	m_pSecret->Show(m_bSecret);
}

#if defined (PRE_PARTY_DB) && defined (_WORK)
void SPartyInfoList::SetDebugTitle( LPCWSTR wszTitle, INT64 sortPoint, ePartyType type ) 
{ 
#ifdef PRE_WORLDCOMBINE_PARTY
	std::wstring str;
	if (type == _WORLDCOMBINE_PARTY)
	{
		str = FormatW(L"(%I64d)[WorldCombine] %s", sortPoint, wszTitle);
		m_pTitle->SetTextColor(textcolor::ORANGERED, true);
	}
	else
	{
		str = FormatW(L"(%I64d)%s", sortPoint, wszTitle);
		m_pTitle->SetTextColor(textcolor::WHITE);
	}
#else
	std::wstring str = FormatW(L"(%I64d)%s", sortPoint, wszTitle);
#endif
	m_pTitle->SetText( str.c_str() );
}
#endif

#ifdef PRE_WORLDCOMBINE_PARTY
void SPartyInfoList::SetWorldCombineParty(bool bSet, const SWorldCombinePartyData& combineData)
{
	if (bSet)
	{
		m_pTitle->SetTextColor(textcolor::ORANGE, true);
		m_pTargetStage->SetTextColor(textcolor::ORANGE, true);
		m_pUserLevel->SetTextColor(textcolor::ORANGE, true);
		m_pCount->SetTextColor(textcolor::ORANGE, true);

		if (m_pWorldCombinePartyBonus == NULL)
			return;

		if (combineData.bHasPartyBuff)
		{
			SetWorldCombineBonus(true);
			m_pWorldCombinePartyBonus->SetTooltipText(combineData.partyBuffTooltipString.c_str());
		}
	}
	else
	{
		m_pTitle->SetTextColor(textcolor::WHITE);
		m_pTargetStage->SetTextColor(textcolor::WHITE);
		m_pUserLevel->SetTextColor(textcolor::WHITE);
		m_pCount->SetTextColor(textcolor::WHITE);
	}
}
#endif

void SPartyInfoList::SetTitle( LPCWSTR wszTitle ) 
{
	m_pTitle->SetText( wszTitle ); 
}

void SPartyInfoList::SetCount( int nCurCount, int nMaxCount ) 
{
	swprintf_s( m_wszTemp, 80, L"%d/%d", nCurCount, nMaxCount );
	m_pCount->SetText( m_wszTemp ); 
}

void SPartyInfoList::SetWorldLevel( int nLevel ) 
{ 
	swprintf_s( m_wszTemp, 80, L"%s %d", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5006 ), nLevel );
}

#ifdef PRE_PARTY_DB
void SPartyInfoList::SetUserLevel(int nMinLevel)
{
	if (nMinLevel <= 1)
		swprintf_s(m_wszTemp, 80, L"%s%s", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 92), GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 93)); // UISTRING : 제한, 없음
	else
		swprintf_s(m_wszTemp, 80, L"%s %d %s", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 64), nMinLevel, GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1326)); // UISTRING : 레벨, 이상

	m_pUserLevel->SetTextWithEllipsis( m_wszTemp, L"..." );
}

void SPartyInfoList::SetBonusExpParty(bool bSet)
{
	m_pBonusExpParty->Show(bSet);
}

#ifdef PRE_ADD_NEWCOMEBACK
void SPartyInfoList::SetComebackUser( bool bShow )
{
	if( m_pComebackUser )
		m_pComebackUser->Show( bShow );	
}
#endif // PRE_ADD_NEWCOMEBACK

bool SPartyInfoList::IsEmpty() const
{
	if (m_pTitle == NULL)
		return true;

	const std::wstring& title = m_pTitle->GetText();
	return (title.empty());
}

#ifdef PRE_WORLDCOMBINE_PARTY
void SPartyInfoList::SetWorldCombineBonus(bool bSet)
{
	m_pWorldCombinePartyBonus->Show(bSet);
}
#endif

#else // PRE_PARTY_DB
void SPartyInfoList::SetUserLevel( int nMinLevel, int nMaxLevel )
{
	if( nMinLevel == 1 && nMaxLevel == 100 )
	{
		swprintf_s( m_wszTemp, 80, L"%s%s", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 92 ), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 93 ) );
	}
	else
	{
#if defined(_US)
		swprintf_s( m_wszTemp, 80, L"%s %d-%d", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 64 ), nMinLevel, nMaxLevel );
#else
		swprintf_s( m_wszTemp, 80, L"%s %d~%d", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 64 ), nMinLevel, nMaxLevel );
#endif
	}

	m_pUserLevel->SetText( m_wszTemp );
}
#endif // PRE_PARTY_DB

void SPartyInfoList::Enable(bool bEnable)
{
	m_pTitle->Enable(bEnable);
	m_pSecret->Enable(bEnable);
	m_pCount->Enable(bEnable);
	m_pBase->Enable(bEnable);
	m_pUserLevel->Enable(bEnable);
	m_pTargetStage->Enable(bEnable);
	m_pTargetDifficulty->Enable(bEnable);
#ifdef PRE_WORLDCOMBINE_PARTY
	m_pWorldCombinePartyBonus->Enable(bEnable);
#endif
}

void SPartyInfoList::Clear()
{
	InitInfoList();
}

void SPartyInfoList::SetTargetStage(const std::wstring& stage)
{
	std::wstring whole;
	whole = stage.empty() ? L"" : stage.c_str();

	m_pTargetStage->SetTextWithEllipsis(whole.c_str(), L"...");
}

void SPartyInfoList::SetTargetDifficulty(TDUNGEONDIFFICULTY difficultyIdx)
{
	std::wstring whole;
	DWORD color = 0xffffffff;

#ifdef PRE_PARTY_DB
	if (difficultyIdx < Dungeon::Difficulty::Max)
#else
	if (difficultyIdx > 0)
#endif
	{
		DN_INTERFACE::STRING::GetStageDifficultyText(whole, difficultyIdx);
		color = DN_INTERFACE::STRING::GetStageDifficultyColor(difficultyIdx);
	}

	m_pTargetDifficulty->SetTextColor(color);
	m_pTargetDifficulty->SetText(whole.c_str());
}

void SPartyInfoList::SetVoiceChat(bool bVoiceChat)
{
	m_pVoice->Show(bVoiceChat);
}


SPartyMemberData::SPartyMemberData()
: nUserLevel(0)
, nSlotIndex(-1)
, bMaster(false)
, nClass(0)
, nWorldLevel(0)
, nSessionID(0)
, nFatigueValue(0)
, dwNameColor(0)
#ifdef PRE_ADD_NEWCOMEBACK
, bComeback(false)
#endif // PRE_ADD_NEWCOMEBACK
{
	memset(m_wszTemp, 0, sizeof(m_wszTemp));
}

void SPartyMemberData::InitInfoList()
{
	nSlotIndex = -1;
	nUserLevel = 0;
	bMaster = false;
	nClass = 0;
	nWorldLevel = 0;
	nSessionID = 0;
	nFatigueValue = 0;
}

void SPartyMemberData::SetSlotIndex(int nIdx)
{
	nSlotIndex = nIdx;
}

void SPartyMemberData::SetUserLevel(int nLevel)
{ 
	nUserLevel = nLevel;
}

void SPartyMemberData::SetMaster(bool bRhsMaster)
{
	bMaster = bRhsMaster;
}

void SPartyMemberData::SetJobClass(int nRhsClass)
{ 
	nClass = nRhsClass;
}

void SPartyMemberData::SetWorldLevel(int nLevel)
{
	nWorldLevel = nLevel;
}

bool SPartyMemberData::IsEmpty() const
{
	return (name.empty());
}

#ifdef PRE_ADD_NEWCOMEBACK
void SPartyMemberData::SetComeback(bool bShow)
{
	bComeback = bShow;
}
#endif // PRE_ADD_NEWCOMEBACK

//////////////////////////////////////////////////////////////////////////
SPartyMemberNormalUIData::SPartyMemberNormalUIData()
: pUserLevel(NULL)
, pMaster(NULL)
, pClass(NULL)
, pName(NULL)
, pBase(NULL)
, pState(NULL)
, pFreeCoinIcon(NULL)
, pFreeCoinCount(NULL)
, pCashCoinIcon(NULL)
, pCashCoinCount(NULL)
, pStaticInvitation(NULL)
, pTextInviting(NULL)
#ifdef PRE_ADD_NEWCOMEBACK
, pComebackUser(NULL)
#endif // PRE_ADD_NEWCOMEBACK
{
}

void SPartyMemberNormalUIData::InitInfoList()
{
	SPartyMemberData::InitInfoList();

	pUserLevel->ClearText();
	pMaster->Show(false);
	pClass->ClearText();
	pName->ClearText();
	pBase->Show(true);
	pState->ClearText();
	pFreeCoinIcon->Show(false);
	pFreeCoinCount->Show(false);
	pCashCoinIcon->Show(false);
	pCashCoinCount->Show(false);
#ifdef PRE_ADD_NEWCOMEBACK
	pComebackUser->Show(false);
#endif // PRE_ADD_NEWCOMEBACK
	ShowInvitationCtrl(false);

	SUIControlProperty* pProp = pName->GetProperty();
	dwNameColor = (pProp) ? pProp->StaticProperty.dwFontColor : textcolor::PALEGREEN;
}

void SPartyMemberNormalUIData::SetUserLevel( int nLevel )
{ 
	swprintf_s( m_wszTemp, 80, L"%s %d", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 64 ), nLevel );
	pUserLevel->SetText( m_wszTemp );
	SPartyMemberData::SetUserLevel(nLevel);
}

void SPartyMemberNormalUIData::SetState(bool bAbsence)
{
	pState->SetTextColor(bAbsence ? textcolor::RED : textcolor::WHITE);
	pState->SetText(bAbsence ? GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3593 ) : L""); // UISTRING : 자리비움
}

void SPartyMemberNormalUIData::SetJobClass( int nClass )
{ 
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TJOB );
	if( !pSox ) return;

	int nStringTableID = pSox->GetFieldFromLablePtr( nClass, "_JobName" )->GetInteger();
	swprintf_s( m_wszTemp, _countof(m_wszTemp), L"%s", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nStringTableID ) );

	pClass->SetText( m_wszTemp );
	SPartyMemberData::SetJobClass(nClass);
}

void SPartyMemberNormalUIData::SetCoin( int freeCoin, int cashCoin )
{
	pFreeCoinCount->SetIntToText( freeCoin );
	pCashCoinCount->SetIntToText( cashCoin );

	pFreeCoinIcon->Show(true);
	pFreeCoinCount->Show(true);
	pCashCoinIcon->Show(true);
	pCashCoinCount->Show(true);
}

void SPartyMemberNormalUIData::SetName(const WCHAR* pNameString, bool bRed)
{
	SPartyMemberData::SetName(pNameString);
	pName->SetText(pNameString);
	pName->SetTextColor(bRed ? textcolor::RED : dwNameColor);
}

void SPartyMemberNormalUIData::SetMaster(bool bMaster)
{
	SPartyMemberData::SetMaster(bMaster);
	pMaster->Show(bMaster);
}

void SPartyMemberNormalUIData::ShowInvitationCtrl(bool bShow)
{
	if (pStaticInvitation)
		pStaticInvitation->Show(bShow);
	if (pTextInviting)
		pTextInviting->Show(bShow);
}

#ifdef PRE_ADD_NEWCOMEBACK
void SPartyMemberNormalUIData::SetComeback(bool bShow)
{
	SPartyMemberData::SetComeback(bShow);
	pComebackUser->Show(bShow);
}
#endif // PRE_ADD_NEWCOMEBACK

void SPartyMemberNormalUIData::Enable(bool bEnable)
{
	pMaster->Enable(bEnable);
	pClass->Enable(bEnable);
	pName->Enable(bEnable);
	pBase->Enable(bEnable);
	pUserLevel->Enable(bEnable);
}

bool SPartyMemberNormalUIData::IsInsideItem(float fX, float fY)
{
	SUICoord uiCoords;
	pBase->GetUICoord(uiCoords);

	return uiCoords.IsInside(fX, fY);
}

//////////////////////////////////////////////////////////////////////////

SPartyMemberRaidUIData::SPartyMemberRaidUIData()
: pUserLevel(NULL)
, pMaster(NULL)
, pName(NULL)
, pBase(NULL)
, pFreeCoinIcon(NULL)
, pFreeCoinCount(NULL)
, pCashCoinIcon(NULL)
, pCashCoinCount(NULL)
, pStaticInvitation(NULL)
, pTextInviting(NULL)
, pJobIcon(NULL)
#ifdef PRE_ADD_NEWCOMEBACK
, pComebackUser(NULL)
#endif // PRE_ADD_NEWCOMEBACK
{
}

void SPartyMemberRaidUIData::InitInfoList()
{
	SPartyMemberData::InitInfoList();

	pUserLevel->ClearText();
	pMaster->Show(false);
	pName->ClearText();
	pBase->Show(true);
	pFreeCoinIcon->Show(false);
	pFreeCoinCount->Show(false);
	pCashCoinIcon->Show(false);
	pCashCoinCount->Show(false);
	ShowInvitationCtrl(false);
	pJobIcon->SetIconID(-1);
#ifdef PRE_ADD_NEWCOMEBACK
	pComebackUser->Show(false);	
#endif // PRE_ADD_NEWCOMEBACK

	SUIControlProperty* pProp = pName->GetProperty();
	dwNameColor = (pProp) ? pProp->StaticProperty.dwFontColor : textcolor::PALEGREEN;
}

void SPartyMemberRaidUIData::SetUserLevel( int nLevel )
{ 
	swprintf_s( m_wszTemp, 80, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 728 ), nLevel );
	pUserLevel->SetText( m_wszTemp );
	SPartyMemberData::SetUserLevel(nLevel);
}

void SPartyMemberRaidUIData::SetJobClass(int nClass)
{
	if (CommonUtil::IsValidCharacterJobId(nClass) == false)
		return;

	pJobIcon->SetIconID(nClass);

	SPartyMemberData::SetJobClass(nClass);
}

void SPartyMemberRaidUIData::SetCoin( int freeCoin, int cashCoin )
{
	pFreeCoinCount->SetIntToText( freeCoin );
	pCashCoinCount->SetIntToText( cashCoin );

	pFreeCoinIcon->Show(true);
	pFreeCoinCount->Show(true);
	pCashCoinIcon->Show(true);
	pCashCoinCount->Show(true);
}

void SPartyMemberRaidUIData::SetName(const WCHAR* pNameString, bool bRed)
{
	SPartyMemberData::SetName(pNameString);
	pName->SetText(pNameString);
	pName->SetTextColor(bRed ? textcolor::RED : dwNameColor);
}

void SPartyMemberRaidUIData::SetMaster(bool bMaster)
{
	SPartyMemberData::SetMaster(bMaster);
	pMaster->Show(bMaster);
}

void SPartyMemberRaidUIData::ShowInvitationCtrl(bool bShow)
{
	if (pStaticInvitation)
		pStaticInvitation->Show(bShow);
	if (pTextInviting)
		pTextInviting->Show(bShow);
}

#ifdef PRE_ADD_NEWCOMEBACK
void SPartyMemberRaidUIData::SetComeback(bool bShow)
{
	SPartyMemberData::SetComeback(bShow);
	pComebackUser->Show(bShow);
}
#endif // PRE_ADD_NEWCOMEBACK

void SPartyMemberRaidUIData::Enable(bool bEnable)
{
	pMaster->Enable(bEnable);
	pName->Enable(bEnable);
	pBase->Enable(bEnable);
	pUserLevel->Enable(bEnable);
}

bool SPartyMemberRaidUIData::IsInsideItem(float fX, float fY)
{
	SUICoord uiCoords;
	pBase->GetUICoord(uiCoords);

	return uiCoords.IsInside(fX, fY);
}

//////////////////////////////////////////////////////////////////////////

SRaidReGroupCtrlUnit::SRaidReGroupCtrlUnit()
:	pLevel(NULL),
	pName(NULL),
	pSelect(NULL),
	sessionId(0),
	pBase(NULL),
	pJobIcon(NULL)
{
}

void SRaidReGroupCtrlUnit::InitUnit()
{
	sessionId = 0;
	pLevel->ClearText();
	pName->ClearText();
	pSelect->Show(false);
	int i = 0;
	pJobIcon->SetIconID(-1);
}

void SRaidReGroupCtrlUnit::SetInfo(UINT sesId, const WCHAR* pszName, int level, int jobId)
{
	std::wstring temp;
	temp = FormatW(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 728 ), level);
	pLevel->SetText(temp.c_str());

	pName->SetText(pszName);

	if (CommonUtil::IsValidCharacterJobId(jobId))
	{
		pJobIcon->SetIconID(jobId);
	}

	sessionId = sesId;
}

bool SRaidReGroupCtrlUnit::IsEmpty() const
{
	return (sessionId == 0);
}

void SRaidReGroupInfo::InitInfo()
{
	std::vector<SRaidReGroupCtrlUnit>::iterator iter = partyCtrlList.begin();
	for(; iter != partyCtrlList.end(); ++iter)
	{
		SRaidReGroupCtrlUnit& unit = *iter;
		unit.InitUnit();
	}
}