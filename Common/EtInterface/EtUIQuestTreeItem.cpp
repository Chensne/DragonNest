#include "StdAfx.h"
#include "EtUIQuestTreeItem.h"
#include "EtResourceMng.h"
#include "EtLoader.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CEtUIQuestTreeItem::CEtUIQuestTreeItem()
{
	m_bMainQuest = false;
	m_bUpgrade = true;
}

CEtUIQuestTreeItem::~CEtUIQuestTreeItem(void)
{
	SAFE_RELEASE_SPTR( m_hBackImage );
}

void CEtUIQuestTreeItem::Initialize( CEtUIControl *pControl, int nDepth )
{
	ASSERT( pControl&&"CDnQuestTreeItem::Initialize, pControl is NULL!" );

	SUIControlProperty *pProperty = pControl->GetProperty();
	if( !pProperty ) return;

	m_pParentControl = pControl;

	m_nTreeDepth = nDepth;
	m_fLineSpace = pProperty->QuestTreeControlProperty.fLineSpace;
	m_fIndentSize = pProperty->TreeControlProperty.fIndentSize*(nDepth-1);

	SUIElement *pElement(NULL);
	pElement = pControl->GetElement(0);
	m_ButtonCoord.fWidth = pElement->fTextureWidth;
	m_ButtonCoord.fHeight = pElement->fTextureHeight;

	// Note : Frame Coord
	//
	pElement = pControl->GetElement(nDepth+1);
	m_FrameCoord.fWidth = pElement->fTextureWidth;
	m_FrameCoord.fHeight = pElement->fTextureHeight;

	// Note : Frame Select Coord
	//
	pElement = pControl->GetElement(nDepth+4);
	m_FrameSelectCoord.fWidth = pElement->fTextureWidth;
	m_FrameSelectCoord.fHeight = pElement->fTextureHeight;

	pElement = pControl->GetElement(8);
	m_StateCoord.fWidth = pElement->fTextureWidth;
	m_StateCoord.fHeight = pElement->fTextureHeight;

	pElement = pControl->GetElement(10);
	if ( pElement )
	{
		m_NotifierCoord.fWidth = pElement->fTextureWidth;
		m_NotifierCoord.fHeight = pElement->fTextureHeight;
	}


	if( GetDepth() == 1 )
	{
		DWORD dwType = GetItemType();
		dwType |= typeMain;
		SetItemType( dwType );
	}
}

void CEtUIQuestTreeItem::SetUICoord( SUICoord &uiCoord )
{
	m_uiCoord = uiCoord;

	if( GetDepth() == 1 )
	{
		m_uiCoord.fHeight += m_fLineSpace;
	}

	m_strCoord.SetSize( m_uiCoord.fWidth-m_ButtonCoord.fWidth-m_StateCoord.fWidth, m_FrameCoord.fHeight );

	m_strBackCoord.SetSize( m_uiCoord.fWidth-m_ButtonCoord.fWidth-m_StateCoord.fWidth, m_FrameCoord.fHeight );
}

void CEtUIQuestTreeItem::UpdatePos( float fX, float fY )
{
	if( m_bUpgrade )
	{
		m_uiCoord.SetPosition( fX + m_fIndentSize, fY );

		float fValue = ((m_uiCoord.fHeight-m_ButtonCoord.fHeight)/2.0f);
		m_ButtonCoord.fX = m_uiCoord.fX+0.01f;
		m_ButtonCoord.fY = m_uiCoord.fY+fValue+m_fLineSpace;

		m_NotifierCoord.fX = m_ButtonCoord.fX + m_ButtonCoord.fWidth + 0.001f;

		m_strCoord.SetPosition( m_NotifierCoord.fX + m_NotifierCoord.fWidth, fY + m_fLineSpace );
		m_strBackCoord.SetPosition( m_NotifierCoord.fX + m_uiCoord.fWidth - 0.1f, fY + m_fLineSpace );

		m_StateCoord.fX = m_uiCoord.Right()-m_StateCoord.fWidth-0.004f;
		m_StateCoord.fY = m_uiCoord.fY+((m_uiCoord.fHeight-m_StateCoord.fHeight)/2.0f)+m_fLineSpace;


		static float fAdd = -0.003f;
		if( m_bMainQuest  ) {
			fAdd = -0.001f;
		}

		m_NotifierCoord.fY = m_uiCoord.fY+((m_uiCoord.fHeight-m_NotifierCoord.fHeight)/2.0f)+m_fLineSpace + fAdd;
	}
	else
	{
		m_uiCoord.SetPosition( fX + m_fIndentSize, fY );

		float fValue = ((m_uiCoord.fHeight-m_ButtonCoord.fHeight)/2.0f);
		m_ButtonCoord.fX = m_uiCoord.fX+0.01f;
		m_ButtonCoord.fY = m_uiCoord.fY+fValue+m_fLineSpace;

		m_strCoord.SetPosition( m_uiCoord.fX + m_ButtonCoord.fWidth + 0.014f, fY + m_fLineSpace );

		m_StateCoord.fX = m_uiCoord.Right()-m_StateCoord.fWidth-0.004f;
		m_StateCoord.fY = m_uiCoord.fY+((m_uiCoord.fHeight-m_StateCoord.fHeight)/2.0f)+m_fLineSpace;

		m_NotifierCoord.fX = m_uiCoord.Right()-m_StateCoord.fWidth-0.008f-m_NotifierCoord.fWidth;


		static float fAdd = -0.003f;
		if( m_bMainQuest  ) {
			fAdd = -0.001f;
		}

		m_NotifierCoord.fY = m_uiCoord.fY+((m_uiCoord.fHeight-m_NotifierCoord.fHeight)/2.0f)+m_fLineSpace + fAdd;
	}

	m_FrameCoord.SetPosition( m_uiCoord.fX, m_uiCoord.fY+m_fLineSpace );
	m_FrameSelectCoord.SetPosition( m_uiCoord.fX, m_uiCoord.fY+m_fLineSpace );
}

void CEtUIQuestTreeItem::SetTexture( EtTextureHandle hTextureHandle, int nX, int nY, int nWidth, int nHeight )
{
	if ( !hTextureHandle )
		return;

	ASSERT( hTextureHandle&&"CTreeItem::SetTexture, hTextureHandle is NULL!" );
	m_hBackImage = hTextureHandle;
	m_TextureCoord = m_uiCoord;

	m_TextureUV.SetCoord(nX / ( float )hTextureHandle->Width(), nY / ( float )hTextureHandle->Height(), nWidth / ( float )hTextureHandle->Width(), nHeight / ( float )hTextureHandle->Height());
}