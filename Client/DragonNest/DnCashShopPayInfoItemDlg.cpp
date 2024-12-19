#include "StdAfx.h"
#include "DnCashShopPayInfoItemDlg.h"
#include "DnCashShopTask.h"
#include "DnInterface.h"
#ifdef PRE_MOD_PETAL_WRITE
#include "DnInterfaceString.h"
#endif // PRE_MOD_PETAL_WRITE
#include "DnTableDB.h"
#include "DnItemTask.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
CDnCashShopPayInfoItemDlg::CDnCashShopPayInfoItemDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
			: CEtUIDialog(dialogType, pParentDialog, nID, pCallback, true)
{
	m_SN = 0;
	m_pNameStatic = NULL;
	m_pInfoStatic = NULL;
	m_pPriceStatic = NULL;
}

CDnCashShopPayInfoItemDlg::~CDnCashShopPayInfoItemDlg(void)
{
}

void CDnCashShopPayInfoItemDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName("CSPayment_Text.ui").c_str(), bShow);
}

void CDnCashShopPayInfoItemDlg::InitialUpdate()
{
	m_pNameStatic = GetControl<CEtUIStatic>("ID_STATIC_NAME");
	m_pInfoStatic = GetControl<CEtUIStatic>("ID_STATIC_INFO");
	m_pPriceStatic = GetControl<CEtUIStatic>("ID_STATIC_PRICE");
}

void CDnCashShopPayInfoItemDlg::Show(bool bShow)
{
	if( m_bShow == bShow )
		return;

	CEtUIDialog::Show(bShow);
}

void CDnCashShopPayInfoItemDlg::SetInfo(const SCashShopItemInfo& data, const SCashShopCartItemInfo& cartInfo)
{
	m_SN = cartInfo.presentSN;

	std::wstring forConvStr;
	m_pNameStatic->SetText(data.nameString.c_str());

	if (cartInfo.isPackage == false)
	{
		SCashShopItemInfo* pItemInfo = GetCashShopTask().GetItemInfo(cartInfo.selectedSN);
		if (pItemInfo)
		{
#ifdef PRE_MOD_PETAL_WRITE
			forConvStr = FormatW( L"%s", DN_INTERFACE::UTIL::GetAddCommaString( pItemInfo->price ).c_str() );
#else // PRE_MOD_PETAL_WRITE
			forConvStr = FormatW( L"%d", pItemInfo->price );
#endif // PRE_MOD_PETAL_WRITE
			m_pPriceStatic->SetText(forConvStr.c_str());
		}
		else
		{
			m_pPriceStatic->ClearText();
		}
	}
	else
	{
		m_pPriceStatic->ClearText();
	}

	int num = 0;
#ifdef PRE_ADD_VIP
	if (data.type == eCSType_Costume || data.type == eCSType_Term)
#else
	if (data.type == eCSType_Costume)
#endif
	{
		CDnItem* pItem = NULL;
		TItemInfo itemInfo;
		if( CDnItem::MakeItemInfo( cartInfo.ability, 1, itemInfo ) )
		{
			pItem = GetItemTask().CreateItem( itemInfo );
		}

		if( pItem == NULL )
			return;

		int nPotentialIndex = pItem->GetTypeParam( 0 );
		SAFE_DELETE( pItem );

		DNTableFileFormat*  pPotenSox = GetDNTable( CDnTableDB::TPOTENTIAL );
		if( pPotenSox == NULL )
		{
			_ASSERT(0);
			return;
		}
		DNTableFileFormat*  pSkillTable = GetTableDB().GetTable( CDnTableDB::TSKILL );
		if( pSkillTable == NULL )
		{
			_ASSERT(0);
			return;
		}

		std::wstring strAbility;
		if( nPotentialIndex > 0 && cartInfo.nOptionIndex > 0 )
		{
			std::vector<int> nVecItemID;
			pPotenSox->GetItemIDListFromField( "_PotentialID", nPotentialIndex, nVecItemID );

			int itemId = nVecItemID[cartInfo.nOptionIndex - 1];
			int nSkillID = pPotenSox->GetFieldFromLablePtr( itemId, "_SkillID" )->GetInteger();
			int nSkillLevel = pPotenSox->GetFieldFromLablePtr( itemId, "_SkillLevel" )->GetInteger();

			if( nSkillLevel > 0 )
			{
				int iStringID = 0;
				iStringID = pSkillTable->GetFieldFromLablePtr( nSkillID, "_NameID" )->GetInteger();
				WCHAR wszSkillInfo[64] = {0,};
				swprintf_s( wszSkillInfo, _countof( wszSkillInfo ), L"%s +%d ", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, iStringID ), nSkillLevel );
				strAbility = wszSkillInfo;
			}
		}
		
		strAbility += GetCashShopTask().GetAbilityString( cartInfo.ability );
		if( strAbility.length() > 0 )
		{
			m_pInfoStatic->SetTextWithEllipsis( strAbility.c_str(), L"..." );
			std::wstring strRenderText = m_pInfoStatic->GetRenderText();
			if( wcscmp( strAbility.c_str(), strRenderText.c_str() ) != 0 )
				m_pInfoStatic->SetTooltipText( strAbility.c_str() );
			else
				m_pInfoStatic->SetTooltipText( L"" );
		}
		else
			m_pInfoStatic->SetTooltipText( L"" );
	}
	else if (data.type == eCSType_Function)
	{
		std::wstring str;
		str = FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4625), data.count);	// UISTRING : %d °³
		if (num >= 0)
			m_pInfoStatic->SetText(str.c_str());
	}
}
