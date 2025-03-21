#pragma once
#include "EtUITreeItem.h"

class CEtUIQuestTreeItem : public CTreeItem
{
public:
	CEtUIQuestTreeItem();
	virtual ~CEtUIQuestTreeItem();

protected:
	//float m_fStateSize;
	//float m_fBorderSize;
	EtTextureHandle m_hBackImage;

	bool m_bUpgrade;

public:
	SUICoord m_TextureUV;		// Note : 텍스쳐 UV좌표
	SUICoord m_TextureCoord;	// Note : 텍스쳐 출력될 좌표

	SUICoord m_StateCoord;		// Note : 오른쪽 상태(퀘스트 진행중,완료) 이미지 좌표
	SUICoord m_NotifierCoord;	// Note : 알리미 마크 이미지 좌표
	
	//SUICoord m_FrameCoord[3];
	//SUICoord m_SelFrameCoord[3];

	SUICoord m_FrameCoord;
	SUICoord m_FrameSelectCoord;

	bool	m_bMainQuest;

public:
	void SetTexture( EtTextureHandle hTextureHandle, int nX, int nY, int nWidth, int nHeight );
	EtTextureHandle GetTexture() { return m_hBackImage; }

public:
	virtual void SetUICoord( SUICoord &uiCoord );
	virtual void UpdatePos( float fX, float fY );

public:
	virtual void Initialize( CEtUIControl *pControl, int nDepth = 1 );

	void SetMainQuest( bool bMainQuest ) { m_bMainQuest = bMainQuest ;}
};
