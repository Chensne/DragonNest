#include "StdAfx.h"
#include "DnHeadEffectRender.h"
#include "DnTableDB.h"
#include "DnActor.h"
#include "DnInCodeResource.h"

#define HEADEFFECT_ETCOFFSET	-200
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#ifdef  PRE_MOD_CHRAMBOX_ROTATE
#include "DnCamera.h"
#endif 


CDnHeadEffectRender::CDnHeadEffectRender()
{
	int aiPriority[HeadEffect_Amount] = {
		0,	// 말풍선
		0,	// 라디오메세지
		1,	// 강화이펙트
		1,	// 랜덤아이템
		1,	// 문장보옥
		1,	// 아이템 조합
#ifdef PRE_ADD_SECONDARY_SKILL
		1,
		1,
		1,
		1,
		1,
		1,
#endif // PRE_ADD_SECONDARY_SKILL
	};
	memcpy( m_aiEffectPriority, aiPriority, sizeof(m_aiEffectPriority) );
#ifdef PRE_MOD_HEAD_EFFECT_HEIGHT
	m_fAdjustHeight = 0.0f;
#endif
}

CDnHeadEffectRender::~CDnHeadEffectRender()
{
	// 재생중이다가 갑자기 액터가 소멸되면 반복재생 이펙트는 안사라지고 남을 수 있다.
	if( m_hHeadEffectActor )
	{
		for( int i = 0; i < CDnInCodeResource::EtcIndex_Amount; ++i )
		{
			eHeadEffectType eType = ToHeadEffectType((CDnInCodeResource::eHeadEffectEtcIndex)i);
			m_hHeadEffectActor->TSmartPtrSignalImp<DnEtcHandle, EtcObjectSignalStruct>::RemoveSignalHandle( HEADEFFECT_ETCOFFSET, eType );
		}
	}
}

void CDnHeadEffectRender::Initialize( DnActorHandle hActor )
{
	m_hHeadEffectActor = hActor;
}

void CDnHeadEffectRender::Process( LOCAL_TIME LocalTime, float fDelta )
{
	if( !m_hHeadEffectActor ) return;

	// 아래 Insert할때 SetParentActor를 해주면, 알아서 Process가 호출되서
	// EtcObject의 파티클은 알파가 부모액터를 따라 변경된다. 그러나 메시알파는 변하지 않는 구조라 이렇게 직접 변경해준다.
	for( int i = 0; i < CDnInCodeResource::EtcIndex_Amount; ++i )
	{
		eHeadEffectType eType = ToHeadEffectType((CDnInCodeResource::eHeadEffectEtcIndex)i);
		if( m_hHeadEffectActor->TSmartPtrSignalImp<DnEtcHandle, EtcObjectSignalStruct>::IsExistSignalHandle( HEADEFFECT_ETCOFFSET, eType ) )
		{
			DnEtcHandle hHandle = m_hHeadEffectActor->TSmartPtrSignalImp<DnEtcHandle, EtcObjectSignalStruct>::GetSignalHandle( HEADEFFECT_ETCOFFSET, eType );
			if( hHandle ) {
				float fAlpha = m_hHeadEffectActor->GetAniObjectHandle()->GetObjectAlpha();
				if( hHandle->GetObjectHandle() )
					hHandle->GetObjectHandle()->SetObjectAlpha( fAlpha );	// 메시오브젝트의 알파 변경.
			}
		}
	}
}

CDnInCodeResource::eHeadEffectEtcIndex CDnHeadEffectRender::ToEtcIndex( eHeadEffectType eType )
{
	switch( eType )
	{
	case HeadEffect_Enchant:	return CDnInCodeResource::EtcIndex_Enchant;
	case HeadEffect_RandomItem:	return CDnInCodeResource::EtcIndex_RandomItem;
	case HeadEffect_Plate:		return CDnInCodeResource::EtcIndex_Plate;
	case HeadEffect_Compound:	return CDnInCodeResource::EtcIndex_Compound;
#ifdef PRE_ADD_SECONDARY_SKILL
	case HeadEffect_Plant:			return CDnInCodeResource::EtcIndex_Plant;
	case HeadEffect_Water:			return CDnInCodeResource::EtcIndex_Water;
	case HeadEffect_Harvest:		return CDnInCodeResource::EtcIndex_Harvest;
	case HeadEffect_Cooking_Making:	return CDnInCodeResource::EtcIndex_Cooking_Making;
	case HeadEffect_Cooking_Finish:	return CDnInCodeResource::EtcIndex_Cooking_Finish;
	case HeadEffect_Fishing:		return CDnInCodeResource::EtcIndex_Fishing;
#endif // PRE_ADD_SECONDARY_SKILL
	default:
		_ASSERT(0&&"CDnHeadEffectRender::ToEtcIndex 유효하지 않은 인자 들어왔습니다.");
	}
	return CDnInCodeResource::EtcIndex_Enchant;
}

CDnHeadEffectRender::eHeadEffectType CDnHeadEffectRender::ToHeadEffectType( CDnInCodeResource::eHeadEffectEtcIndex eType )
{
	switch( eType )
	{
	case CDnInCodeResource::EtcIndex_Enchant:		return HeadEffect_Enchant;
	case CDnInCodeResource::EtcIndex_RandomItem:	return HeadEffect_RandomItem;
	case CDnInCodeResource::EtcIndex_Plate:			return HeadEffect_Plate;
	case CDnInCodeResource::EtcIndex_Compound:		return HeadEffect_Compound;
#ifdef PRE_ADD_SECONDARY_SKILL
	case CDnInCodeResource::EtcIndex_Plant:				return HeadEffect_Plant;
	case CDnInCodeResource::EtcIndex_Water:				return HeadEffect_Water;
	case CDnInCodeResource::EtcIndex_Harvest:			return HeadEffect_Harvest;
	case CDnInCodeResource::EtcIndex_Cooking_Making:	return HeadEffect_Cooking_Making;
	case CDnInCodeResource::EtcIndex_Cooking_Finish:	return HeadEffect_Cooking_Finish;
	case CDnInCodeResource::EtcIndex_Fishing:			return HeadEffect_Fishing;
#endif // PRE_ADD_SECONDARY_SKILL
	}
	return HeadEffect_Enchant;
}

CDnHeadEffectRender::eHeadEffectType CDnHeadEffectRender::IsEffectPlaying()
{
	if( m_ChatBalloon.IsRenderChatBalloon() )
		return HeadEffect_ChatBalloon;

	if( m_HeadIcon.IsRenderHeadIcon() )
		return HeadEffect_RadioMsg;

	for( int i = 0; i < CDnInCodeResource::EtcIndex_Amount; ++i ) {
		eHeadEffectType eType = ToHeadEffectType((CDnInCodeResource::eHeadEffectEtcIndex)i);
		if( m_hHeadEffectActor->TSmartPtrSignalImp<DnEtcHandle, EtcObjectSignalStruct>::IsExistSignalHandle( HEADEFFECT_ETCOFFSET, eType ) )
		{
			DnEtcHandle hHandle = m_hHeadEffectActor->TSmartPtrSignalImp<DnEtcHandle, EtcObjectSignalStruct>::GetSignalHandle( HEADEFFECT_ETCOFFSET, eType );
			if( hHandle ) {
				std::string szCurrentActionName = hHandle->GetCurrentAction();
				if( !szCurrentActionName.empty() )
					return eType;
			}
			else {
				// 여기 들어오는 경우는 강화성공,실패 같은 루프없는 이펙트가 진행되고나서
				// 디스트로이 시그널에 의해 DnEtcHandle은 파괴되었지만, 시그널 insert해둔것만 남아있는 상태다.
				// Etc핸들 없이 시그널만 들어있을 이유는 없기때문에, 여기서 삭제해주도록 하겠다.
				// 루프되는 Making 같은 경우엔 위에서 IsPlaying으로 체크되어 Stop루틴을 따르기때문에 이쪽으로 안들어온다.
				m_hHeadEffectActor->TSmartPtrSignalImp<DnEtcHandle, EtcObjectSignalStruct>::RemoveSignalHandle( HEADEFFECT_ETCOFFSET, eType );
			}
		}
	}

	return HeadEffect_None;
}

void CDnHeadEffectRender::StopHeadEffect( eHeadEffectType eType )
{
	// 플레이 중인 이펙트를 중지한다.
	switch( eType )
	{
	case HeadEffect_ChatBalloon:	m_ChatBalloon.StopRenderChatBalloon();	break;
	case HeadEffect_RadioMsg:		m_HeadIcon.StopRenderHeadIcon();		break;
	case HeadEffect_Enchant:
	case HeadEffect_RandomItem:
	case HeadEffect_Plate:
	case HeadEffect_Compound:
#ifdef PRE_ADD_SECONDARY_SKILL
	case HeadEffect_Plant:
	case HeadEffect_Water:
	case HeadEffect_Harvest:
	case HeadEffect_Cooking_Making:
	case HeadEffect_Cooking_Finish:
	case HeadEffect_Fishing:
#endif // PRE_ADD_SECONDARY_SKILL
		m_hHeadEffectActor->TSmartPtrSignalImp<DnEtcHandle, EtcObjectSignalStruct>::RemoveSignalHandle( HEADEFFECT_ETCOFFSET, eType );
		break;
	}
}

bool CDnHeadEffectRender::CheckPlayable( eHeadEffectType eType )
{
	eHeadEffectType eCurType = IsEffectPlaying();
	if( eCurType != HeadEffect_None ) {
		if( m_aiEffectPriority[eCurType] > m_aiEffectPriority[eType] )
			return false;
		StopHeadEffect( eCurType );
	}
	return true;
}

void CDnHeadEffectRender::SetChatBalloonText( LPCWSTR wszMessage, DWORD dwTalkTime, int nChatType, bool nCount )
{
	if( !CheckPlayable(HeadEffect_ChatBalloon) )
		return;

	m_ChatBalloon.SetChatBalloonText( wszMessage, dwTalkTime, nChatType );
	
	if( nCount )
		m_ChatBalloon.IncreaseBalloon();
}

void CDnHeadEffectRender::SetHeadIcon( EtTextureHandle hTexture, DWORD dwStartTime, DWORD dwRenderTime, bool bReleaseTexture )
{
	if( !CheckPlayable(HeadEffect_RadioMsg) )
		return;

	m_HeadIcon.SetHeadIcon( hTexture, dwStartTime, dwRenderTime, bReleaseTexture );
}

void CDnHeadEffectRender::SetHeadEffect( eBroadcastingEffect eType, eBroadcastingEffect eState )
{
	// 서버로부터 온 타입을 클라에 맞게 매칭
	eHeadEffectType eEffectType = HeadEffect_None;
	switch(eType)
	{
	case EffectType_Enchant:	eEffectType = HeadEffect_Enchant;		break;
	case EffectType_Compound:	eEffectType = HeadEffect_Compound;		break;
	case EffectType_Emblem:		eEffectType = HeadEffect_Plate;			break;
	case EffectType_Random:		eEffectType = HeadEffect_RandomItem;	break;
#ifdef PRE_ADD_SECONDARY_SKILL
	case EffectType_Plant:				eEffectType = HeadEffect_Plant;			break;
	case EffectType_Water:				eEffectType = HeadEffect_Water;			break;
	case EffectType_Harvest:			eEffectType = HeadEffect_Harvest;		break;
	case EffectType_Cooking_Making:		eEffectType = HeadEffect_Cooking_Making;	break;
	case EffectType_Cooking_Finish:		eEffectType = HeadEffect_Cooking_Finish;	break;
	case EffectType_Fishing:			eEffectType = HeadEffect_Fishing;		break;
#endif // PRE_ADD_SECONDARY_SKILL
	}

	if( eEffectType == HeadEffect_None )
	{
		_ASSERT(0&&"브로드캐스트 이펙트의 타입값이 유효하지 않습니다.");
		return;
	}

	if( !CheckPlayable(eEffectType) )
		return;

	if( eState == EffectState_Cancel )
		return;

	DnEtcHandle hHandle = CDnInCodeResource::GetInstance().CreateHeadEffect( ToEtcIndex(eEffectType) );
	if( hHandle ) {
		hHandle->SetPosition( *m_hHeadEffectActor->GetPosition() );
		hHandle->SetParentActor( m_hHeadEffectActor );

		// 액션인덱스에 헤드이펙트 디파인오프셋 넣고, 시그널 인덱스에 이펙트타입 넣어서 관리한다.
		EtcObjectSignalStruct *pResult = m_hHeadEffectActor->TSmartPtrSignalImp<DnEtcHandle, EtcObjectSignalStruct>::InsertSignalHandle( HEADEFFECT_ETCOFFSET, eEffectType, hHandle );
		pResult->bLinkObject = true;
#ifdef PRE_MOD_HEAD_EFFECT_HEIGHT
		pResult->vOffset = EtVector3( 0.f, m_fAdjustHeight, 0.f );
#else
		pResult->vOffset = EtVector3( 0.f, 0.f, 0.f );
#endif
#ifdef PRE_MOD_CHRAMBOX_ROTATE // 항상 케릭터가 바라보는 방향으로 상자가 입을 열수 있도록. Degree : 180 고정으로 두었습니다. 
		if( eEffectType == HeadEffect_RandomItem ) {
			pResult->vRotate = EtVector3( 0.f, 180, 0.f );
		}
		else
			pResult->vRotate = EtVector3( 0.f, 0.f, 0.f );
#else
		pResult->vRotate = EtVector3( 0.f, 0.f, 0.f );
#endif 
		pResult->bDefendenceParent = false;
		pResult->szBoneName[0] = 0;

		// 상태에 맞는 적절한 액션을 실행한다.
		CEtActionBase::ActionElementStruct *pStruct = NULL;
		switch( eState )
		{
		case EffectState_Start:
			hHandle->SetActionQueue( "Making" );
			break;
		case EffectState_Success:
			// Ready애니때문에 사운드가 결과보다 늦게 나와 별로라고 한다. 어쩔 수 없이 Ready애니 생략하고 바로 보여주기로 한다.
			//hHandle->SetActionQueue( "Ready_Success" );
			hHandle->SetActionQueue( "Success" );
			break;
		case EffectState_Fail:
			//hHandle->SetActionQueue( "Ready_Fail" );
			hHandle->SetActionQueue( "Fail" );
			break;
		case EffectState_SoftFail:
			//hHandle->SetActionQueue( "Ready_DownGrade" );
			hHandle->SetActionQueue( "DownGrade" );
			break;
		case EffectState_Open:
			hHandle->SetActionQueue( "Ready" );
			break;
		case EffectState_VeryGood:
			hHandle->SetActionQueue( "Open_VeryGood" );
			break;
		case EffectState_Good:
			hHandle->SetActionQueue( "Open_Good" );
			break;
		case EffectState_Normal:
			hHandle->SetActionQueue( "Open_Normal" );
			break;
		case EffectState_Bad:
			hHandle->SetActionQueue( "Open_Bad" );
			break;
		}
	}
}