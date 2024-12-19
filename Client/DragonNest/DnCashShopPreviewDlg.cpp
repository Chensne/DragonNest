#include "StdAfx.h"
#include "DnCashShopPreviewDlg.h"
#include "DnCharStatusDlg.h"
#include "DnTableDB.h"
#include "DnActorClassDefine.h"
#include "DnCashShopTask.h"
#include "DnCommonUtil.h"
#include "DnInterface.h"
#include "DnVehicleActor.h"
#include "DnPetActor.h"
#include "DnCashShopDlg.h"


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
CDnCashShopPreviewDlg::CDnCashShopPreviewDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
: CDnCustomDlg(dialogType, pParentDialog, nID, pCallback, true)
, m_pReduceBtn( NULL )
, m_pRotateLeftBtn( NULL )
, m_pRotateRightBtn( NULL )
, m_pRotateFrontBtn( NULL )
, m_pRotateBackBtn( NULL )
, m_pEnlargeBtn( NULL )
, m_pBuyAllBtn( NULL )
, m_pGiftAllBtn( NULL )
, m_pRollbackBtn( NULL )
, m_pSaveCoordi( NULL )
, m_pDrawFrameBtn( NULL )
, m_pNormalActionBtn( NULL )
, m_pBattleActionBtn( NULL )
, m_bHoldRender( false )
, m_fMouseX( 0.0f )
, m_fMouseY( 0.0f )
{
}

CDnCashShopPreviewDlg::~CDnCashShopPreviewDlg(void)
{
}

void CDnCashShopPreviewDlg::Initialize(bool bShow)
{
#ifdef PRE_ADD_CASHSHOP_RENEWAL
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName("CS_Preview.ui").c_str(), bShow);
#else
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName("CSPreview.ui").c_str(), bShow);
#endif // PRE_ADD_CASHSHOP_RENEWAL
}

void CDnCashShopPreviewDlg::InitialUpdate()
{
	m_pRotateLeftBtn	= GetControl<CEtUIButton>("ID_ROTATE_LEFT");
	m_pRotateRightBtn	= GetControl<CEtUIButton>("ID_ROTATE_RIGHT");

	m_pRotateFrontBtn	 = GetControl<CEtUIButton>("ID_BUTTON_FRONT");
	m_pRotateBackBtn = GetControl<CEtUIButton>("ID_BUTTON_BACK");

	m_pDrawFrameBtn = GetControl<CEtUIButton>("ID_BUTTON_PREVIEW");

	m_pBuyAllBtn	= GetControl<CEtUIButton>("ID_BUTTON_BUYALL");
	m_pGiftAllBtn	= GetControl<CEtUIButton>("ID_BUTTON_PRESENTALL");
	m_pRollbackBtn	= GetControl<CEtUIButton>("ID_BUTTON_ROLLBACK");

	m_pNormalActionBtn = GetControl<CEtUIButton>("ID_BUTTON_NORMAL");
	m_pNormalActionBtn->Show(false);
	m_pBattleActionBtn = GetControl<CEtUIButton>("ID_BUTTON_FIGHT");
	m_pBattleActionBtn->Show(true);

	if (m_pDrawFrameBtn == NULL)
	{
		_ASSERT(0);
		return;
	}

	SUICoord charViewCoord;
	if (m_pDrawFrameBtn->GetProperty() != NULL)
		charViewCoord = m_pDrawFrameBtn->GetProperty()->UICoord;

	const int nTextureSize = 1024;
	SCameraInfo CameraInfo;
	CameraInfo.Target = CT_RENDERTARGET_NO_GENERATE_BACKBUFFER;
	CameraInfo.fNear = 10.f;
	CameraInfo.Type = CT_ORTHOGONAL;
	m_RenderAvatar.Initialize( nTextureSize, nTextureSize, CameraInfo, 160, 120, FMT_A8R8G8B8, true, true );
	m_RenderAvatar.CalcUVCoord( charViewCoord.fWidth, charViewCoord.fHeight );
}

ITEMCLSID CDnCashShopPreviewDlg::GetDefaultPartsItemId(DNTableFileFormat* pSox, int classId, CDnParts::PartsTypeEnum partsType, int partsIndex) const
{
	if (pSox == NULL || CommonUtil::IsValidCharacterClassId(classId) == false)
		return ITEMCLSID_NONE;

	if (partsType < CDnParts::DefaultPartsType_Min || partsType > CDnParts::DefaultPartsType_Max)
		return ITEMCLSID_NONE;

	const int offset = CDnParts::DefaultPartsType_Max - CDnParts::DefaultPartsType_Min + 1;
	const char *szLabelList[offset] = { "_DefaultBody", "_DefaultLeg", "_DefaultHand", "_DefaultFoot" };
	const int index = partsType - CDnParts::DefaultPartsType_Min;
	return pSox->GetFieldFromLablePtr(classId, szLabelList[index])->GetInteger();
}

ITEMCLSID CDnCashShopPreviewDlg::GetBasicPartsItemId(DNTableFileFormat* pSox, int classId, CDnParts::PartsTypeEnum partsType, int partsIndex) const
{
	if (pSox == NULL || CommonUtil::IsValidCharacterClassId(classId) == false)
		return ITEMCLSID_NONE;

	if (partsType < 0 || partsType >= CDnParts::PartsTypeEnum_Amount)
		return ITEMCLSID_NONE;

	if (partsType >= CDnParts::DefaultPartsType_Min && partsType <= CDnParts::DefaultPartsType_Max)
	{
		return GetDefaultPartsItemId(pSox, classId, partsType, partsIndex);
	}
	else
	{
		char *szLabelList[CDnParts::PartsTypeEnum_Amount] = { "_Face", "_Hair", "_Helmet", "_Body", "_Leg", "_Hand", "_Foot", NULL, NULL, NULL, NULL };
		return pSox->GetFieldFromLablePtr(classId, FormatA("%s%d", szLabelList[partsType], partsIndex).c_str())->GetInteger();
	}
}

DWORD CDnCashShopPreviewDlg::GetBasicPartsColor(DNTableFileFormat*  pDefaultSox, MAPartsBody::PartsColorEnum colorType, int nColorIndex, int classId) const
{
	if (pDefaultSox == NULL || CommonUtil::IsValidCharacterClassId(classId) == false)
	{
		_ASSERT(0);
		return 0;
	}

	switch( colorType )
	{
	case MAPartsBody::HairColor:
		{
			D3DCOLOR d3dColor = D3DCOLOR_XRGB(
				pDefaultSox->GetFieldFromLablePtr( classId, FormatA("_HairColor%dR", nColorIndex+1).c_str() )->GetInteger(),
				pDefaultSox->GetFieldFromLablePtr( classId, FormatA("_HairColor%dG", nColorIndex+1).c_str() )->GetInteger(),
				pDefaultSox->GetFieldFromLablePtr( classId, FormatA("_HairColor%dB", nColorIndex+1).c_str() )->GetInteger() );

			DWORD dwColorR10G10B10 = 0;
			float fIntensity = 1.1f;
			CDnParts::ConvertD3DCOLORToR10G10B10( &dwColorR10G10B10, d3dColor, fIntensity );
			return dwColorR10G10B10;
		}

	case MAPartsBody::SkinColor:
		{
			int nA = (int)(pDefaultSox->GetFieldFromLablePtr( classId, FormatA("_SkinColor%dA", nColorIndex+1).c_str() )->GetFloat() * 255);
			int nR = (int)(pDefaultSox->GetFieldFromLablePtr( classId, FormatA("_SkinColor%dR", nColorIndex+1).c_str() )->GetFloat() * 255);
			int nG = (int)(pDefaultSox->GetFieldFromLablePtr( classId, FormatA("_SkinColor%dG", nColorIndex+1).c_str() )->GetFloat() * 255);
			int nB = (int)(pDefaultSox->GetFieldFromLablePtr( classId, FormatA("_SkinColor%dB", nColorIndex+1).c_str() )->GetFloat() * 255);

			ASSERT( nA >= 0 && nR >= 0 && nG >= 0 && nB >= 0 );
			ASSERT( nA <= 255 && nR <= 255 && nG <= 255 && nB <= 255 );

			DWORD dwSkinColor = D3DCOLOR_ARGB( nA, nR, nG, nB );
			return dwSkinColor;
		}

	case MAPartsBody::EyeColor:
		{
			D3DCOLOR d3dColor = D3DCOLOR_XRGB(
				pDefaultSox->GetFieldFromLablePtr( classId, FormatA("_EyeColor%dR", nColorIndex+1).c_str() )->GetInteger(),
				pDefaultSox->GetFieldFromLablePtr( classId, FormatA("_EyeColor%dG", nColorIndex+1).c_str() )->GetInteger(),
				pDefaultSox->GetFieldFromLablePtr( classId, FormatA("_EyeColor%dB", nColorIndex+1).c_str() )->GetInteger() );

			float fIntensity = 1.0f;
			DWORD dwColorR10G10B10 = 0;
			CDnParts::ConvertD3DCOLORToR10G10B10( &dwColorR10G10B10, d3dColor, fIntensity );
			return dwColorR10G10B10;
		}

	default:
		{
			_ASSERT(0);
			return 0;
		}
	}

	return 0;
}

int CDnCashShopPreviewDlg::GetPartsIndex(int classId) const
{
	if (CommonUtil::IsValidCharacterClassId(classId) == false)
		return 0;

	return 1;
}

void CDnCashShopPreviewDlg::MakeAvatarToRender(int classId, bool bDefault)
{
	CDnPlayerActor* pLocalActor = (CDnActor::s_hLocalActor.GetPointer()->GetClassID() == classId) ? static_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer()) : NULL;

	int nDefaultParts[CDnParts::DefaultPartsTypeEnum_Amount];
	memset(nDefaultParts, 0, sizeof(nDefaultParts));

	m_RenderAvatar.ResetActor();

	int i = 0;
	//	NORMAL PARTS
	if (pLocalActor)
	{
		m_RenderAvatar.SetActor( CDnActor::s_hLocalActor, true, true );
	}
	else
	{
		m_RenderAvatar.SetActor(classId);
		
		if(!m_RenderAvatar.GetActor() || !m_RenderAvatar.GetActor()->IsPlayerActor())
			return;

		CDnPlayerActor *pPlayerActor = static_cast<CDnPlayerActor*>(m_RenderAvatar.GetActor().GetPointer());
		if (pPlayerActor == NULL) return;

		int i = 0;
		const DefaultPartsStruct& partsStruct = m_VecCreateDefaultPartsList[classId - 1];
		int nDefaultParts[ CDnParts::DefaultPartsTypeEnum_Amount ];
		for (i = 0; i < CDnParts::DefaultPartsTypeEnum_Amount; i++)
			nDefaultParts[ i ] = partsStruct.nSelectPartsIndex[CDnParts::Body + i];
		pPlayerActor->SetDefaultPartsInfo(nDefaultParts);

		for (i = 0; i < CDnParts::PartsTypeEnum_Amount; i++)
			m_RenderAvatar.AttachItem(partsStruct.nSelectPartsIndex[i]);

		//	WEAPON
		for (i = 0; i < 2; ++i)
			m_RenderAvatar.AttachItem(partsStruct.nWeapon[i]);

		pPlayerActor->SetPartsColor(MAPartsBody::HairColor, partsStruct.dwColor[MAPartsBody::HairColor]);
		pPlayerActor->SetPartsColor(MAPartsBody::SkinColor, partsStruct.dwColor[MAPartsBody::SkinColor]);
		pPlayerActor->SetPartsColor(MAPartsBody::EyeColor, partsStruct.dwColor[MAPartsBody::EyeColor]);

#ifdef PRE_ADD_CASH_COSTUME_AURA
		CDnPlayerActor *pAvatarActor = static_cast<CDnPlayerActor*>(m_RenderAvatar.GetActor().GetPointer());
		if(pAvatarActor)
			pAvatarActor->ComputeRTTModeCostumeAura();
#endif
	}

	// 액터가 바뀔때 애니 역시 초기화되므로 버튼도 갱신해준다.
	//m_pNormalActionBtn->Show(false);
	//m_pBattleActionBtn->Show(true);

	if (m_pNormalActionBtn->IsShow())
		ToggleAction(true);

	if (bDefault == false)
	{
		//	attach parts from cash task
		const CART_ITEM_LIST* pList = GetCashShopTask().GetPreviewCartList(classId);
		if (pList == NULL || pList->size() <= 0)
			return;

		CART_ITEM_LIST::const_iterator iter = pList->begin();
		for (; iter != pList->end(); ++iter)
		{
			const SCashShopCartItemInfo& info = *iter;
			const SCashShopItemInfo* pItemInfo = GetCashShopTask().GetItemInfo(info.presentSN);
			if (pItemInfo != NULL && pItemInfo->presentItemId != ITEMCLSID_NONE)
				AttachParts(pItemInfo->presentItemId);
		}
	}
}

void CDnCashShopPreviewDlg::MakeDefaultParts()
{
	m_VecCreateDefaultPartsList.clear();
	m_VecOnepieceList.clear();

	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TDEFAULTCREATE );
	int nActorCount = pSox->GetItemCount();
	char *szLabelList[CDnParts::PartsTypeEnum_Amount] = { "_Face", "_Hair", "_Helmet", "_Body", "_Leg", "_Hand", "_Foot", NULL, NULL, NULL, NULL };
	char szLabel[64];
	int i = 1;	// Start : Warrior

	for (; i <= nActorCount; ++i)
	{
		DefaultPartsStruct Struct;

		int k = 0;
		if (CDnActor::s_hLocalActor.GetPointer()->GetClassID() == i)
		{
			CDnPlayerActor* pActor = static_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());

			MAPartsBody *pPartsBody = dynamic_cast<MAPartsBody*>(pActor);
			if (pPartsBody == NULL)
				continue;

			// PARTS
			memset(Struct.nSelectPartsIndex, 0, sizeof(Struct.nSelectPartsIndex));
			for (k = 0; k < CDnParts::PartsTypeEnum_Amount; ++k)
			{
				ITEMCLSID itemId = ITEMCLSID_NONE;
				DnPartsHandle hParts = pPartsBody->GetParts( (CDnParts::PartsTypeEnum)k);
				if (!hParts)
					continue;
				CDnItem *pItem = static_cast<CDnItem*>(hParts.GetPointer());
				Struct.nSelectPartsIndex[k] = pItem->GetClassID();
			}

			//	CASH PARTS
			bool bOnepiece = false;
			memset(Struct.nSelectCashPartsIndex, 0, sizeof(Struct.nSelectCashPartsIndex));
			memset(Struct.nSelectCashPartsLookIndex, 0, sizeof(Struct.nSelectCashPartsLookIndex));
			for (k = CDnParts::CashHelmet; k < CDnParts::CashPartsTypeEnum_Amount; k++)
			{
				DnPartsHandle hParts = pPartsBody->GetCashParts((CDnParts::PartsTypeEnum)k);
				if (!hParts)
					continue;
				if( hParts->IsExistSubParts() ) bOnepiece = true;
				CDnItem *pItem = (CDnItem *)hParts.GetPointer();
				Struct.nSelectCashPartsIndex[k] = pItem->GetClassID();
				Struct.nSelectCashPartsLookIndex[k] = pItem->GetLookItemID();
			}

			//	WEAPON
			memset(Struct.nWeapon, 0, sizeof(Struct.nWeapon));
			for (k = 0; k < 2; k++)
			{
				if (pActor->GetWeapon(k)) 
					Struct.nWeapon[k] = pActor->GetWeapon(k)->GetClassID();
			}

			//	CASH WEAPON
			memset(Struct.nCashWeapon, 0, sizeof(Struct.nCashWeapon));
			memset(Struct.nCashWeaponLookIndex, 0, sizeof(Struct.nCashWeaponLookIndex));
			for (k = 0; k < 2; k++)
			{
				if (pActor->GetCashWeapon(k)) 
				{
					Struct.nCashWeapon[k] = pActor->GetCashWeapon(k)->GetClassID();
					Struct.nCashWeaponLookIndex[k] = pActor->GetCashWeapon(k)->GetLookItemID();
				}
			}

			// 헤어색,    (피부색, 눈깔색 추가예정)
			Struct.dwColor[MAPartsBody::HairColor] = pPartsBody->GetPartsColor( MAPartsBody::HairColor );
			Struct.dwColor[MAPartsBody::SkinColor] = pPartsBody->GetPartsColor( MAPartsBody::SkinColor );
			Struct.dwColor[MAPartsBody::EyeColor] = pPartsBody->GetPartsColor( MAPartsBody::EyeColor );

			m_VecCreateDefaultPartsList.push_back( Struct );
			m_VecOnepieceList.push_back( bOnepiece );
			continue;
		}

		for (k = 0; k < CDnParts::PartsTypeEnum_Amount; ++k)
		{
			if( szLabelList[k] == NULL )
				continue;

			ITEMCLSID partsItemId = GetBasicPartsItemId(pSox, i, (CDnParts::PartsTypeEnum)k, GetPartsIndex(i));
			Struct.nSelectPartsIndex[k] = partsItemId;
		}

		Struct.dwColor[MAPartsBody::HairColor] = GetBasicPartsColor(pSox, MAPartsBody::HairColor, GetPartsIndex(i), i);
		Struct.dwColor[MAPartsBody::SkinColor] = GetBasicPartsColor(pSox, MAPartsBody::SkinColor, GetPartsIndex(i), i);
		Struct.dwColor[MAPartsBody::EyeColor] = GetBasicPartsColor(pSox, MAPartsBody::EyeColor, GetPartsIndex(i), i);

		int j = 0;
		for (; j < 2; j++)
		{
			sprintf_s( szLabel, "_Weapon%d", j + 1 );
			Struct.nWeapon[j] = pSox->GetFieldFromLablePtr( i, szLabel )->GetInteger();
		}

		memset(Struct.nSelectCashPartsIndex, 0, sizeof(Struct.nSelectCashPartsIndex));
		memset(Struct.nCashWeapon, 0, sizeof(Struct.nCashWeapon));
		memset(Struct.nSelectCashPartsLookIndex, 0, sizeof(Struct.nSelectCashPartsLookIndex));
		memset(Struct.nCashWeaponLookIndex, 0, sizeof(Struct.nCashWeaponLookIndex));

		m_VecCreateDefaultPartsList.push_back( Struct );
		m_VecOnepieceList.push_back( false );
	}
}

void CDnCashShopPreviewDlg::ChangeVehicleParts(int nClassID,int nPartClassID, bool bDefault)
{
	if(m_RenderAvatar.GetActor() && m_RenderAvatar.GetActor()->IsVehicleActor())
	{
		CDnVehicleActor *pVehicle = dynamic_cast<CDnVehicleActor*>(m_RenderAvatar.GetActor().GetPointer());
		if(!pVehicle)
			return;

		if(nClassID != 0)
			pVehicle->SetItemID(nClassID);

		if(!bDefault && nPartClassID != 0 )
		{
			int VehicleClassType = -1;
			int VehiclePartsClassType = -1;
			int nItemType = 0;

			DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TITEM );
			nItemType = pSox->GetFieldFromLablePtr( nPartClassID, "_Type" )->GetInteger();

			if(nItemType != ITEMTYPE_VEHICLEHAIRCOLOR)
			{
				DNTableFileFormat* pVehicleTable = GetDNTable( CDnTableDB::TVEHICLE );
				if(!pVehicleTable || !pVehicleTable->IsExistItem(pVehicle->GetItemID()))
					return;
			
				DNTableFileFormat* pVehiclePartsTable = GetDNTable( CDnTableDB::TVEHICLEPARTS );
				if( !pVehiclePartsTable || !pVehiclePartsTable->IsExistItem(nPartClassID)) 
					return;
		
				VehicleClassType = pVehicleTable->GetFieldFromLablePtr( pVehicle->GetItemID() , "_VehicleClassID" )->GetInteger();
				VehiclePartsClassType = pVehiclePartsTable->GetFieldFromLablePtr( nPartClassID , "_VehicleClassID" )->GetInteger();
		
				if( (VehicleClassType != VehiclePartsClassType) ||  VehicleClassType == -1)
				{
					CDnInterface::GetInstance().ShowCaptionDialog( CDnInterface::typeCaption3, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9212 ), textcolor::YELLOW, 4.0f );
					return;
				}
			}

			pVehicle->EquipItem(nPartClassID);
		}
		else if(bDefault)
		{
			pVehicle->SetDefaultParts();
		}
	}
}

void CDnCashShopPreviewDlg::SetVehicleDefaultParts()
{
	if(m_RenderAvatar.GetActor() && m_RenderAvatar.GetActor()->IsVehicleActor())
	{
		CDnVehicleActor *pVehicle = dynamic_cast<CDnVehicleActor*>(m_RenderAvatar.GetActor().GetPointer());
		if(!pVehicle)
			return;
		
		pVehicle->SetDefaultParts();
	}
}


eRetPreviewChange CDnCashShopPreviewDlg::ChangePreviewAvatar(int classId,bool bForce)
{
	if (CommonUtil::IsValidCharacterClassId(classId) == false && !bForce)
		return eRETPREVIEW_ERROR;

	if (classId != GetCashShopTask().GetPreviewCartClassId() || bForce)
	{
		MakeAvatarToRender(classId, false);
		GetCashShopTask().SetPreviewCartClassId(classId);
		return eRETPREVIEW_CHANGE;
	}

	return eRETPREVIEW_NOCHANGE;
}

bool CDnCashShopPreviewDlg::AttachParts(CASHITEM_SN sn, ITEMCLSID itemId, bool bCheckPackageCashRing2)
{
	if (itemId == ITEMCLSID_NONE)
		return false;

	int classId = GetCashShopTask().GetAttachableClass(GetCashShopTask().GetPreviewCartClassId(), itemId);
	if (CommonUtil::IsValidCharacterClassId(classId) == false)
	{
		_ASSERT(0);
		return false;
	}

	ChangePreviewAvatar(classId);

	bool bCheckCashRing2 = false;
	if (GetCashShopTask().PutItemIntoPreviewCart(classId, sn, itemId, (bCheckPackageCashRing2?&bCheckCashRing2:NULL)) == false)
		return false;

	m_IdMatchingList[classId].insert(std::make_pair(sn, itemId));

	AttachParts(itemId, bCheckCashRing2);

	return true;
}

void CDnCashShopPreviewDlg::AttachParts(ITEMCLSID itemId, bool bCashRing2)
{
	m_RenderAvatar.AttachItem(itemId, 0, (bCashRing2?1:0) );
}

bool CDnCashShopPreviewDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsShow() )
	{
		return false;
	}

	switch( uMsg )
	{
	case WM_MOUSEWHEEL:
		{
			if( IsMouseInDlg() )
			{
				UINT uLines;
				SystemParametersInfo( SPI_GETWHEELSCROLLLINES, 0, &uLines, 0 );
				int nScrollAmount = int( ( short )HIWORD( wParam ) ) / WHEEL_DELTA * uLines;
				if( nScrollAmount < 0 ) {
					m_RenderAvatar.ZoomOut();
				}
				else if( nScrollAmount > 0 ) {
					m_RenderAvatar.ZoomIn();
				}
			}
		}
		break;
	}

	return CDnCustomDlg::MsgProc( hWnd, uMsg, wParam, lParam );
}

void CDnCashShopPreviewDlg::DetachPartsAll(int classId)
{
	if (CommonUtil::IsValidCharacterClassId(classId) == false)
	{
		_ASSERT(0);
		return;
	}

	std::map<int, ITEMCLSID>::iterator iter = m_IdMatchingList[classId].begin();
	for (; iter != m_IdMatchingList[classId].end(); ++iter)
	{
		ITEMCLSID itemId = (*iter).second;
		DetachPartsWithItemId(classId, itemId);
	}

	m_IdMatchingList[classId].clear();

	if( m_RenderAvatar.GetActor() )
	{
		CDnPlayerActor *pActor = (CDnPlayerActor *)m_RenderAvatar.GetActor().GetPointer();
		DnPartsHandle hParts = pActor->GetCashParts( CDnParts::CashRing2 );
		int nCurID = 0;
		if( hParts ) nCurID = hParts->GetClassID();
		const DefaultPartsStruct& partsInfo = m_VecCreateDefaultPartsList[classId - 1];
		if( nCurID != partsInfo.nSelectCashPartsIndex[CASHEQUIP_RING2] )
		{
			pActor->DetachCashParts( CDnParts::CashRing2 );
			if( partsInfo.nSelectCashPartsIndex[CASHEQUIP_RING2] != 0 )
				m_RenderAvatar.AttachItem(partsInfo.nSelectCashPartsIndex[CASHEQUIP_RING2], 0, 1, partsInfo.nSelectCashPartsLookIndex[CASHEQUIP_RING2]);
		}
	}
}

void CDnCashShopPreviewDlg::DetachPartsWithItemId(int classId, ITEMCLSID itemId)
{
	if (GetCashShopTask().GetPreviewCartClassId() == classId)
	{
		const DefaultPartsStruct& partsInfo = m_VecCreateDefaultPartsList[classId - 1];
		const bool& bOnepiece = m_VecOnepieceList[classId - 1];

		DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TITEM );
		if( !pSox ) return;
		if( !pSox->IsExistItem( itemId ) ) return;

		CDnPlayerActor *pPlayerActor = static_cast<CDnPlayerActor*>(m_RenderAvatar.GetActor().GetPointer());
		if( !pPlayerActor ) return;

		eItemTypeEnum type = (eItemTypeEnum)pSox->GetFieldFromLablePtr( itemId, "_Type" )->GetInteger();
		if (type == ITEMTYPE_HAIRDYE)
		{
			pPlayerActor->SetPartsColor(MAPartsBody::HairColor, partsInfo.dwColor[MAPartsBody::HairColor]);
		}
		else if(type == ITEMTYPE_EYEDYE)
		{
			pPlayerActor->SetPartsColor(MAPartsBody::EyeColor, partsInfo.dwColor[MAPartsBody::EyeColor]);
		}
		else if(type == ITEMTYPE_SKINDYE)
		{
			pPlayerActor->SetPartsColor(MAPartsBody::SkinColor, partsInfo.dwColor[MAPartsBody::SkinColor]);
		}
		else if (type == ITEMTYPE_PARTS)
		{
			int nPartsType = CDnParts::GetPartsType(itemId);
			if (bOnepiece)
			{
				if (nPartsType >= CDnParts::CashBody && nPartsType <= CDnParts::CashFoot)
					nPartsType = CDnParts::CashBody;
			}
			if (nPartsType >= 0 && nPartsType < EQUIPMAX)
			{
				if (partsInfo.nSelectCashPartsIndex[nPartsType] != 0)
				{
					m_RenderAvatar.AttachItem(partsInfo.nSelectCashPartsIndex[nPartsType], 0, 0, partsInfo.nSelectCashPartsLookIndex[nPartsType]);
				}
				else
				{
					if (nPartsType + 2 < EQUIPMAX)
					{
						m_RenderAvatar.DetachItem(itemId);
						m_RenderAvatar.AttachItem(partsInfo.nSelectPartsIndex[nPartsType+2], 0, 0, ITEMCLSID_NONE);
					}
					else
					{
						_ASSERT(0);
		#ifdef _WORK
						GetInterface().AddChatMessage(CHATTYPE_SYSTEM, L"", L"PartsType 범위 문제로 캐시샵 프리뷰창 장착에 문제가 있습니다.", true);
		#endif
					}
				}
			}
		}
		else if (type == ITEMTYPE_FACIAL || type == ITEMTYPE_HAIRDRESS)
		{
			DWORD dwItemId = (DWORD)pSox->GetFieldFromLablePtr( itemId, "_TypeParam1" )->GetInteger();
			DNTableFileFormat* pPartsSox = GetDNTable( CDnTableDB::TPARTS );
			if (pPartsSox->IsExistItem( dwItemId ) == false)
			{
				ASSERT( false );
				return;
			}
			ASSERT( CDnItem::GetItemType( dwItemId ) == type );
			int nPartsType = pPartsSox->GetFieldFromLablePtr( dwItemId, "_Parts" )->GetInteger();
			if (nPartsType >= 0 && nPartsType < EQUIPMAX)
				m_RenderAvatar.AttachItem(partsInfo.nSelectPartsIndex[nPartsType]);
		}
		else if (type == ITEMTYPE_WEAPON)
		{
			int nEquipIndex = CDnCharStatusDlg::EQUIPTYPE_2_EQUIPINDEX( CDnWeapon::GetEquipType(itemId) ) - EQUIP_WEAPON1;
			m_RenderAvatar.DetachItem(itemId);
			if (partsInfo.nCashWeapon[nEquipIndex] != 0)
			{
				m_RenderAvatar.AttachItem(partsInfo.nCashWeapon[nEquipIndex], 0, 0, partsInfo.nCashWeaponLookIndex[nEquipIndex]);
			}
			else
			{
				m_RenderAvatar.AttachItem(partsInfo.nWeapon[nEquipIndex]);
			}
		}
	}
}

void CDnCashShopPreviewDlg::DetachPartsWithSN(int classId, CASHITEM_SN sn)
{
	if (CommonUtil::IsValidCharacterClassId(classId) == false)
	{
		_ASSERT(0);
		return;
	}

	std::map<int, ITEMCLSID>::iterator iter = m_IdMatchingList[classId].find(sn);
	if (iter != m_IdMatchingList[classId].end())
	{
		ITEMCLSID itemId = (*iter).second;
		DetachPartsWithItemId(classId, itemId);
	}
}

void CDnCashShopPreviewDlg::InitRenderAvatar()
{
	MakeDefaultParts();

	CDnLocalPlayerActor *pDnPlayerActor = static_cast<CDnLocalPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
	if (pDnPlayerActor == NULL) 
		return;

	MakeAvatarToRender( pDnPlayerActor->GetClassID(), true );
}

void CDnCashShopPreviewDlg::Show(bool bShow)
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
		m_pNormalActionBtn->Show( false );
		m_pBattleActionBtn->Show( true );
		if( m_RenderAvatar.GetActor() )
			GetCashShopTask().SetPreviewCartClassId( m_RenderAvatar.GetActor()->GetClassID() );
	}

	CDnCustomDlg::Show(bShow);
}

void CDnCashShopPreviewDlg::Render( float fElapsedTime )
{
	CDnCustomDlg::Render( fElapsedTime );

	if( IsShow() ) {
		if (m_bHoldRender)
			return;

		SUICoord charViewCoord;
		if (m_pDrawFrameBtn->GetProperty() != NULL)
			charViewCoord = m_pDrawFrameBtn->GetProperty()->UICoord;

		DrawSprite( m_RenderAvatar.GetRTT(), m_RenderAvatar.GetUVCoord(), 0xFFFFFFFF, charViewCoord );
	}
}

void CDnCashShopPreviewDlg::Process( float fElapsedTime )
{
	if( !CDnCashShopTask::IsActive() || GetCashShopTask().IsOpenCashShop() == false ) return;

	m_RenderAvatar.Process( fElapsedTime );

	static float fRotScale = 200.f;
	if( m_pRotateLeftBtn->IsPressed() )
	{
		m_RenderAvatar.AddRotateYaw( fElapsedTime * fRotScale );
	}
	else if( m_pRotateRightBtn->IsPressed() )
	{
		m_RenderAvatar.AddRotateYaw( -fElapsedTime * fRotScale );
	}

	if( m_pDrawFrameBtn->IsPressed()  )
	{
		float fX = m_pDrawFrameBtn->GetMouseCoord().fX;
		float fY = m_pDrawFrameBtn->GetMouseCoord().fY;
		if( m_fMouseX != 0.f && m_fMouseY != 0.f ) {
			static float fMoveScale = 500.f;
			float fAddAngle = fMoveScale * sqrtf( (m_fMouseX-fX)*(m_fMouseX-fX)+(m_fMouseY-fY)*(m_fMouseY-fY) ) *  (((m_fMouseX-fX)>0.f)? 1.f : -1.f) ;
			m_RenderAvatar.AddRotateYaw( fAddAngle );
		}
		m_fMouseX = fX;
		m_fMouseY = fY;

		focus::SetFocus( m_pDrawFrameBtn );
	}
	else if( m_pDrawFrameBtn->IsRightPressed()  )
	{
		float fX = m_pDrawFrameBtn->GetMouseCoord().fX;
		float fY = m_pDrawFrameBtn->GetMouseCoord().fY;

		if( m_fMouseX != 0.f && m_fMouseY != 0.f ) {
			float fDistX = m_fMouseX - fX;
			float fDistY = m_fMouseY - fY;
			m_RenderAvatar.Panning( fDistX * GetScreenWidth(), fDistY * GetScreenHeight() );
		}

		m_fMouseX = fX;
		m_fMouseY = fY;

		focus::SetFocus( m_pDrawFrameBtn );
	}
	else {
		m_fMouseX = 0.f;
		m_fMouseY = 0.f;
	}


	if( m_RenderAvatar.IsFrontView() ) {
		m_pRotateFrontBtn->Show( false );
		m_pRotateBackBtn->Show( true );
	}
	else {
		m_pRotateFrontBtn->Show( true );
		m_pRotateBackBtn->Show( false );
	}

	const CART_ITEM_LIST* pList = GetCashShopTask().GetPreviewCartList(GetCashShopTask().GetPreviewCartClassId());
	if (CDnActor::s_hLocalActor)
	{
		int previewClassId = GetCashShopTask().GetPreviewCartClassId();
		bool bEnable = false;
		if( pList != NULL && (int)pList->size() > 0 && previewClassId == CDnActor::s_hLocalActor->GetClassID() && GetCashShopTask().GetPreviewType(previewClassId) == CDnCashShopTask::ePreview_Normal )
			bEnable = true;
#ifdef PRE_ADD_SALE_COUPON
		if( GetInterface().GetCashShopDlg()->GetCurrentTabID() == 7 )
			bEnable = false;
#endif // PRE_ADD_SALE_COUPON
		m_pBuyAllBtn->Enable(bEnable);
	}
	else
	{
		_ASSERT(0);
		m_pBuyAllBtn->Enable(false);
	}

	bool bEnable = false;
	int previewClassId = GetCashShopTask().GetPreviewCartClassId();
	if( pList != NULL && (int)pList->size() > 0 && GetCashShopTask().GetPreviewType(previewClassId) == CDnCashShopTask::ePreview_Normal )
		bEnable = true;
#ifdef PRE_ADD_SALE_COUPON
	if( GetInterface().GetCashShopDlg()->GetCurrentTabID() == 7 )
		bEnable = false;
#endif // PRE_ADD_SALE_COUPON
#if defined PRE_ADD_PETALSHOP || defined PRE_CHN_OBTCASHSHOP
	m_pGiftAllBtn->Enable(false);
#else
	m_pGiftAllBtn->Enable(bEnable);
#endif // defined PRE_ADD_PETALSHOP || defined PRE_CHN_OBTCASHSHOP

	CDnCustomDlg::Process(fElapsedTime);
}

void CDnCashShopPreviewDlg::ToggleAction(bool bBattleMode)
{
	if( m_RenderAvatar.GetActor() && m_RenderAvatar.GetActor()->IsPlayerActor())
	{
		CDnPlayerActor *pActor = static_cast<CDnPlayerActor *>(m_RenderAvatar.GetActor().GetPointer());
		if (bBattleMode)
		{
			if( !pActor->IsBattleMode() && pActor->IsMovable() && ( pActor->IsStay() || pActor->IsMove() ) )
			{
				if( pActor->IsCanBattleMode() )
				{
					if( pActor->IsMove() ) pActor->CmdStop( "Stand" );
					pActor->CmdToggleBattle( true );

					m_pNormalActionBtn->Show( true );
					m_pBattleActionBtn->Show( false );
				}
			}
		}
		else
		{
			if( pActor->IsBattleMode() && pActor->IsMovable() && ( pActor->IsStay() || pActor->IsMove() ) )
			{
				if( pActor->IsMove() ) pActor->CmdStop( "Stand" );
				pActor->CmdToggleBattle( false );

				m_pNormalActionBtn->Show( false );
				m_pBattleActionBtn->Show( true );
			}
		}
	}
	else if(m_RenderAvatar.GetActor() && m_RenderAvatar.GetActor()->IsVehicleActor())
	{
		CDnVehicleActor *pVehicleActor = static_cast<CDnVehicleActor *>(m_RenderAvatar.GetActor().GetPointer());
		pVehicleActor->CmdStop("Jump_Stand");
	}		

}

void CDnCashShopPreviewDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if (IsCmdControl("ID_BUTTON_ROLLBACK"))
		{

			if(m_RenderAvatar.GetActor() && m_RenderAvatar.GetActor()->IsVehicleActor())
			{
				SetVehicleDefaultParts();
				return;
			}

			ClearParts(GetCashShopTask().GetPreviewCartClassId());
			GetCashShopTask().ClearPreviewCart(GetCashShopTask().GetPreviewCartClassId());
			return;
		}

		if (IsCmdControl("ID_BUTTON_BUYALL"))
		{
			GetCashShopTask().BuyPreviewCart(GetCashShopTask().GetPreviewCartClassId());
			return;
		}

		if (IsCmdControl("ID_BUTTON_PRESENTALL"))
		{
			GetCashShopTask().GiftPreviewCart();
			return;
		}

		if( IsCmdControl("ID_BUTTON_PREVIEW"))	
		{
			focus::ReleaseControl();
		}

		if( IsCmdControl("ID_BUTTON_FRONT") )
		{
			m_RenderAvatar.SetFrontView();
		}
		if( IsCmdControl("ID_BUTTON_BACK") )
		{
			m_RenderAvatar.SetRearView();
		}
		if( IsCmdControl("ID_BUTTON_ZOOMIN") )
		{
			m_RenderAvatar.ZoomIn();
		}
		if( IsCmdControl("ID_BUTTON_ZOOMOUT") )
		{
			m_RenderAvatar.ZoomOut();
		}
		if( IsCmdControl("ID_BUTTON_FIGHT") )
		{
			ToggleAction(true);
		}
		else if( IsCmdControl("ID_BUTTON_NORMAL") )
		{
			ToggleAction(false);
		}
	}

	CDnCustomDlg::ProcessCommand(nCommand, bTriggeredByUser, pControl, uMsg);
}

void CDnCashShopPreviewDlg::ClearParts(int classId)
{
	if( m_RenderAvatar.GetActorID() != classId )
	{
		MakeAvatarToRender(classId, true);
	}
	else
	{
		DetachPartsAll(classId);
	}
}

CDnCashShopPetPreviewDlg::CDnCashShopPetPreviewDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCharPetPreviewDlg( dialogType, pParentDialog, nID, pCallback )
, m_pBuyAllBtn( NULL )
, m_pGiftAllBtn( NULL )
, m_pRollbackBtn( NULL )
{
}

CDnCashShopPetPreviewDlg::~CDnCashShopPetPreviewDlg()
{
	SAFE_RELEASE_SPTR( m_hPet );
}

void CDnCashShopPetPreviewDlg::Initialize( bool bShow )
{
#ifdef PRE_ADD_CASHSHOP_RENEWAL
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName("CS_Preview.ui").c_str(), bShow);
#else
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "CSPreview.ui" ).c_str(), bShow );
#endif // PRE_ADD_CASHSHOP_RENEWAL
	
}

void CDnCashShopPetPreviewDlg::InitialUpdate()
{
	m_pButtonRotateLeft = GetControl<CEtUIButton>( "ID_ROTATE_LEFT" );
	m_pButtonRotateRight = GetControl<CEtUIButton>( "ID_ROTATE_RIGHT" );
	m_pButtonRotateFront = GetControl<CEtUIButton>( "ID_BUTTON_FRONT" );
	m_pButtonRotateBack = GetControl<CEtUIButton>( "ID_BUTTON_BACK" );
	m_pButtonAvatarViewArea = GetControl<CEtUIButton>( "ID_BUTTON_PREVIEW" );

	m_pBuyAllBtn = GetControl<CEtUIButton>( "ID_BUTTON_BUYALL" );
	m_pGiftAllBtn = GetControl<CEtUIButton>( "ID_BUTTON_PRESENTALL" );
	m_pRollbackBtn = GetControl<CEtUIButton>( "ID_BUTTON_ROLLBACK" );

	CEtUIButton* pButton = GetControl<CEtUIButton>( "ID_BUTTON_NORMAL" );
	pButton->Enable( false );
	pButton = GetControl<CEtUIButton>( "ID_BUTTON_FIGHT" );
	pButton->Enable( false );

	SUICoord charViewCoord;
	if( m_pButtonAvatarViewArea->GetProperty() != NULL )
		charViewCoord = m_pButtonAvatarViewArea->GetProperty()->UICoord;

	const int nTextureSize = 512;
	SCameraInfo CameraInfo;
	CameraInfo.Target = CT_RENDERTARGET_NO_GENERATE_BACKBUFFER;
	CameraInfo.fNear = 20.f;
	CameraInfo.Type = CT_ORTHOGONAL;
	m_RenderAvatar.Initialize( nTextureSize, nTextureSize, CameraInfo, 192, 144, FMT_A8R8G8B8, true, true );
	m_RenderAvatar.CalcUVCoord( charViewCoord.fWidth, charViewCoord.fHeight );
	m_RenderAvatar.SetCameraYPos( 60.0f );
}

void CDnCashShopPetPreviewDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	CEtUIDialog::Show( bShow );

	if( bShow )
	{
		ResetBasePet();
	}
	else
	{
		GetCashShopTask().ClearPreviewCart( PET_CLASS_ID );
		if( m_hPet )
			SAFE_RELEASE_SPTR( m_hPet );
			
		m_RenderAvatar.ResetActor();
	}
}

void CDnCashShopPetPreviewDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_BUTTON_ROLLBACK" ) )
		{
			ResetBasePet();
			GetCashShopTask().ClearPetPreviewCart();

			if( GetInterface().GetCashShopDlg() && GetInterface().GetCashShopDlg()->GetCurrentTabID() == 0 )
				GetInterface().GetCashShopDlg()->SwapPreview( true );

			return;
		}

		if( IsCmdControl( "ID_BUTTON_BUYALL" ) )
		{
			if( GetInterface().GetCashShopDlg() )
				GetInterface().GetCashShopDlg()->OpenBuyPetPreviewCart();
			return;
		}

		if( IsCmdControl( "ID_BUTTON_PRESENTALL" ) )
		{
			if( GetInterface().GetCashShopDlg() )
				GetInterface().GetCashShopDlg()->GiftPreviewCart( true );
			return;
		}
	}

	CDnCharPetPreviewDlg::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnCashShopPetPreviewDlg::ResetBasePet()
{
	SAFE_RELEASE_SPTR( m_hPet );

	CDnPlayerActor* pDnPlayerActor = static_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
	if( pDnPlayerActor == NULL ) 
		return;

	if( m_RenderAvatar.GetActorID() )
		m_RenderAvatar.ResetActor();

	if( pDnPlayerActor && pDnPlayerActor->IsSummonPet() )
		SetPetAvatar( pDnPlayerActor->GetPetInfo().Vehicle[0].nItemID );

	if( m_hPet == NULL )
		return;

	dynamic_cast<CDnPetActor*>( m_hPet.GetPointer() )->SetPetInfo( pDnPlayerActor->GetPetInfo() );

	for( int i=0; i<Pet::Slot::Max; i++ )
	{
		if( ( i == Pet::Slot::Accessory1 || i == Pet::Slot::Accessory2 ) && 
			pDnPlayerActor->GetPetInfo().Vehicle[i].nItemID > 0 )
		{
			CDnPetActor* pDnPetActor = dynamic_cast<CDnPetActor*>( m_hPet.GetPointer() );
			pDnPetActor->EquipItem( pDnPlayerActor->GetPetInfo().Vehicle[i] );
		}
	}

	m_RenderAvatar.RefreshEquip( m_hPet );
	m_RenderAvatar.AddRotateYaw( 25.f );
}

void CDnCashShopPetPreviewDlg::Process( float fElapsedTime )
{
	CDnCharPetPreviewDlg::Process( fElapsedTime );
	
	if( !CDnCashShopTask::IsActive() ) return;
	
	const CART_ITEM_LIST* pList = GetCashShopTask().GetPreviewCartList( PET_CLASS_ID );
	bool bEnable = false;
	if( pList != NULL && (int)pList->size() > 0 )
		bEnable = true;

#ifdef PRE_ADD_SALE_COUPON
	if( GetInterface().GetCashShopDlg()->GetCurrentTabID() == 7 )
		bEnable = false;
#endif // PRE_ADD_SALE_COUPON
	
	m_pBuyAllBtn->Enable( bEnable );
#if defined PRE_ADD_PETALSHOP || defined PRE_CHN_OBTCASHSHOP
	m_pGiftAllBtn->Enable( false );
#else
	m_pGiftAllBtn->Enable( bEnable );
#endif // defined PRE_ADD_PETALSHOP || defined PRE_CHN_OBTCASHSHOP
}

void CDnCashShopPetPreviewDlg::RefreshPetEquip()
{
	if( m_hPet == NULL ) 
		return;

	if( !m_RenderAvatar.GetActor() )
		return;

	m_RenderAvatar.RefreshEquip( m_hPet );
}

void CDnCashShopPetPreviewDlg::SetPetAvatar( int nPetItemID )
{
	if( m_hPet )
	{
		SAFE_RELEASE_SPTR( m_hPet );
		m_RenderAvatar.ResetActor();
	}

	DNTableFileFormat*  pVehicleTable = GetDNTable( CDnTableDB::TVEHICLE );
	if( !pVehicleTable )
		return;

	int nPetActorTableID = pVehicleTable->GetFieldFromLablePtr( nPetItemID, "_VehicleActorID" )->GetInteger();

	m_hPet = CreateActor( nPetActorTableID, false, false );
	if( m_hPet == NULL )
		return;

	m_hPet->SetHP( 1 );
	
	DNTableFileFormat*  pItemTable = GetDNTable( CDnTableDB::TITEM );
	if( pItemTable == NULL )
		return;

	DWORD dwPetDefaultColor1 = (DWORD)pItemTable->GetFieldFromLablePtr( nPetItemID , "_TypeParam1" )->GetInteger();
	DWORD dwPetDefaultColor2 = (DWORD)pItemTable->GetFieldFromLablePtr( nPetItemID , "_TypeParam2" )->GetInteger();

	CDnPetActor* pDnPetActor = dynamic_cast<CDnPetActor*>( m_hPet.GetPointer() );
	pDnPetActor->GetPetInfo().Vehicle[Pet::Slot::Body].nItemID = nPetItemID;
	pDnPetActor->GetPetInfo().dwPartsColor1 = dwPetDefaultColor1;
	pDnPetActor->GetPetInfo().dwPartsColor2 = dwPetDefaultColor2;

	m_RenderAvatar.SetActor( m_hPet );

	GetCashShopTask().SetPreviewCartClassId( PET_CLASS_ID );	// 펫 클래스 아이디로 구분함.
}

void CDnCashShopPetPreviewDlg::SetPetAvatarParts( int nPartsType, int nPartsItemID )
{
	if( m_hPet == NULL )
		return;

	CDnPetActor* pDnPetActor = dynamic_cast<CDnPetActor*>( m_hPet.GetPointer() );
	if( nPartsType == 0 )
		nPartsType = Pet::Slot::Accessory1;
	else if( nPartsType == 1 )
		nPartsType = Pet::Slot::Accessory2;
	pDnPetActor->GetPetInfo().Vehicle[nPartsType].nItemID = nPartsItemID;

	RefreshPetEquip();

	GetCashShopTask().SetPreviewCartClassId( PET_CLASS_ID );	// 펫 클래스 아이디로 구분함.
}

void CDnCashShopPetPreviewDlg::SetPetAvatarColor( eItemTypeEnum eItemType, DWORD dwColor )
{
	if( m_hPet == NULL ) 
		return;

	CDnPetActor* pDnPetActor = dynamic_cast<CDnPetActor*>( m_hPet.GetPointer() );
	if( eItemType == ITEMTYPE_PETCOLOR_BODY )
		pDnPetActor->GetPetInfo().dwPartsColor1 = dwColor;
	else if( eItemType == ITEMTYPE_PETCOLOR_TATOO )
		pDnPetActor->GetPetInfo().dwPartsColor2 = dwColor;

	RefreshPetEquip();

	GetCashShopTask().SetPreviewCartClassId( PET_CLASS_ID );	// 펫 클래스 아이디로 구분함.
}

void CDnCashShopPetPreviewDlg::DetachPartsWithType( int nPartsSlot )
{
	if( m_hPet == NULL )
		return;

	CDnPetActor* pDnPetActor = dynamic_cast<CDnPetActor*>( m_hPet.GetPointer() );

	int nPartsType = -1;
	if( nPartsSlot == Pet::Slot::Accessory1 )
		nPartsType = Pet::Parts::PetAccessory1;
	if( nPartsSlot == Pet::Slot::Accessory2 )
		nPartsType = Pet::Parts::PetAccessory2;

	pDnPetActor->UnEquipItem( static_cast<Pet::Parts::ePetParts>( nPartsType ) );
	pDnPetActor->GetPetInfo().Vehicle[nPartsSlot].nItemID = 0;

	RefreshPetEquip();
}

