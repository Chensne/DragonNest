#include "StdAfx.h"
#include "DnQuickSlotButton.h"
#include "DnActor.h"
#include "DnSkill.h"
#include "DnItem.h"
#include "DnMainDlg.h"
#include "DnItemTask.h"
#include "DnPlayerActor.h"
#include "DnLocalPlayerActor.h"
#ifdef PRE_ADD_SKILLCOOLTIME
#include "GameOption.h"
#endif

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#define TOGGLE_BLEND_TIME	0.8f

CDnQuickSlotButton::CDnQuickSlotButton( CEtUIDialog *pParent )
	: CDnSlotButton( pParent )
{
	m_nButtonOrder = -1;
	m_nItemCondition = -1;
	m_fItemCoolTime = 0.0f;
	m_fPrevCoolTime = 0.0f;
	m_fCompleteCoolTime = 0.0f;
	m_fConst = 1.0f;
	m_fProgressSize = 0.0f;
	m_fToggleTime = 0.0f;

	m_IconUVCoord.fWidth = 1.0f;
	m_IconUVCoord.fHeight = 1.0f;

	m_nTypeOutline = typeItemOutline;
	m_nTypeBlack = typeItemBlack;
	m_nTypeRed = typeItemRed;

	m_pElementToggle1 = NULL;
	m_pElementToggle2 = NULL;

	m_uiControlState1 = UI_STATE_NORMAL;
	m_uiControlState2 = UI_STATE_HIDDEN;
	m_bToggleBegin = true;

	m_CompleteCoolTimeColor = EtColor((DWORD)0x00ffffff);
	m_nTabIndex = -1;
	m_bDragFlag = false;

	m_bForceUsable = false;
	m_bForceUnUsable = false;
	m_iRootJobID = 0;

#ifdef _QUICKSLOT_USE_EFFECT
	m_fSizeRate = 1.0f;
	m_fSizeAlpha = 0.0f;
	m_fSizeTime = 0.0f;
	m_fSizeReadyTime = 0.0f;
	m_fSizeConst = 0.0f;
#endif

#ifdef PRE_ADD_LEVELUP_REWARD_NOTIFIER
	m_nEndBlinkTime = 0;
	m_nProcessBlinkTime = 0;
#endif 
}

CDnQuickSlotButton::~CDnQuickSlotButton(void)
{
	SAFE_RELEASE_SPTRVEC( m_hVecTextureList );
#ifdef _QUICKSLOT_USE_EFFECT
	SAFE_RELEASE_SPTR( m_hQuickSlotUse );
#endif
}

void CDnQuickSlotButton::Initialize( SUIControlProperty *pProperty )
{
	m_hSkillIconLight = LoadResource( CEtResourceMng::GetInstance().GetFullName( "SkillButtonLight.dds" ).c_str(), RT_TEXTURE );
	m_hItemIconLight = LoadResource( CEtResourceMng::GetInstance().GetFullName( "ItemButtonLight.dds" ).c_str(), RT_TEXTURE );
	m_hShortCutIcon = LoadResource( CEtResourceMng::GetInstance().GetFullName( "MainBarShortCut.dds" ).c_str(), RT_TEXTURE );
	
	m_hVecTextureList.push_back( m_hSkillIconLight );
	m_hVecTextureList.push_back( m_hItemIconLight );
	m_hVecTextureList.push_back( m_hShortCutIcon );

#ifdef _QUICKSLOT_USE_EFFECT
	m_hQuickSlotUse = LoadResource( CEtResourceMng::GetInstance().GetFullName( "SkillOnEffect.dds" ).c_str(), RT_TEXTURE );
#endif

	CDnSlotButton::Initialize( pProperty );

	m_ShortCutCoord = m_Property.UICoord;
	m_ShortCutCoord.fWidth = (float)QUICKSLOT_SHORCUT_ICON_XSIZE / DEFAULT_UI_SCREEN_WIDTH;
	m_ShortCutCoord.fHeight = (float)QUICKSLOT_SHORCUT_ICON_YSIZE / DEFAULT_UI_SCREEN_HEIGHT;
	m_ShortCutCoord.fY = m_Property.UICoord.Bottom() - m_ShortCutCoord.fHeight;

	m_pElementToggle1 = GetElement(typeSkillToggle01);
	m_pElementToggle2 = GetElement(typeSkillToggle02);

#ifdef _QUICKSLOT_USE_EFFECT
	GetUICoord( m_uiDefaultCoord );
#endif
}

void CDnQuickSlotButton::SetButtonOrder( int nOrder )
{
	m_nButtonOrder = nOrder;
	//CalcButtonUV( nOrder, m_hShortCutIcon, m_ShortCutUV, QUICKSLOT_SHORCUT_ICON_XSIZE, QUICKSLOT_SHORCUT_ICON_YSIZE );
}

void CDnQuickSlotButton::SetHotKey( int nKey )
{
	CEtUIControl::SetHotKey( nKey );
	CalcHotKeyUV( nKey, m_hShortCutIcon, m_ShortCutUV, QUICKSLOT_SHORCUT_ICON_XSIZE, QUICKSLOT_SHORCUT_ICON_YSIZE );
}

void CDnQuickSlotButton::SetTabIndex( int nIndex )
{
	m_nTabIndex = nIndex;
}

int CDnQuickSlotButton::GetItemSlotIndex()
{
	return GetTabIndex() * CDnMainDlg::QUICKSLOT_BUTTON_MAX + GetSlotIndex();
}

void CDnQuickSlotButton::SetQuickItem( MIInventoryItem *pItem )
{
	ASSERT(pItem&&"CDnQuickSlotButton::SetQuickItem");

	CDnSlotButton::SetItem(pItem, CDnSlotButton::ITEM_ORIGINAL_COUNT);
	
	switch( pItem->GetType() )
	{
	case MIInventoryItem::Skill:
		{
			m_hCurIcon = m_hSkillIcon;
			m_hCurIconLight = m_hSkillIconLight;
			m_nTypeOutline = typeSkillOutline;
			m_nTypeBlack = typeSkillBlack;
			m_nTypeRed = typeSkillRed;
		}
		break;
	case MIInventoryItem::Item:
		{
			m_hCurIcon = m_hItemIcon;
			m_hCurIconLight = m_hItemIconLight;

			eItemTypeEnum ItemType = ((CDnItem*)pItem)->GetItemType();
			if( ItemType == ITEMTYPE_WEAPON ||
				ItemType == ITEMTYPE_PARTS ||
				ItemType == ITEMTYPE_RANDOM ||
				( ( ItemType == ITEMTYPE_NORMAL || ItemType == ITEMTYPE_COOKING ) && ((CDnItem*)pItem)->GetSkillID() > 0 ) )
			{
				switch( ((CDnItem*)pItem)->GetItemRank() )
				{
				case ITEMRANK_D:	m_nTypeOutline = typeItemOutline;	break;
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

			m_nTypeBlack = typeItemBlack;
			m_nTypeRed = typeItemRed;
		}
		break;
	default:
		ASSERT(0&&"CDnQuickSlotButton::SetQuickItem");
		break;
	}
}

void CDnQuickSlotButton::Process( float fElapsedTime )
{
	// 버리기의 경우 렌더에 있었는데, m_bShow여부와 관계없이 처리가능해야해서 Process로 옮긴다.
	if( !CDnActor::s_hLocalActor ) return;
	if( !m_pItem )
	{
		ResetSlot();
#ifdef _QUICKSLOT_USE_EFFECT
		// 퀵슬롯 사용 이펙트 리셋할때 같이 초기화한다.
		// (퀵슬롯 사용 이펙트 시작하다가 리셋슬롯되면 잔영 그대로 남을수도 있기때문)
		m_fSizeRate = 1.0f;
#endif
		return;
	}

	// 멤버값은 제대로 가지고 있는지 확인.
	static ITEM_SLOT_TYPE s_SlotType = (ITEM_SLOT_TYPE)0;
	static MIInventoryItem::InvenItemTypeEnum s_ItemType = (MIInventoryItem::InvenItemTypeEnum)0;
	static MIInventoryItem *s_pItem = NULL;
	s_SlotType = m_SlotType;
	s_ItemType = m_itemType;
	s_pItem = m_pItem;

	// Parent가 쓰레기값이라면 this가 깨진걸 의심해야 하지 않을까?
	bool bCheckShow = true;
	if( !m_bShow ) bCheckShow = false;
	if( !m_pParent ) bCheckShow = false;
	if( bCheckShow )
	{
		if( m_pParent && m_pParent->IsShow() == false ) bCheckShow = false;
		if( m_pParent && m_pParent->GetParentDialog() && m_pParent->GetParentDialog()->IsShow() == false ) bCheckShow = false;
	}

	// Note : 아래 m_pItem의 포인터가 잘못되었다면 어딘가에서 아이템이나 스킬을 다시 생성했다.
	//		ex). CDnItemTask::GetInstance().CreateItem();
	//

	// 캐시인벤같이 무한으로 확장되는 슬롯이 많아지면서 프로세스에서 프레임저하가 일어난다.
	// 확인해보니 GetItemCondition내부에서 물약같은 스킬연계된 아이템에 대해 처리 루틴이 좀 복잡하게 되어있어서 그랬던 것.
	// 그래서 보여지는 창에 한해서만 구하도록 하겠다.
	if( GetSlotType() == ST_INVENTORY_CASH )
	{
		if( m_pParent->IsShow() )
			m_nItemCondition = m_pItem->GetItemCondition();
	}
	else
	{
		// 컨디션 체크는 렌더되는 순간에 돌려도 상관없으므로 Show상태 판단해서 돌리기로 한다.
		if( bCheckShow )
			m_nItemCondition = m_pItem->GetItemCondition();
	}

	// 쿨은 다 돌아왔을때 반짝 하는 효과때문에 하이드 상태에서도 돌아야한다.
	bool bCheckCoolTime = true;
	if( m_bForceUsable || m_bForceUnUsable ) bCheckCoolTime = false;
	if( GetSlotType() == ST_INVENTORY_VEHICLE || GetSlotType() == ST_INVENTORY_QUEST || GetSlotType() == ST_INVENTORY_CASHSHOP ) bCheckCoolTime = false;
	if( bCheckCoolTime )
	{
		if( m_itemType == MIInventoryItem::Item )
		{
			m_fItemCoolTime = m_pItem->GetCoolTime();
		}
		else if( m_itemType == MIInventoryItem::Skill )
		{
			if( GetSlotType() == ST_QUICKSLOT || GetSlotType() == ST_SKILL )
			{
				// 어쩔 수 없다. 우선은 dynamic_cast라도 사용.
				CDnSkill *pSkill = dynamic_cast<CDnSkill*>(m_pItem);
				if( pSkill )
					m_fItemCoolTime = pSkill->GetCoolTime();
			}
			else
			{
				m_fItemCoolTime = m_pItem->GetCoolTime();
			}
		}
	}

#if defined( PRE_ADD_ACADEMIC )
	if( GetSlotType() == ST_QUICKSLOT && m_itemType == MIInventoryItem::Skill && m_pParent && m_pParent->IsShow() )
	{
		CDnSkill *pSkill = dynamic_cast<CDnSkill*>(m_pItem);
		if(pSkill && pSkill->GetDurationType() == CDnSkill::DurationTypeEnum::ActiveToggleForSummon )
		{
			if( pSkill->IsEanbleActiveToggleCoolTime() ) 
				m_fItemCoolTime = m_pItem->GetCoolTime();
			else
				m_fItemCoolTime = 0.f;
		}

		if( pSkill && pSkill->GetGlyphActiveSkillDisable() )
			m_nItemCondition = MIInventoryItem::Unusable;
	}
#endif


	// 퀵슬롯에 등록한 아이템이나 현재 보여지는 인벤토리 아이템라면, 허용가능한 맵인지 한번 더 검사
	bool bMapCheck = false;
	if( m_itemType == MIInventoryItem::Item && m_nItemCondition == MIInventoryItem::Usable )
	{
		if( GetSlotType() == ST_INVENTORY || GetSlotType() == ST_INVENTORY_CASH || GetSlotType() == ST_QUICKSLOT )
		{
			if( m_pParent->IsShow() ) bMapCheck = true;
		}
	}
	if( bMapCheck )
	{
		CDnItem *pItem = (CDnItem*)m_pItem;
		if( GetItemTask().IsDisableItemInCharInven( pItem ) )
			m_nItemCondition = MIInventoryItem::Unusable;
	}

#if defined(PRE_ADD_STAGE_USECOUNT_ITEM)
	CDnItem *pItem = (CDnItem*)m_pItem;
	if(pItem && pItem->IsUseLimited())
		m_nItemCondition = MIInventoryItem::Unusable;
#endif

	if( m_fItemCoolTime > 0.f )
	{
#ifdef _QUICKSLOT_USE_EFFECT
		if( GetSlotType() == ST_QUICKSLOT )
		{
			if( m_fPrevCoolTime == 0.f )
			{
				// fReadyTime만큼 초기상태에서 대기 후 fTime 동안 변경된다.
				SetStartCoolTime( 0.1f, 0.25f );
			}
		}
#endif

		m_fPrevCoolTime = m_fItemCoolTime;

		// 10.0f의 의미는 위, 아래 5.0f씩 빼는 크기다.
		//m_fProgressSize = (m_Property.UICoord.fHeight - (10.0f/DEFAULT_UI_SCREEN_HEIGHT)) * m_fItemCoolTime;
		// 전에 m_fProgressSize는 검은색 음영이 전체를 가리다가 Height가 줄어들어 끝나는 형태였는데,
		// 이번엔 반대로 붉은색이 아래쪽부터 시작해서 점점 커지는 형태다.
		m_fProgressSize = (m_Property.UICoord.fHeight - (10.0f/DEFAULT_UI_SCREEN_HEIGHT)) * (1.0f - m_fItemCoolTime);
	}
	else 
	{
		if( m_fPrevCoolTime != 0.f )
		{
			m_fPrevCoolTime = 0.f;
			SetCompleteCoolTime( 0.75f );
		}
	}

	if( m_fCompleteCoolTime > 0.0f )
	{
		EtColorLerp( &m_CompleteCoolTimeColor, &EtColor((DWORD)0xffffffff), &EtColor((DWORD)0x00ffffff), 1.0f-(1/powf(m_fConst,2)*powf(m_fCompleteCoolTime,2) ) );
		m_fCompleteCoolTime -= fElapsedTime;
	}
	else
	{
		m_CompleteCoolTimeColor = EtColor((DWORD)0x00ffffff);
	}

#ifdef _QUICKSLOT_USE_EFFECT
	if( GetSlotType() == ST_QUICKSLOT )
	{
		// ReadyTime이 먼저 감소하고, 다 감소하면 SizeRate에 사용되는 SizeTime이 감소한다.
		if( m_fSizeReadyTime > 0.0f || m_fSizeTime > 0.0f )
		{
			if( m_fSizeReadyTime > 0.0f )
			{
				m_fSizeReadyTime -= fElapsedTime;
			}
			else
			{
				m_fSizeAlpha = 1.0f + (1.0f-(1/powf(m_fSizeConst,2)*powf(m_fSizeTime,2))) * ( 0 - 1.0f );	// 1.0f 에서 0.0f 으로 Lerp
				m_fSizeRate = 1.0f + m_fSizeAlpha * 0.75f;
				m_fSizeTime -= fElapsedTime;
			}
		}
		else
		{
			m_fSizeRate = 1.0f;
			m_fSizeAlpha = 0.0f;
		}
	}
#endif

#ifdef PRE_ADD_LEVELUP_REWARD_NOTIFIER
	if( IsBlink() )
	{
		m_nProcessBlinkTime += int( fElapsedTime * 1000 ); 
		if( m_nProcessBlinkTime >= m_nEndBlinkTime )
		{
			SetBlink( false );
			m_nProcessBlinkTime = 0;
			m_nEndBlinkTime = 0;
		}
	}
#endif 

}

#ifdef PRE_ADD_SKILLCOOLTIME
void CDnQuickSlotButton::RenderCoolTimeCounter(const float& fElapsedTime, const float& fMaxCoolTime, const float& fPresentCoolTime)
{
	SUIElement *pElement = GetElement(typeCooltimeCount);
	if (pElement)
	{
		WCHAR wszBuffer[32]={0};

		if (fMaxCoolTime < 1.f)
		{
			swprintf_s(wszBuffer, L"%.1f", fPresentCoolTime);
#ifdef PRE_MOD_SKILLCOOLTIME
			m_CurrentState = UI_STATE_PRESSED;
#endif
		}
		else
		{
#ifdef PRE_MOD_SKILLCOOLTIME
			int nCoolTime = (int)fPresentCoolTime;
			int nCipher = CommonUtil::GetCipherDecimal(nCoolTime);

			if (nCipher == 3)
				m_CurrentState = UI_STATE_MOUSEENTER;
			else if (nCipher == 1)
				m_CurrentState = UI_STATE_PRESSED;
			else
				m_CurrentState = UI_STATE_NORMAL;

			_itow_s(nCoolTime, wszBuffer, 32, 10);
#else
			_itow_s((int)fPresentCoolTime, wszBuffer, 32, 10);
#endif
		}

		SUICoord uiTextCoord;
		m_pParent->CalcTextRect( wszBuffer, pElement, uiTextCoord );
		uiTextCoord.fX = m_Property.UICoord.fX + (m_Property.UICoord.fWidth - uiTextCoord.fWidth) * 0.5f;
		uiTextCoord.fY = m_Property.UICoord.fY + (m_Property.UICoord.fHeight - uiTextCoord.fHeight) * 0.5f;

		pElement->FontColor.Blend(m_CurrentState, fElapsedTime, m_fBlendRate);
		pElement->ShadowFontColor.Blend(m_CurrentState, fElapsedTime, m_fBlendRate);

		m_pParent->DrawDlgText( wszBuffer, pElement, pElement->FontColor.dwCurrentColor, uiTextCoord );	
	}
}
#endif

void CDnQuickSlotButton::Render( float fElapsedTime )
{
	if( !m_bShow ) return;

#ifdef _QUICKSLOT_USE_EFFECT
	if( GetSlotType() == ST_QUICKSLOT )
	{
		SetSize( m_uiDefaultCoord.fWidth * m_fSizeRate, m_uiDefaultCoord.fHeight * m_fSizeRate );
		float fOffsetX = (m_fSizeRate - 1.0f) * m_uiDefaultCoord.fWidth / 2.0f;
		float fOffsetY = (m_fSizeRate - 1.0f) * m_uiDefaultCoord.fHeight / 2.0f;
		SetPosition( m_uiDefaultCoord.fX - fOffsetX, m_uiDefaultCoord.fY - fOffsetY );
	}
#endif

	// 아이템 갯수 나타내는 폰트색상을 블렌드해서 Current를 제대로 구해둔다.
	// SlotButton의 경우 다른 여러 상태를 사용하지 않기때문에 상태전환 코드는 수행하지 않는다.
	{
		UpdateBlendRate();
		SUIElement *pElement = GetElement(typeItemCount);
		if( pElement )
		{
			pElement->FontColor.Blend( m_CurrentState, fElapsedTime, m_fBlendRate );
			pElement->ShadowFontColor.Blend( m_CurrentState, fElapsedTime, m_fBlendRate );
		}
	}
#ifdef PRE_ADD_LEVELUP_REWARD_NOTIFIER
	UpdateBlink( fElapsedTime );
#endif

	if( m_pItem )
	{
		DWORD dwColor = m_dwIconColor;

		if( IsDragMode() )
		{
			dwColor = 0xff808080;
		}

		SUIElement *pElement(NULL);
		bool bToggle(false);

		if( m_bForceUsable || m_bForceUnUsable )
		{
			if( m_bForceUsable )
			{
#ifdef PRE_ADD_LEVELUP_REWARD_NOTIFIER
				D3DXCOLOR colorMouseEnter( dwColor );
				colorMouseEnter.a = m_nBlinkAlpha / 255.0f;
				dwColor = colorMouseEnter;
#endif 	
				m_pParent->DrawSprite( m_hCurIcon, m_IconUV, dwColor, m_Property.UICoord );

				// 테두리도 정해진 색깔 그대로,,
				pElement = GetElement(m_nTypeOutline);			
				if( pElement )
				{
#ifdef PRE_ADD_LEVELUP_REWARD_NOTIFIER
					D3DXCOLOR colorMouseEnter( /*pElement->TextureColor.dwColor[UI_STATE_MOUSEENTER]*/pElement->TextureColor.dwCurrentColor );
					colorMouseEnter.a = m_nBlinkAlpha / 255.0f;
					pElement->TextureColor.dwCurrentColor = colorMouseEnter;
#endif 	

					if( m_bExistTemplateTexture )
						m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pElement->TemplateUVCoord, pElement->TextureColor.dwCurrentColor, m_Property.UICoord );
					else
						m_pParent->DrawSprite( pElement->UVCoord, pElement->TextureColor.dwCurrentColor, m_Property.UICoord );
				}
			}
			else
			if( m_bForceUnUsable )
			{
				// 따로 로드한 검은색 아이콘 드로우~
				m_pParent->DrawSprite( m_hSkillGrayIcon, m_IconUV, dwColor, m_Property.UICoord );

				// 테두리도 회색으로..
				pElement = GetElement(typeSkillOutlineDisabled);

				D3DXCOLOR colorMouseEnter( /*pElement->TextureColor.dwColor[UI_STATE_MOUSEENTER]*/pElement->TextureColor.dwCurrentColor );
				colorMouseEnter.a = m_nBlinkAlpha / 255.0f;
				pElement->TextureColor.dwCurrentColor = colorMouseEnter;

				if( pElement )
				{
#ifdef PRE_ADD_LEVELUP_REWARD_NOTIFIER
					D3DXCOLOR colorMouseEnter( /*pElement->TextureColor.dwColor[UI_STATE_MOUSEENTER]*/pElement->TextureColor.dwCurrentColor );
					colorMouseEnter.a = m_nBlinkAlpha / 255.0f;
					pElement->TextureColor.dwCurrentColor = colorMouseEnter;
#endif 	
					if( m_bExistTemplateTexture )
						m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pElement->TemplateUVCoord, pElement->TextureColor.dwCurrentColor, m_Property.UICoord );
					else
						m_pParent->DrawSprite( pElement->UVCoord, pElement->TextureColor.dwCurrentColor, m_Property.UICoord );
				}

				//pElement = GetElement(m_nTypeBlack);
				//if( pElement )
				//{
				//	m_pParent->DrawSprite( pElement->UVCoord, pElement->TextureColor.dwCurrentColor, m_Property.UICoord );
				//}
			}

			// 음영처리. 등록이 되어있거나, 쿨타임중이거나. 그런거 상관없이 일괄로 enable/disable 상태로먄 표시.
			// 새로 얻은 것은 new 로 표시해준다.
			if( m_bForceUsable )
			{
				CDnSkill* pSkill = static_cast<CDnSkill*>(m_pItem);
				if( pSkill->IsNewAcquire() )
				{
					SUIElement *pElement = GetElement(typeItemNew);
					if( pElement )
					{
#ifdef PRE_ADD_LEVELUP_REWARD_NOTIFIER
						D3DXCOLOR colorMouseEnter( /*pElement->TextureColor.dwColor[UI_STATE_MOUSEENTER]*/pElement->TextureColor.dwCurrentColor );
						colorMouseEnter.a = m_nBlinkAlpha / 255.0f;
						pElement->TextureColor.dwCurrentColor = colorMouseEnter;
#endif 	
						if( m_bExistTemplateTexture )
							m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pElement->TemplateUVCoord, pElement->TextureColor.dwCurrentColor, m_Property.UICoord );
						else
							m_pParent->DrawSprite( pElement->UVCoord, pElement->TextureColor.dwCurrentColor, m_Property.UICoord );
					}
				}
			}

		}
		else
		{
			// 음영처리. 등록이 되어있거나, 쿨타임중이거나.
			bool bShadow = false;
			if( m_bRegist || m_fItemCoolTime > 0.0f ) bShadow = true;

#ifdef PRE_ADD_LEVELUP_REWARD_NOTIFIER		
			dwColor = bShadow ? 0xff808080 : dwColor;
			D3DXCOLOR colorMouseEnter( dwColor );	
			colorMouseEnter.a = m_nBlinkAlpha / 255.0f;
			dwColor = colorMouseEnter;
			m_pParent->DrawSprite( m_hCurIcon, m_IconUV, dwColor, m_Property.UICoord );
#else 	
			m_pParent->DrawSprite( m_hCurIcon, m_IconUV, bShadow ? 0xff808080 : dwColor, m_Property.UICoord );
#endif

			switch( m_nItemCondition )
			{
				case MIInventoryItem::Usable:
					break;
				case MIInventoryItem::NotEnoughCondition:
					{
						pElement = GetElement(m_nTypeRed);
						if( pElement )
						{
#ifdef PRE_ADD_LEVELUP_REWARD_NOTIFIER
							D3DXCOLOR colorMouseEnter( /*pElement->TextureColor.dwColor[UI_STATE_MOUSEENTER]*/pElement->TextureColor.dwCurrentColor );
							colorMouseEnter.a = m_nBlinkAlpha / 255.0f;
							pElement->TextureColor.dwCurrentColor = colorMouseEnter;
#endif 	

							if( m_bExistTemplateTexture )
								m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pElement->TemplateUVCoord, pElement->TextureColor.dwCurrentColor, m_Property.UICoord );
							else
								m_pParent->DrawSprite( pElement->UVCoord, pElement->TextureColor.dwCurrentColor, m_Property.UICoord );
						}
					}
					break;
				case MIInventoryItem::Toggle:
					{
						bToggle = true;

						if( m_bToggleBegin )
						{
							m_pElementToggle1->TextureColor.SetState( UI_STATE_NORMAL );
							m_pElementToggle2->TextureColor.SetState( UI_STATE_HIDDEN );
							m_bToggleBegin = false;
							m_fToggleTime = 0.0f;
						}
					}
					break;
				case MIInventoryItem::Unusable:
				default:
					{
						if( m_fItemCoolTime > 0.f || m_SlotType == ST_INVENTORY_CASHSHOP) 
							break;

						pElement = GetElement(m_nTypeBlack);
						if( pElement )
						{
#ifdef PRE_ADD_LEVELUP_REWARD_NOTIFIER
							D3DXCOLOR colorMouseEnter( /*pElement->TextureColor.dwColor[UI_STATE_MOUSEENTER]*/pElement->TextureColor.dwCurrentColor );
							colorMouseEnter.a = m_nBlinkAlpha / 255.0f;
							pElement->TextureColor.dwCurrentColor = colorMouseEnter;
#endif 	
							if( m_bExistTemplateTexture )
								m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pElement->TemplateUVCoord, pElement->TextureColor.dwCurrentColor, m_Property.UICoord );
							else
								m_pParent->DrawSprite( pElement->UVCoord, pElement->TextureColor.dwCurrentColor, m_Property.UICoord );
						}
					}
					break;
			}

			if( m_pItem->GetType() == MIInventoryItem::Item )
			{
				CDnItem *pItem = (CDnItem*)m_pItem;
				if( pItem->IsNeedJob() == true && CDnActor::s_hLocalActor )
				{
					std::vector<int> nVecJobList;
					((CDnPlayerActor*)CDnActor::s_hLocalActor.GetPointer())->GetJobHistory( nVecJobList );
					if( !pItem->IsPermitPlayer( nVecJobList ) )
					{
						pElement = GetElement(typeItemUnusable);
						if( pElement )
						{
#ifdef PRE_ADD_LEVELUP_REWARD_NOTIFIER
							D3DXCOLOR colorMouseEnter( /*pElement->TextureColor.dwColor[UI_STATE_MOUSEENTER]*/pElement->TextureColor.dwCurrentColor );
							colorMouseEnter.a = m_nBlinkAlpha / 255.0f;
							pElement->TextureColor.dwCurrentColor = colorMouseEnter;
#endif 	

							if( m_bExistTemplateTexture )
								m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pElement->TemplateUVCoord, pElement->TextureColor.dwCurrentColor, m_Property.UICoord );
							else
								m_pParent->DrawSprite( pElement->UVCoord, pElement->TextureColor.dwCurrentColor, m_Property.UICoord );
						}
					}
				}
			}

			if( m_fItemCoolTime > 0.0f )
			{
				// 쿨타임 빔 아래 부분은 붉은 색으로 그린다.
				SUICoord uiCoord(m_Property.UICoord);

				// 아래부분 시작점은 기본 fY + 기본 fHeight - 5픽셀 - 프로그레스 크기.
				uiCoord.fY += (uiCoord.fHeight - (5.0f / DEFAULT_UI_SCREEN_HEIGHT) - m_fProgressSize);
				uiCoord.fHeight = m_fProgressSize;

				pElement = GetElement(typeSkillPassive);
				if( pElement )
				{
					SUICoord uvCoord(pElement->UVCoord);
					if( m_bExistTemplateTexture )
						uvCoord = pElement->TemplateUVCoord;
					//uvCoord.fY += (5.0f / DEFAULT_UI_SCREEN_HEIGHT);
					//uvCoord.fHeight = m_fProgressSize * DEFAULT_UI_SCREEN_HEIGHT / m_pParent->GetUITexture()->Height();
					//m_pParent->DrawSprite( uvCoord, pElement->TextureColor.dwCurrentColor, uiCoord );
					// 위 코드에 버그 있었다.
					// uvCoord는 말그대로 Dialog가 사용하는 텍스처의 uvCoord인데,
					// uiCoord 계산하듯이 DEFAULT_UI_SCREEN_HEIGHT로 int->float 변환을 하면 당연히 안된다.
					// 전에는 우연히 uv가 잘 맞아서 잘 나왔던거겠지만,
					// 조금만 실수하면 uv밀려서 아무것도 안나왔던 상태였다.
					//
					// 아래처럼 다이얼로그가 사용하는 ui텍스처 크기를 가지고 uv float변환을 해야한다.
					EtTextureHandle hTexture = m_pParent->GetUITexture();
					if( m_bExistTemplateTexture )
						hTexture = m_Template.m_hTemplateTexture;
					if( hTexture )
					{
						float fProgressSize = (uvCoord.fHeight - (10.0f/hTexture->Height())) * (1.0f - m_fItemCoolTime);
						uvCoord.fY += (uvCoord.fHeight - (5.0f/hTexture->Height()) - fProgressSize);
						uvCoord.fHeight = fProgressSize;
						m_pParent->DrawSprite( hTexture, uvCoord, 0x80800000, uiCoord );
					}
				}

				pElement = GetElement(typeIconBeam);
				if( pElement )
				{
#ifdef PRE_ADD_LEVELUP_REWARD_NOTIFIER
					D3DXCOLOR colorMouseEnter( /*pElement->TextureColor.dwColor[UI_STATE_MOUSEENTER]*/pElement->TextureColor.dwCurrentColor );
					colorMouseEnter.a = m_nBlinkAlpha / 255.0f;
					pElement->TextureColor.dwCurrentColor = colorMouseEnter;
#endif 	

					SUICoord beamCoord;
					beamCoord.SetSize( pElement->fTextureWidth, pElement->fTextureHeight );
					beamCoord.fX = uiCoord.fX + ((uiCoord.fWidth - beamCoord.fWidth) / 2.0f);
					beamCoord.fY = uiCoord.fY - (beamCoord.fHeight / 2.0f) + 0.0013f;

					if( m_bExistTemplateTexture )
						m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pElement->TemplateUVCoord, pElement->TextureColor.dwCurrentColor, beamCoord );
					else
						m_pParent->DrawSprite( pElement->UVCoord, pElement->TextureColor.dwCurrentColor, beamCoord );
				}

#ifdef PRE_ADD_SKILLCOOLTIME
				if (CGameOption::GetInstance().bSkillCoolTimeCounter)
				{

					if  (m_pItem->GetType() == MIInventoryItem::Skill)
					{
						CDnSkill* pSkill = static_cast<CDnSkill*>(m_pItem);
						if (pSkill)
						{
							float fMaxCoolTime = pSkill->GetDelayTime();
							float fPresentCoolTime = pSkill->GetElapsedDelayTime() + 1;

							RenderCoolTimeCounter(fElapsedTime, fMaxCoolTime, fPresentCoolTime);
						}
					}
					else if (m_pItem->GetType() == MIInventoryItem::Item)
					{
						CDnItem* pItem = static_cast<CDnItem*>(m_pItem);
						if (pItem)
						{
							float fMaxCoolTime = pItem->GetDelayTime();
							float fPresentCoolTime = pItem->GetElapsedDelayTime() + 1;

							RenderCoolTimeCounter(fElapsedTime, fMaxCoolTime, fPresentCoolTime);
						}
					}
				}
#endif
			}

			pElement = GetElement(m_nTypeOutline);

			// 교환창에 올라간 아이템 혹은 쿨타임중인 아이템은 테두리 색도 비활성화시켜 눈에 잘 띄도록 한다.
			if( pElement )
			{
#ifdef PRE_ADD_LEVELUP_REWARD_NOTIFIER
				D3DXCOLOR colorMouseEnter( /*pElement->TextureColor.dwColor[UI_STATE_MOUSEENTER]*/pElement->TextureColor.dwCurrentColor );
				colorMouseEnter.a = m_nBlinkAlpha / 255.0f;
				pElement->TextureColor.dwCurrentColor = colorMouseEnter;
#endif 	
				if( m_bExistTemplateTexture )
					m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pElement->TemplateUVCoord, bShadow ? 0xff808080 : pElement->TextureColor.dwCurrentColor, m_Property.UICoord );
				else
					m_pParent->DrawSprite( pElement->UVCoord, bShadow ? 0xff808080 : pElement->TextureColor.dwCurrentColor, m_Property.UICoord );
			}

			if( m_fCompleteCoolTime > 0 )
			{
				m_pParent->DrawSprite( m_hCurIconLight, m_IconUVCoord, m_CompleteCoolTimeColor, m_Property.UICoord );
			}

			// 내구도 아이콘 표시
			{
				// 다른 클래스와 달리 QuickSlotButton은 스킬도 포함한다. 그래서 Item인지 Skill인지도 검사해봐야한다.
				if( m_pItem->GetType() == MIInventoryItem::Item )
				{
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
#ifdef PRE_ADD_LEVELUP_REWARD_NOTIFIER
								D3DXCOLOR colorMouseEnter( /*pElement->TextureColor.dwColor[UI_STATE_MOUSEENTER]*/pElement->TextureColor.dwCurrentColor );
								colorMouseEnter.a = m_nBlinkAlpha / 255.0f;
								pElement->TextureColor.dwCurrentColor = colorMouseEnter;
#endif 	

								if( m_bExistTemplateTexture )
									m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pElement->TemplateUVCoord, pElement->TextureColor.dwCurrentColor, m_Property.UICoord );
								else
									m_pParent->DrawSprite( pElement->UVCoord, pElement->TextureColor.dwCurrentColor, m_Property.UICoord );
							}
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
#ifdef PRE_ADD_LEVELUP_REWARD_NOTIFIER
									D3DXCOLOR colorMouseEnter( /*pElement->TextureColor.dwColor[UI_STATE_MOUSEENTER]*/pElement->TextureColor.dwCurrentColor );
									colorMouseEnter.a = m_nBlinkAlpha / 255.0f;
									pElement->TextureColor.dwCurrentColor = colorMouseEnter;
#endif 	
									if( m_bExistTemplateTexture )
										m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pElement->TemplateUVCoord, pElement->TextureColor.dwCurrentColor, m_Property.UICoord );
									else
										m_pParent->DrawSprite( pElement->UVCoord, pElement->TextureColor.dwCurrentColor, m_Property.UICoord );
								}
							}
						}
					}

					if (pItem->GetReversion() != CDnItem::NoReversion && pItem->IsSoulbBound() == false && pItem->GetSealID() > 0 )
					{
						pElement = GetElement(typeItemButton_Seal);
						if (pElement)
						{
#ifdef PRE_ADD_LEVELUP_REWARD_NOTIFIER
							D3DXCOLOR colorMouseEnter( /*pElement->TextureColor.dwColor[UI_STATE_MOUSEENTER]*/pElement->TextureColor.dwCurrentColor );
							colorMouseEnter.a = m_nBlinkAlpha / 255.0f;
							pElement->TextureColor.dwCurrentColor = colorMouseEnter;
#endif 	
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
#ifdef PRE_ADD_LEVELUP_REWARD_NOTIFIER
							D3DXCOLOR colorMouseEnter( /*pElement->TextureColor.dwColor[UI_STATE_MOUSEENTER]*/pElement->TextureColor.dwCurrentColor );
							colorMouseEnter.a = m_nBlinkAlpha / 255.0f;
							pElement->TextureColor.dwCurrentColor = colorMouseEnter;
#endif 	
							if( m_bExistTemplateTexture )
								m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pElement->TemplateUVCoord, pElement->TextureColor.dwCurrentColor, m_Property.UICoord );
							else
								m_pParent->DrawSprite( pElement->UVCoord, pElement->TextureColor.dwCurrentColor, m_Property.UICoord );
						}
					}
				}
			}

			RenderCount( GetElement(typeItemCount) );
			
			if (bToggle) {
				RenderToggle(fElapsedTime);
			} else {
				m_bToggleBegin = true;
			}

			//rlkt_test
			if (GetSlotType() == ST_QUICKSLOT )
			{
				if (m_pItem->GetType() == MIInventoryItem::Skill)
				{
					CDnSkill *psItem = (CDnSkill*)m_pItem;
					if (psItem->IsAcquiredByBlow())
					{
						SUIElement *pElement = GetElement(typeActiveSkill);
						if (pElement)
						{
#ifdef PRE_ADD_LEVELUP_REWARD_NOTIFIER
							D3DXCOLOR colorMouseEnter( /*pElement->TextureColor.dwColor[UI_STATE_MOUSEENTER]*/pElement->TextureColor.dwCurrentColor);
							colorMouseEnter.a = m_nBlinkAlpha / 255.0f;
							pElement->TextureColor.dwCurrentColor = colorMouseEnter;
#endif 	
							if (m_bExistTemplateTexture)
								m_pParent->DrawSprite(m_Template.m_hTemplateTexture, pElement->TemplateUVCoord, pElement->TextureColor.dwCurrentColor, m_Property.UICoord);
							else
								m_pParent->DrawSprite(pElement->UVCoord, pElement->TextureColor.dwCurrentColor, m_Property.UICoord);
						}
					}
				}
			}

			if( GetSlotType() == ST_INVENTORY || GetSlotType() == ST_INVENTORY_CASH ||  GetSlotType() == ST_INVENTORY_VEHICLE )
			{
				if( m_pItem->GetType() == MIInventoryItem::Item )
				{
					CDnItem *pItem = (CDnItem*)m_pItem;
					
					if( pItem->IsNewGain())
					{
						SUIElement *pElement = GetElement(typeItemNew);
						if( pElement )
						{
#ifdef PRE_ADD_LEVELUP_REWARD_NOTIFIER
							D3DXCOLOR colorMouseEnter( /*pElement->TextureColor.dwColor[UI_STATE_MOUSEENTER]*/pElement->TextureColor.dwCurrentColor );
							colorMouseEnter.a = m_nBlinkAlpha / 255.0f;
							pElement->TextureColor.dwCurrentColor = colorMouseEnter;
#endif 	
							if( m_bExistTemplateTexture )
								m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pElement->TemplateUVCoord, pElement->TextureColor.dwCurrentColor, m_Property.UICoord );
							else
								m_pParent->DrawSprite( pElement->UVCoord, pElement->TextureColor.dwCurrentColor, m_Property.UICoord );
						}
					}
				}
			}
		}

#ifdef PRE_ADD_CASHREMOVE
		if( ST_INVENTORY_CASHREMOVE == m_SlotType )
			RenderExpire( GetElement(typeItemCount) );
#endif
	}
#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
	else
	{
		if (GetSlotType() == ST_TOTAL_LEVEL_SKILL)
		{
			SUIElement *pElement(NULL);
			DWORD dwColor = m_dwIconColor;

			if( m_bForceUsable )
			{
				// 테두리도 정해진 색깔 그대로,,
				pElement = GetElement(typeSkillOutline);
				if( pElement )
				{
#ifdef PRE_ADD_LEVELUP_REWARD_NOTIFIER
					D3DXCOLOR colorMouseEnter( /*pElement->TextureColor.dwColor[UI_STATE_MOUSEENTER]*/pElement->TextureColor.dwCurrentColor );
					colorMouseEnter.a = m_nBlinkAlpha / 255.0f;
					pElement->TextureColor.dwCurrentColor = colorMouseEnter;
#endif 	
					if( m_bExistTemplateTexture )
						m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pElement->TemplateUVCoord, pElement->TextureColor.dwCurrentColor, m_Property.UICoord );
					else
						m_pParent->DrawSprite( pElement->UVCoord, pElement->TextureColor.dwCurrentColor, m_Property.UICoord );
				}
			}
			else
			if( m_bForceUnUsable )
			{
				// 테두리도 회색으로..
				pElement = GetElement(typeSkillOutlineDisabled);
				if( pElement )
				{
#ifdef PRE_ADD_LEVELUP_REWARD_NOTIFIER
					D3DXCOLOR colorMouseEnter( /*pElement->TextureColor.dwColor[UI_STATE_MOUSEENTER]*/pElement->TextureColor.dwCurrentColor );
					colorMouseEnter.a = m_nBlinkAlpha / 255.0f;
					pElement->TextureColor.dwCurrentColor = colorMouseEnter;
#endif 	
					if( m_bExistTemplateTexture )
						m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pElement->TemplateUVCoord, pElement->TextureColor.dwCurrentColor, m_Property.UICoord );
					else
						m_pParent->DrawSprite( pElement->UVCoord, pElement->TextureColor.dwCurrentColor, m_Property.UICoord );
				}
			}
		}
	}
#endif // PRE_ADD_TOTAL_LEVEL_SKILL

	if( GetItemType() == MIInventoryItem::Item )
	{
		if( m_ButtonState == ITEMSLOT_DISABLE )
		{
			SUIElement *pElement = GetElement(typeItemDisable);
			if( pElement )
			{
#ifdef PRE_ADD_LEVELUP_REWARD_NOTIFIER
				D3DXCOLOR colorMouseEnter( /*pElement->TextureColor.dwColor[UI_STATE_MOUSEENTER]*/pElement->TextureColor.dwCurrentColor );
				colorMouseEnter.a = m_nBlinkAlpha / 255.0f;
				pElement->TextureColor.dwCurrentColor = colorMouseEnter;
#endif 	
				if( m_bExistTemplateTexture )
					m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pElement->TemplateUVCoord, pElement->TextureColor.dwCurrentColor, m_Property.UICoord );
				else
					m_pParent->DrawSprite( pElement->UVCoord, pElement->TextureColor.dwCurrentColor, m_Property.UICoord );
				return;
			}
		}
	}

#ifdef _QUICKSLOT_USE_EFFECT
	if( GetSlotType() == ST_QUICKSLOT && m_fSizeRate != 1.0f )
	{
		SUICoord uiCoord;
		uiCoord.fWidth = (float)m_hQuickSlotUse->Width() / DEFAULT_UI_SCREEN_WIDTH;
		uiCoord.fHeight = (float)m_hQuickSlotUse->Height() / DEFAULT_UI_SCREEN_HEIGHT;
		uiCoord.fWidth *= m_fSizeRate;
		uiCoord.fHeight *= m_fSizeRate;
		uiCoord.fX = m_uiDefaultCoord.fX + ((m_uiDefaultCoord.fWidth - uiCoord.fWidth) / 2.0f);
		uiCoord.fY = m_uiDefaultCoord.fY + ((m_uiDefaultCoord.fHeight - uiCoord.fHeight) / 2.0f);
		EtColor Color(0xffffffff);
		Color.a = m_fSizeAlpha;
		m_pParent->DrawSprite( m_hQuickSlotUse, m_IconUVCoord, Color, uiCoord );
	}

	if( GetSlotType() == ST_QUICKSLOT && m_fSizeRate == 1.0f )
#else
	if( GetSlotType() == ST_QUICKSLOT )
#endif
	{
		if( 0 < m_Property.nHotKey )
			m_pParent->DrawSprite( m_hShortCutIcon, m_ShortCutUV, m_dwIconColor, m_ShortCutCoord );
	}

	if( GetSlotType() == ST_LIFESKILL_QUICKSLOT_EX )
	{
		if( 0 < m_Property.nHotKey )
			m_pParent->DrawSprite( m_hShortCutIcon, m_ShortCutUV, m_dwIconColor, m_ShortCutCoord );
	}
}

void CDnQuickSlotButton::RenderDrag( float fElapsedTime )
{
	if (IsDragMode() == false)
		return;
	if( !m_hCurIcon ) return;

	POINT CursorPoint;
	GetCursorPos( &CursorPoint );
	ScreenToClient( GetEtDevice()->GetHWnd(), &CursorPoint );
	m_CursorCoord = m_Property.UICoord;
	m_CursorCoord.fWidth /= m_pParent->GetScreenWidthRatio();
	m_CursorCoord.fHeight /= m_pParent->GetScreenHeightRatio();
	m_CursorCoord.fX = CursorPoint.x / ( float )GetEtDevice()->Width() - m_CursorCoord.fWidth * 0.5f;
	m_CursorCoord.fY = CursorPoint.y / ( float )GetEtDevice()->Height() - m_CursorCoord.fHeight * 0.5f;
	
	CEtSprite::GetInstance().DrawSprite( ( EtTexture * )m_hCurIcon->GetTexturePtr(), 
										m_hCurIcon->Width(), 
										m_hCurIcon->Height(), 
										m_IconUV, 
										0xffffffff, 
										m_CursorCoord, 
										0.0f );

	EtTextureHandle hTexture;
	hTexture = m_pParent->GetUITexture();
	if( m_bExistTemplateTexture )
		hTexture = m_Template.m_hTemplateTexture;
	if( !hTexture ) return;

	SUIElement *pElement;
	pElement = GetElement(m_nTypeOutline);

	if( pElement )
	{
		SUICoord UVCoord = pElement->UVCoord;
		if( m_bExistTemplateTexture )
			UVCoord = pElement->TemplateUVCoord;
		CEtSprite::GetInstance().DrawSprite( ( EtTexture * )hTexture->GetTexturePtr(), 
											hTexture->Width(), 
											hTexture->Height(), 
											UVCoord, 
											pElement->TextureColor.dwCurrentColor, 
											m_CursorCoord, 
											0.0f );
	}

	RenderDragCount( GetElement(typeItemCount), m_CursorCoord );
}

void CDnQuickSlotButton::RenderToggle( float fElapsedTime )
{
	m_uiControlState1 = (UI_CONTROL_STATE)m_pElementToggle1->TextureColor.GetCurrentState();
	m_uiControlState2 = (UI_CONTROL_STATE)m_pElementToggle2->TextureColor.GetCurrentState();

	m_fToggleTime += fElapsedTime;

	if( m_fToggleTime > TOGGLE_BLEND_TIME )
	{
		if( m_uiControlState1 == UI_STATE_NORMAL )
		{
			m_uiControlState1 = UI_STATE_HIDDEN;
			m_uiControlState2 = UI_STATE_NORMAL;
		}
		else
		{
			m_uiControlState1 = UI_STATE_NORMAL;
			m_uiControlState2 = UI_STATE_HIDDEN;
		}

		m_fToggleTime = 0.0f;
	}

	m_pElementToggle1->TextureColor.BlendEx( m_uiControlState1, fElapsedTime, TOGGLE_BLEND_TIME );
	if( m_bExistTemplateTexture )
		m_pParent->DrawSprite( m_Template.m_hTemplateTexture, m_pElementToggle1->TemplateUVCoord, m_pElementToggle1->TextureColor.dwCurrentColor, m_Property.UICoord );
	else
		m_pParent->DrawSprite( m_pElementToggle1->UVCoord, m_pElementToggle1->TextureColor.dwCurrentColor, m_Property.UICoord );

	m_pElementToggle2->TextureColor.BlendEx( m_uiControlState2, fElapsedTime, TOGGLE_BLEND_TIME );
	if( m_bExistTemplateTexture )
		m_pParent->DrawSprite( m_Template.m_hTemplateTexture, m_pElementToggle2->TemplateUVCoord, m_pElementToggle2->TextureColor.dwCurrentColor, m_Property.UICoord );
	else
		m_pParent->DrawSprite( m_pElementToggle2->UVCoord, m_pElementToggle2->TextureColor.dwCurrentColor, m_Property.UICoord );

}

//bool CDnQuickSlotButton::HandleMouse( UINT uMsg, float fX, float fY, WPARAM wParam, LPARAM lParam )
//{
//	// Note : 퀵슬롯은 툴팁을 출력하지 않기 위해
//	//		버튼 HandleMouse()함수를 호출한다.
//	return CEtUIButton::HandleMouse( uMsg, fX, fY, wParam, lParam );
//}

void CDnQuickSlotButton::SetCompleteCoolTime( float fTime )
{
	m_fConst = fTime;
	m_fCompleteCoolTime = fTime;
	m_CompleteCoolTimeColor = EtColor((DWORD)0xffffffff);
}

#ifdef _QUICKSLOT_USE_EFFECT
void CDnQuickSlotButton::SetStartCoolTime( float fReadyTime, float fTime )
{
	m_fSizeReadyTime = fReadyTime;
	m_fSizeConst = fTime;
	m_fSizeTime = fTime;
	m_fSizeRate = 1.75f;
	m_fSizeAlpha = 1.0f;
	m_pParent->MoveToTail( this );
}
#endif
