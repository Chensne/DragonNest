#pragma once

#include "DnActionSignalImp.h"
#include "DnInCodeResource.h"
#include "MAChatBalloon.h"
#include "DnHeadIcon.h"

class CDnHeadEffectRender
{
public:
	// 머리 위에 뿌려줄 이펙트Enum이 추가된다면 아래 타입 변환하는 곳도 같이 변경해야합니다.
	// npc reaction 객체에서 제어해야 하는 경우가 있어서 열거체 선언 public 으로 뺍니다. - 한기.
	enum eHeadEffectType
	{
		HeadEffect_None = -1,
		HeadEffect_ChatBalloon,
		HeadEffect_RadioMsg,
		HeadEffect_Enchant,
		HeadEffect_RandomItem,
		HeadEffect_Plate,
		HeadEffect_Compound,

#ifdef PRE_ADD_SECONDARY_SKILL
		HeadEffect_Plant,
		HeadEffect_Water,
		HeadEffect_Harvest,
		HeadEffect_Cooking_Making,
		HeadEffect_Cooking_Finish,
		HeadEffect_Fishing,
#endif // PRE_ADD_SECONDARY_SKILL

#ifdef PRE_ADD_SECONDARY_SKILL
		HeadEffect_Amount = 12,
#else // PRE_ADD_SECONDARY_SKILL
		HeadEffect_Amount = 6,
#endif // PRE_ADD_SECONDARY_SKILL
	};

	CDnHeadEffectRender();
	virtual ~CDnHeadEffectRender();

	void Initialize( DnActorHandle hActor );
	void Process( LOCAL_TIME LocalTime, float fDelta );

	void SetChatBalloonText( LPCWSTR wszMessage, DWORD dwTalkTime, int nChatType, bool nCount = false );
	void SetHeadIcon( EtTextureHandle hTexture, DWORD dwStartTime, DWORD dwRenderTime = 0, bool bReleaseTexture = false );
	void SetHeadEffect( eBroadcastingEffect eType, eBroadcastingEffect eState );

	void StopHeadEffect( eHeadEffectType eType );

	// Balloon은 클래스가 커져서 얻어다 쓸 수 있게 변경.
	MAChatBalloon &GetChatBalloon() { return m_ChatBalloon; }

#ifdef PRE_MOD_HEAD_EFFECT_HEIGHT
	void SetHeadEffectAdjustHeight( float fAdjustHeight ) { m_fAdjustHeight = fAdjustHeight; }
#endif

protected:
	CDnInCodeResource::eHeadEffectEtcIndex ToEtcIndex( eHeadEffectType eType );
	eHeadEffectType ToHeadEffectType( CDnInCodeResource::eHeadEffectEtcIndex eType );

	// 현재 플레이중인 것보다 우선순위가 높아야만 중지시키고 재생된다.
	int m_aiEffectPriority[HeadEffect_Amount];

	eHeadEffectType IsEffectPlaying();
	bool CheckPlayable( eHeadEffectType eType );

	// 공통으로 묶을 수 있는 것에 한계가 있어서 have관계로 간다.
	MAChatBalloon m_ChatBalloon;
	CDnHeadIcon m_HeadIcon;
	DnActorHandle m_hHeadEffectActor;

#ifdef PRE_MOD_HEAD_EFFECT_HEIGHT
	float m_fAdjustHeight;
#endif
};