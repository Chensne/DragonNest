#include "StdAfx.h"
#include "DnItemSlotButton.h"
#include "DnCustomControlCommon.h"
#include "DnItem.h"
#include "DnActor.h"
#include "DnItemTask.h"
#ifdef PRE_ADD_EQUIPLOCK
#include "DnItem.h"
#endif

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnItemSlotButton::CDnItemSlotButton( CEtUIDialog *pParent )
	: CDnSlotButton( pParent )
	, m_bOutlineRender(true)
	, m_bMagnetic(false)
	, m_bWearable(false)
	, m_bBlankItem(false)
	, m_bClosed(false)
	, m_nNeedCount(0)
	, m_nCurCount(0)
	, m_fImageAlpha( 1.0f )
{
	m_nTypeOutline = typeItemOutlineD;
}

CDnItemSlotButton::~CDnItemSlotButton(void)
{
}

void CDnItemSlotButton::Initialize( SUIControlProperty *pProperty )
{
	CDnSlotButton::Initialize( pProperty );
}

void CDnItemSlotButton::DrawBasicItemSlot()
{
	DWORD dwColor = m_dwIconColor;
	SUIElement *pElement(NULL);

	if( IsDragMode() )
	{
		dwColor = 0xff808080;
	}

	// 아이템 슬롯의 경우 등록을 문장보옥의 보옥 등록에서 개수모자를때 사용한다.
	//m_pParent->DrawSprite( m_hItemIcon, m_IconUV, m_bRegist ? 0xff606060 : dwColor, m_Property.UICoord );

	D3DCOLOR ImageColor = m_bRegist ? 0xff606060 : dwColor;
	int nResultAlpha = (EtClamp( (int)( m_fImageAlpha * 255 ), 0, 255) * (ImageColor>>24) / 255 );
	ImageColor = (ImageColor & 0x00ffffff) | (nResultAlpha<<24);
	m_pParent->DrawSprite( m_hItemIcon, m_IconUV, ImageColor, m_Property.UICoord );

	//if( m_ButtonState == ITEMSLOT_EXPIRE )
	//{
	//	m_pParent->DrawRect( m_Property.UICoord, 0x7f808080 );
	//}

	pElement = GetElement(m_nTypeOutline);
	//if( pElement && m_bOutlineRender && (GetSlotType() != ST_JEWEL) )	// 다시 문장보옥의 보옥 등록에서도 테두리를 보여달라고 한다.
	if( pElement && m_bOutlineRender )
	{
		if( m_bExistTemplateTexture )
			m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pElement->TemplateUVCoord, dwColor, m_Property.UICoord );
		else
			m_pParent->DrawSprite( pElement->UVCoord, dwColor, m_Property.UICoord );
	}
}

void CDnItemSlotButton::Render( float fElapsedTime )
{
	CDnSlotButton::Render(fElapsedTime);

	if( !IsShow() )
		return;

	//if( m_bSnapBack )
	//	return;

	// 아이템 갯수 나타내는 폰트색상을 블렌드해서 Current를 제대로 구해둔다.
	// SlotButton의 경우 다른 여러 상태를 사용하지 않기때문에 상태전환 코드는 수행하지 않는다.
	{
		UpdateBlendRate();
		SUIElement *pElement = GetElement(typeItemNumber);
		if( pElement )
		{
			pElement->FontColor.Blend( m_CurrentState, fElapsedTime, m_fBlendRate );
			pElement->ShadowFontColor.Blend( m_CurrentState, fElapsedTime, m_fBlendRate );
		}
	}

	SUIElement *pElement(NULL);

	// 캐릭터창 슬롯들에 장착가능 표시
#if defined(PRE_ADD_TALISMAN_SYSTEM)
	if( ((GetSlotType() == ST_CHARSTATUS) || (GetSlotType() == ST_TALISMAN)) && drag::IsValid() )
#else
	if( (GetSlotType() == ST_CHARSTATUS) && drag::IsValid() )
#endif
	{
		if( m_bWearable )
		{
			if( m_bMagnetic )
			{
				LOCAL_TIME time = CGlobalInfo::GetInstance().m_LocalTime;

				int nAlpha = 255;
				int BLEND_TIME = 500;
				float BLEND_RANGE  = 1.0f;
				int nTemp = (int)time%(BLEND_TIME+BLEND_TIME);
				if( nTemp < BLEND_TIME ) 
				{
					nAlpha = int(( BLEND_RANGE / (float)BLEND_TIME ) * nTemp * 255.0f);
				}
				else 
				{
					nAlpha = int(( BLEND_RANGE / (float)BLEND_TIME ) * ( (BLEND_TIME+BLEND_TIME) - nTemp ) *  255.0f);
				}

				nAlpha = min(nAlpha, 255);

				pElement = GetElement(typeItemWearableArea);
				if( pElement )
				{
					if( m_bExistTemplateTexture )
						m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pElement->TemplateUVCoord, D3DCOLOR_ARGB(nAlpha,255,255,255), m_Property.UICoord );
					else
						m_pParent->DrawSprite( pElement->UVCoord, D3DCOLOR_ARGB(nAlpha,255,255,255), m_Property.UICoord );
				}
			}
			else
			{
				pElement = GetElement(typeItemWearable);
				if( pElement )
				{
					if( m_bExistTemplateTexture )
						m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pElement->TemplateUVCoord, pElement->TextureColor.dwCurrentColor, m_Property.UICoord );
					else
						m_pParent->DrawSprite( pElement->UVCoord, pElement->TextureColor.dwCurrentColor, m_Property.UICoord );
				}
			}
		}
		else
		{
			pElement = GetElement(typeItemNotWear);
			if( pElement )
			{
				if( m_bExistTemplateTexture )
					m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pElement->TemplateUVCoord, pElement->TextureColor.dwCurrentColor, m_Property.UICoord );
				else
					m_pParent->DrawSprite( pElement->UVCoord, pElement->TextureColor.dwCurrentColor, m_Property.UICoord );
			}
		}
	}

	if( m_pItem )
	{
		DrawBasicItemSlot();
		if( GetSlotType() != ST_JEWEL && GetSlotType() != ST_ITEM_UPGRADE_JEWEL && GetSlotType() != ST_ITEM_COMPOUND_JEWEL
#ifdef PRE_ADD_EXCHANGE_ENCHANT
			&& GetSlotType() != ST_ITEM_UPGRADE_EXCHANGE_STUFF
#endif
			)
			RenderCount( GetElement(typeItemNumber) );
		else
			RenderJewelCount( GetElement(typeItemNumber) );

		CDnItem *pItem = (CDnItem*)m_pItem;

		if( pItem->GetItemType() == ITEMTYPE_WEAPON )
		{
			CDnWeapon *pWeapon = dynamic_cast<CDnWeapon *>(pItem);
			if( pWeapon )
			{
				float fRate = (float)pWeapon->GetDurability() / pWeapon->GetMaxDurability();
				if( fRate == 0.0f ) pElement = GetElement(typeRepair03);
				else if( fRate <= 0.1f ) pElement = GetElement(typeRepair02);
				else if( fRate <= 0.2f ) pElement = GetElement(typeRepair01);
				else pElement = NULL;
				if( pElement )
				{
					if( m_bExistTemplateTexture )
						m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pElement->TemplateUVCoord, pElement->TextureColor.dwCurrentColor, m_Property.UICoord );
					else
						m_pParent->DrawSprite( pElement->UVCoord, pElement->TextureColor.dwCurrentColor, m_Property.UICoord );
				}
#ifdef PRE_ADD_EQUIPLOCK
				else
				{
					RenderLock(pWeapon);
				}
#endif
			}
		}
		else if( pItem->GetItemType() == ITEMTYPE_PARTS )
		{
			CDnParts *pParts = dynamic_cast<CDnParts *>(pItem);
			if( pParts )
			{
				if( pParts->GetPartsType()>=CDnParts::Helmet && pParts->GetPartsType()<=CDnParts::Foot )
				{
					float fRate = (float)pParts->GetDurability() / pParts->GetMaxDurability();
					if( fRate == 0.0f ) pElement = GetElement(typeRepair03);
					else if( fRate <= 0.1f ) pElement = GetElement(typeRepair02);
					else if( fRate <= 0.2f ) pElement = GetElement(typeRepair01);
					else pElement = NULL;
					if( pElement )
					{
						if( m_bExistTemplateTexture )
							m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pElement->TemplateUVCoord, pElement->TextureColor.dwCurrentColor, m_Property.UICoord );
						else
							m_pParent->DrawSprite( pElement->UVCoord, pElement->TextureColor.dwCurrentColor, m_Property.UICoord );
					}
#ifdef PRE_ADD_EQUIPLOCK
					else
					{
						RenderLock(pParts);
					}
#endif
				}
#ifdef PRE_ADD_EQUIPLOCK
				if (pParts->IsCashItem() || (pParts->GetPartsType() > CDnParts::DefaultPartsType_Max && pParts->GetPartsType() <= CDnParts::Ring2))
				{
					RenderLock(pParts);
				}
#endif
			}
		}

		if (pItem->GetReversion() != CDnItem::NoReversion && pItem->IsSoulbBound() == false && pItem->GetSealID() > 0 )
		{
			pElement = GetElement(typeItemButton_Seal);
			if (pElement)
			{
				if( m_bExistTemplateTexture )
					m_pParent->DrawSprite(m_Template.m_hTemplateTexture, pElement->TemplateUVCoord, pElement->TextureColor.dwCurrentColor, m_Property.UICoord);
				else
					m_pParent->DrawSprite(pElement->UVCoord, pElement->TextureColor.dwCurrentColor, m_Property.UICoord);
			}
		}

		if( pItem->IsCashItem() ) {
			pElement = GetElement( typeItemCashMark );
			if( pElement )
			{
				if( m_bExistTemplateTexture )
					m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pElement->TemplateUVCoord, pElement->TextureColor.dwCurrentColor, m_Property.UICoord );
				else
					m_pParent->DrawSprite( pElement->UVCoord, pElement->TextureColor.dwCurrentColor, m_Property.UICoord );
			}
		}

		if( ST_STORAGE_FARM == m_SlotType )
			RenderExpire( GetElement(typeItemNumber) );
	}
	else if (m_bBlankItem)
	{
		DrawBasicItemSlot();

		if( GetSlotType() != ST_JEWEL && GetSlotType() != ST_ITEM_UPGRADE_JEWEL && GetSlotType() != ST_ITEM_COMPOUND_JEWEL )
			RenderCount( GetElement(typeItemNumber) );
		else
			RenderJewelCount( GetElement(typeItemNumber) );
	}

	if( m_bClosed ) {
		pElement = GetElement(typeItemButton_Closed);
		if( pElement )
		{
			if( m_bExistTemplateTexture )
				m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pElement->TemplateUVCoord, pElement->TextureColor.dwCurrentColor, m_Property.UICoord );
			else
				m_pParent->DrawSprite( pElement->UVCoord, pElement->TextureColor.dwCurrentColor, m_Property.UICoord );
		}
	}
	else {
		if( m_ButtonState == ITEMSLOT_DISABLE )
		{
			pElement = GetElement(typeItemDisable);
			if( pElement )
			{
				if( m_bExistTemplateTexture )
					m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pElement->TemplateUVCoord, pElement->TextureColor.dwCurrentColor, m_Property.UICoord );
				else
					m_pParent->DrawSprite( pElement->UVCoord, pElement->TextureColor.dwCurrentColor, m_Property.UICoord );
			}
		}
	}
}

#ifdef PRE_ADD_EQUIPLOCK
void CDnItemSlotButton::RenderLock(CDnItem* pItem)
{
	if (pItem == NULL || CDnItemTask::IsActive() == false)
		return;

	if (GetSlotType() != ST_CHARSTATUS)
		return;

	SUIElement* pElement = NULL;

	const CDnItemLockMgr& mgr = CDnItemTask::GetInstance().GetItemLockMgr();
	EquipItemLock::eLockStatus state = mgr.GetLockState(*pItem);
	
	if (state == EquipItemLock::Lock)
		pElement = GetElement(typeItemOutlineLock);
	else if (state == EquipItemLock::RequestUnLock)
		pElement = GetElement(typeItemOutlineUnlocking);
	 
	if (pElement)
	{
		if( m_bExistTemplateTexture )
			m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pElement->TemplateUVCoord, pElement->TextureColor.dwCurrentColor, m_Property.UICoord );
		else
			m_pParent->DrawSprite( pElement->UVCoord, pElement->TextureColor.dwCurrentColor, m_Property.UICoord );
	}
}
#endif

void CDnItemSlotButton::RenderJewelCount( SUIElement *pElement )
{
	if( !pElement ) return;

	// 수량표시하는 곳의 작은 검은색 영역은 그리지 않는다.
	//m_pParent->DrawSprite( pElement->UVCoord, pElement->TextureColor.dwCurrentColor, m_Property.UICoord );

	WCHAR wszBuffer[32]={0};
	if( m_nCurCount < m_nNeedCount )
	{
		// 색상이 다르니 나눠서 렌더링, 대신 가운데 정렬 구할땐 합쳐놓고 구해야한다.
		swprintf_s(wszBuffer, _countof(wszBuffer), L"%d/%d", m_nCurCount, m_nNeedCount);
		SUICoord uiTextCoord;
		m_pParent->CalcTextRect( wszBuffer, pElement, uiTextCoord );
		float fX = m_Property.UICoord.fX+(m_Property.UICoord.fWidth-uiTextCoord.fWidth)/2;	// 아이콘의 가운데 정렬

		swprintf_s(wszBuffer, _countof(wszBuffer), L"%d", m_nCurCount);
		m_pParent->CalcTextRect( wszBuffer, pElement, uiTextCoord );
		uiTextCoord.fX = fX;
		uiTextCoord.fY = m_Property.UICoord.Bottom()-uiTextCoord.fHeight;
		uiTextCoord.fY -= 0.003f;
		m_pParent->DrawDlgText( wszBuffer, pElement, EtInterface::textcolor::RED, uiTextCoord );

		swprintf_s(wszBuffer, _countof(wszBuffer), L"/%d", m_nNeedCount);
		SUICoord uiTextCoord2;
		m_pParent->CalcTextRect( wszBuffer, pElement, uiTextCoord2 );
		uiTextCoord2.fX = uiTextCoord.fX + uiTextCoord.fWidth;
		uiTextCoord2.fY = uiTextCoord.fY;
		m_pParent->DrawDlgText( wszBuffer, pElement, EtInterface::textcolor::GOLD, uiTextCoord2 );
	}
	else
	{
		swprintf_s(wszBuffer, _countof(wszBuffer), L"%d/%d", m_nCurCount, m_nNeedCount);
		SUICoord uiTextCoord;
		m_pParent->CalcTextRect( wszBuffer, pElement, uiTextCoord );
		uiTextCoord.fX = m_Property.UICoord.fX+(m_Property.UICoord.fWidth-uiTextCoord.fWidth)/2;	// 아이콘의 가운데 정렬
		uiTextCoord.fY = m_Property.UICoord.Bottom()-uiTextCoord.fHeight;
		uiTextCoord.fY -= 0.003f;
		m_pParent->DrawDlgText( wszBuffer, pElement, EtInterface::textcolor::GOLD, uiTextCoord );
	}
}

void CDnItemSlotButton::RenderDrag( float fElapsedTime )
{
	if (IsDragMode() == false)
		return;

	//if( !m_bSnap )
	{
		POINT CursorPoint;
		GetCursorPos( &CursorPoint );
		ScreenToClient( GetEtDevice()->GetHWnd(), &CursorPoint );

		m_CursorCoord = m_Property.UICoord;
		m_CursorCoord.fWidth /= m_pParent->GetScreenWidthRatio();
		m_CursorCoord.fHeight /= m_pParent->GetScreenHeightRatio();
		m_CursorCoord.SetPosition( CursorPoint.x / ( float )GetEtDevice()->Width() - m_CursorCoord.fWidth * 0.5f,
									CursorPoint.y / ( float )GetEtDevice()->Height() - m_CursorCoord.fHeight * 0.5f );
	}
	CEtSprite::GetInstance().DrawSprite( (EtTexture*)m_hItemIcon->GetTexturePtr(), m_hItemIcon->Width(), m_hItemIcon->Height(), m_IconUV, 0xffffffff, m_CursorCoord, 0.0f );

	//if( ((CDnItem*)m_pItem)->GetItemType() == CDnItem::Jewel )
	//	return;

	SUIElement *pElement = GetElement(m_nTypeOutline);
	if( !pElement ) return;
	EtTextureHandle hTexture = m_pParent->GetUITexture();
	if( m_bExistTemplateTexture )
		hTexture = m_Template.m_hTemplateTexture;
	if( !hTexture ) return;
	SUICoord UVCoord = pElement->UVCoord;
	if( m_bExistTemplateTexture )
		UVCoord = pElement->TemplateUVCoord;
	CEtSprite::GetInstance().DrawSprite( (EtTexture*)hTexture->GetTexturePtr(), hTexture->Width(), hTexture->Height(), UVCoord, pElement->TextureColor.dwCurrentColor, m_CursorCoord, 0.0f );

	RenderDragCount( GetElement(typeItemNumber), m_CursorCoord );
}

void CDnItemSlotButton::SetBlankItem( bool bEnable )
{
	if( bEnable ) {
		CDnSlotButton::SetItem(NULL, CDnSlotButton::NO_COUNTABLE_RENDER);
		m_nTypeOutline = typeItemOutlineD;
		m_bBlankItem = true;
	}
	else {
		m_bBlankItem = false;
	}
}

void CDnItemSlotButton::SetItem(MIInventoryItem *pItem, int count)
{
	CDnSlotButton::SetItem(pItem, count);

	if (pItem == NULL)
		return;

	m_nTypeOutline = typeItemOutlineD;

	// CDnQuickSlotButton::SetQuickItem 에도 같은 코드 있으니 변경시 같이 변경할 것.
	eItemTypeEnum ItemType = ((CDnItem*)pItem)->GetItemType();
	if( ItemType == ITEMTYPE_WEAPON ||
		ItemType == ITEMTYPE_PARTS ||
		ItemType == ITEMTYPE_RANDOM ||
		( ( ItemType == ITEMTYPE_NORMAL || ItemType == ITEMTYPE_COOKING ) && ((CDnItem*)pItem)->GetSkillID() > 0 ) )
	{
		switch( ((CDnItem*)pItem)->GetItemRank() )
		{
		case ITEMRANK_D:	m_nTypeOutline = typeItemOutlineD;	break;
		case ITEMRANK_C:	m_nTypeOutline = typeItemOutlineC;	break;
		case ITEMRANK_B:	m_nTypeOutline = typeItemOutlineB;	break;
		case ITEMRANK_A:	m_nTypeOutline = typeItemOutlineA;	break;
		case ITEMRANK_S:	m_nTypeOutline = typeItemOutlineS;	break;
		case ITEMRANK_SS:	m_nTypeOutline = typeItemOutlineLG;	break;
		case ITEMRANK_SSS:  m_nTypeOutline = typeMiscItemOutlineR; break;
		}
	}
	else
	{
		switch( ((CDnItem*)pItem)->GetItemRank() )
		{
		case ITEMRANK_D:	m_nTypeOutline = typeMiscItemOutlineN;	break;
		case ITEMRANK_C:	m_nTypeOutline = typeMiscItemOutlineC;	break;
		case ITEMRANK_B:	m_nTypeOutline = typeMiscItemOutlineB;	break;
		case ITEMRANK_A:	m_nTypeOutline = typeMiscItemOutlineA;	break;
		case ITEMRANK_S:	m_nTypeOutline = typeMiscItemOutlineS;	break;
		case ITEMRANK_SS:	m_nTypeOutline = typeItemOutlineLG;	break;
		case ITEMRANK_SSS:  m_nTypeOutline = typeMiscItemOutlineR; break;
		}
	}

	m_bBlankItem = false;
}

void CDnItemSlotButton::SetJewelCount( int nNeedCount, int nCurCount, bool bCorrection )
{
	m_nNeedCount = nNeedCount;
	m_nCurCount = nCurCount;

	if( bCorrection )
	{
		// 필요수치보다 현재수치가 많더라도 필요수치만큼으로 렌더링한다.
		if( m_nCurCount > m_nNeedCount )
			m_nCurCount = m_nNeedCount;
	}
}

void CDnItemSlotButton::Enable( bool bEnable )
{
	CDnSlotButton::Enable( bEnable );
	SetImageAlpha( bEnable ? 1.0f : 0.3f );
}
