#pragma once

#include "DnActionSignalImp.h"
#include "DnInCodeResource.h"
#include "MAChatBalloon.h"
#include "DnHeadIcon.h"

class CDnHeadEffectRender
{
public:
	// �Ӹ� ���� �ѷ��� ����ƮEnum�� �߰��ȴٸ� �Ʒ� Ÿ�� ��ȯ�ϴ� ���� ���� �����ؾ��մϴ�.
	// npc reaction ��ü���� �����ؾ� �ϴ� ��찡 �־ ����ü ���� public ���� ���ϴ�. - �ѱ�.
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

	// Balloon�� Ŭ������ Ŀ���� ���� �� �� �ְ� ����.
	MAChatBalloon &GetChatBalloon() { return m_ChatBalloon; }

#ifdef PRE_MOD_HEAD_EFFECT_HEIGHT
	void SetHeadEffectAdjustHeight( float fAdjustHeight ) { m_fAdjustHeight = fAdjustHeight; }
#endif

protected:
	CDnInCodeResource::eHeadEffectEtcIndex ToEtcIndex( eHeadEffectType eType );
	eHeadEffectType ToHeadEffectType( CDnInCodeResource::eHeadEffectEtcIndex eType );

	// ���� �÷������� �ͺ��� �켱������ ���ƾ߸� ������Ű�� ����ȴ�.
	int m_aiEffectPriority[HeadEffect_Amount];

	eHeadEffectType IsEffectPlaying();
	bool CheckPlayable( eHeadEffectType eType );

	// �������� ���� �� �ִ� �Ϳ� �Ѱ谡 �־ have����� ����.
	MAChatBalloon m_ChatBalloon;
	CDnHeadIcon m_HeadIcon;
	DnActorHandle m_hHeadEffectActor;

#ifdef PRE_MOD_HEAD_EFFECT_HEIGHT
	float m_fAdjustHeight;
#endif
};