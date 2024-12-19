#include "StdAfx.h"
#include "DnCutSceneActorProcessor.h"
#include "DnActor.h"
#include "DnActionbase.h"
#include "DnInterface.h"
#include "DnWorldActProp.h"
#include "DnMonsterActor.h"

#include "DnPlayerActor.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 


CDnCutSceneActorProcessor::CDnCutSceneActorProcessor(void)
{
}

CDnCutSceneActorProcessor::~CDnCutSceneActorProcessor(void)
{
	// #60295 - Clone으로 생성한 플레이어액터 제거.
    S_CS_ACTOR_INFO * pInfo = NULL;
	int size = (int)m_vlpActors.size();	
	for( int i=0; i<size; ++i )
	{
		pInfo = m_vlpActors[ i ];
		if( pInfo->bPlayer && pInfo->bClone )
			static_cast< CDnActor * >( pInfo->pActor )->Release();
	}

}



void CDnCutSceneActorProcessor::AddObjectToPlay( S_CS_ACTOR_INFO* pNewObject )
{
	ICustomActorProcessor::AddObjectToPlay( pNewObject );

	m_vlPrevFrame.push_back( 0.0f );
	m_vlpPrevActionSeq.push_back( NULL );
}


void CDnCutSceneActorProcessor::OnChangeAction( S_CS_ACTOR_INFO* pActionChangeActor )
{
	//CDnActor* pDnActor = static_cast<CDnActor*>(pActionChangeActor->pActor);
	//pDnActor->SetActionQueue( pActionChangeActor->strNowActionName.c_str(), 0, 0.f );

	// 이전 프레임 0으로 바꿔줘야 제대로 0 프레임부터 시작되는 1프레임짜리 시그널이 잡힘.
	//m_vlPrevFrame.at( pActionChangeActor->iActorIndexInProcessorVector ) = 0.0f;
}


void CDnCutSceneActorProcessor::ShowSubtitle( const wchar_t* pSubtitle )
{
	GetInterface().SetCutSceneCaption( pSubtitle );
}


void CDnCutSceneActorProcessor::HideSubtitle( void )
{
	GetInterface().ClearCutSceneCaption();
}


void CDnCutSceneActorProcessor::SetPropAction( CEtWorldProp* pProp, const char* pActionName, LOCAL_TIME AbsoluteTime, float fFrame  )
{
	CDnWorldActProp* pActProp = dynamic_cast<CDnWorldActProp*>(pProp);

	if( pActProp ) {
		pActProp->SetAction( pActionName, 0.0f, 0.0f );
		pActProp->ProcessAction( AbsoluteTime, fFrame);		// 첫프레임의 시그널이 무시되는걸 막기 위해 ProcessAction 을 한번 돌려준다.
	}
}


void CDnCutSceneActorProcessor::ShowProp( CEtWorldProp* pProp, bool bShow )
{
	CDnWorldProp* pDnWorldProp = dynamic_cast<CDnWorldProp*>( pProp );

	if( pDnWorldProp )
	{
		pDnWorldProp->Show( bShow );
		if( pDnWorldProp->GetObjectHandle() )
			pDnWorldProp->GetObjectHandle()->EnableCull( false );
	}
}



void CDnCutSceneActorProcessor::FadeIn( float fSpeed, DWORD dwColor )
{
	dwColor |= 0xff000000;
	//GetInterface().FadeDialog( 0xff000000, 0x00000000, /*CGlobalInfo::GetInstance().m_fFadeDelta*/ fSpeed );
	// 클라에서는 Speed 가 아니라 FadeIn 총 시간임..

	GetInterface().FadeDialog( dwColor, 0x00000000, 1.0f/fSpeed, NULL, true );
}



void CDnCutSceneActorProcessor::FadeOut( float fSpeed, DWORD dwColor )
{
	dwColor |= 0xff000000;
	//GetInterface().FadeDialog( 0x00000000, 0xff000000, /*CGlobalInfo::GetInstance().m_fFadeDelta*/ fSpeed );
	// 클라에서는 Speed 가 아니라 FadeOut 총 시간임..

	GetInterface().FadeDialog( 0x00000000, dwColor, 1.0f/fSpeed, NULL, true );
}


void CDnCutSceneActorProcessor::CalcAniDistance( S_CS_ACTOR_INFO* pActorInfo, float fCurFrame, float fPrevFrame, EtVector3& DistVec )
{
	if( NULL == pActorInfo )
		return;

	if( pActorInfo->strNowActionName.empty() )
		return;

	CDnActor* pDnActor = static_cast<CDnActor*>(pActorInfo->pActor);
	if (pDnActor == NULL)
		return;

	int iAniIndex = pDnActor->GetElementIndex( pActorInfo->strNowActionName.c_str() );
	if (iAniIndex == -1 || (int)pActorInfo->vlAniIndex.size() <= iAniIndex || !(pActorInfo->hObject))
		return;

	pActorInfo->hObject->CalcAniDistance( pActorInfo->vlAniIndex[iAniIndex], fCurFrame, fPrevFrame, DistVec );
}



int CDnCutSceneActorProcessor::GetActionIndex( int iActorIndex, const char* pActionName )
{
	//if( strcmp( "Event_LookTown", pActionName ) == 0 )
	//{
	//	int i = 0; 
	//	++i;
	//	return ;
	//}

	if( (int)m_vlpActors.size() < iActorIndex )
		return -1;

	if( NULL == pActionName )
		return -1;

	if( 0 == strlen( pActionName ) )
		return -1;

	S_CS_ACTOR_INFO* pActorInfo = m_vlpActors.at( iActorIndex );
	CDnActor* pDnActor = static_cast<CDnActor*>(pActorInfo->pActor);
	int iAniIndex = pDnActor->GetElementIndex( pActionName );
	
	return iAniIndex;
}


float CDnCutSceneActorProcessor::GetActorScale( int iActorIndex )
{
	float fResult = 1.0f;

	if( iActorIndex < (int)m_vlpActors.size() )
	{
		S_CS_ACTOR_INFO* pActorInfo = m_vlpActors.at( iActorIndex );

		// 현재는 몬스터 액터만 스케일 값을 사용.
		CDnMonsterActor* pDnMonsterActor = dynamic_cast<CDnMonsterActor*>((CDnActor*)pActorInfo->pActor);
		if( pDnMonsterActor )
			fResult = pDnMonsterActor->GetScale();
	}

	return fResult;
}


void CDnCutSceneActorProcessor::Process( LOCAL_TIME LocalTime, float fDelta )
{
	int iNumActors = (int)m_vlpActors.size();
	for( int iActor = 0; iActor < iNumActors; ++iActor )
	{
		S_CS_ACTOR_INFO* pActorInfo = m_vlpActors.at( iActor );
		if( pActorInfo->bHided )
		{
			//OutputDebug( "[CutScene] %s Hided\n", pActorInfo->strActorName.c_str() );
			continue;
		}

		CDnActor* pDnActor = static_cast<CDnActor*>(pActorInfo->pActor);

		int iAniIndex = pDnActor->GetElementIndex( pActorInfo->strNowActionName.c_str() );
		if( iAniIndex == -1 )
		{
			//OutputDebug( "[CutScene] %s iAniIndex == -1\n", pActorInfo->strActorName.c_str() );

			// 액션이 없는 경우엔 포지션만 셋팅한다.
			pActorInfo->hObject->SetCalcPositionFlag( 0 );
			pActorInfo->hObject->Update( pActorInfo->matExFinal );
			continue;
		}

		// 액션이 바뀔 때는 둘 다 현재, 이전 frame 모두 0, 0 으로 넣어줘야 첫번째 시그널이 잡힘.. 
		float fSignalCurFrame = pActorInfo->fFrame;
		if( (NULL != pActorInfo->pNowActionSequence) && (m_vlpPrevActionSeq.at( iActor ) != pActorInfo->pNowActionSequence) )
		{
			//if( pActorInfo->strNowActionName == "PullOut_Weapon" )
			//{
			//	int i = 0;
			//	++i;
			//}

			m_vlPrevFrame.at( iActor ) = 0.0;
			fSignalCurFrame = 0.0f;
			pActorInfo->fFrame = 0.0f;
		}

		m_vlpPrevActionSeq.at( iActor ) = pActorInfo->pNowActionSequence;

		// Animation Distance 에서 Y 값을 matExFinalPos 에 조정을 해주어야 한다?
		MatrixEx matExFinal = pActorInfo->matExFinal;
		EtVector3 vAniDistance;
		pActorInfo->hObject->CalcAniDistance( pActorInfo->vlAniIndex[iAniIndex], pActorInfo->fFrame, 0.0f, vAniDistance );
		matExFinal.MoveLocalYAxis( -vAniDistance.y );

		// 아래 함수들은 내부적으로 GetMatEx 함수로 넘어오는 EtMatrixEx 를 사용하기 때문에 마찬가지로 업데이트 해줌
		MatrixEx* pMatExCross = pDnActor->GetMatEx();
		*pMatExCross = matExFinal;
		//*pCross = pActorInfo->matExFinal; 

		// 디버그용
		//OutputDebug( "[CutScene] %s Position : %2.2f %2.2f %2.2f \n", pActorInfo->strActorName.c_str(), pDnActor->GetPosition()->x, 
		//																							    pDnActor->GetPosition()->y, 
		//																							    pDnActor->GetPosition()->z );

		//CEtActionBase::ActionElementStruct* pStruct = pDnActor->GetElement( pActorInfo->iNowAniIndex );

		// 컷신에서는 액션 큐를 쓰지 않는다.
		pDnActor->ClearActionQueue();
		CEtActionBase::ActionElementStruct* pStruct = pDnActor->GetElement( iAniIndex );
		pDnActor->SetActionIndex( iAniIndex );
		pDnActor->CDnActionBase::ProcessSignal( pStruct, fSignalCurFrame, m_vlPrevFrame.at(iActor) );
		for( int i=0; i < 2; i++ ) 
		{
			if( pDnActor->GetActiveWeapon( i ) ) {
				pDnActor->GetActiveWeapon( i )->Process( LocalTime, fDelta );
			}
		}
				//pActorInfo->hObject->SetAniFrame( pActorInfo->vlAniIndex[pActorInfo->iNowAniIndex], pActorInfo->fFrame );
		pActorInfo->hObject->SetAniFrame( pActorInfo->vlAniIndex[iAniIndex], pActorInfo->fFrame );
		
		// 로제의 경우 NpcActor로 생성이 되어서 AniDistance가 중복 계산되는 버그 가 생겼다..
		// 외부에서 다르게 세팅이 되면 잘못된 결과가 나오므로 강제로 X,Z  Position 애니 계산을 끈다. by realgaia
		//pActorInfo->hObject->SetCalcPositionFlag( 0 );
		pActorInfo->hObject->Update( pActorInfo->matExFinal );

		// 시그널 처리를 위해.
		//pDnActor->CDnActionBase::ProcessAction( LocalTime, fDelta );
//		pDnActor->CDnActionBase::ProcessAction( LocalTime, fDelta );
		pDnActor->MAFaceAniBase::Process( LocalTime, fDelta );
		pDnActor->CDnActionSignalImp::Process( LocalTime, fDelta );
		CDnWeapon::ProcessClass( LocalTime, fDelta );

		m_vlPrevFrame[iActor] = pActorInfo->fFrame;
	}
}


// #54681 - 플레이어캐릭터의 Hide처리.
#include "DnPlayerActor.h"
void CDnCutSceneActorProcessor::ShowEffect( void * pActor, bool bShow )
{
	CDnActor * pAc = static_cast< CDnActor * >( pActor );
	if( pAc->IsShow() == bShow ) return;
	CDnPlayerActor * pPlayerActor = dynamic_cast< CDnPlayerActor * >( pAc );
	if( pPlayerActor )
	{
		if( bShow == false )
		{
			// 지울땐 무기에서 만든 이펙트도 지워야하므로, PlayerActor의 Show를 호출해서 DnWeapon::ShowWeapon 함수가 호출되게 하는게 맞고,
			pPlayerActor->Show( bShow );
		}
		else
		{
			// 보이게 할땐 컷신중에 강제로 무기를 숨기게 할때가 있어서 무기의 Show상태를 유지시키는게 맞다.
			pPlayerActor->Show( bShow );

			if( pPlayerActor->IsHideWeaponBySignalInCutScene( 0 ) )
			{
				pPlayerActor->CDnPlayerActor::ShowCashWeapon( 0, false );
				pPlayerActor->CDnActor::ShowWeapon( 0, false );
			}

			if( pPlayerActor->IsHideWeaponBySignalInCutScene( 1 ) )
			{
				pPlayerActor->CDnPlayerActor::ShowCashWeapon( 1, false );
				pPlayerActor->CDnActor::ShowWeapon( 1, false );
			}
			// 분명 위의 코드가 적절하지 못한건 사실인데,
			// 이미 이렇게 작업이 되어있는 컷신들이 많아서,
			// 그리고 Show함수를 호출해야만 해서, 이렇게 플래그 두고 처리하도록 한다.


			// #71465 -【그 외】할로윈 펌프킨 정령의 그래픽이 이벤트 무비에서 표시되지 않는다.			
			// < 문제점 >
			// 컷씬에서 Action Property와 Key Property를 이용해 액터의 위치를 변경한 경우에
			// 이펙트에서 변경된 위치를 가져올 수 가 없는 상황이 발생하게되어 이펙트의 위치가 제대로 갱신되지않아서
			// 이펙트가 떨어져보이거나 보이지않는 문제가 발생하는 것입니다.
			// 에초에 고려되지않은 부분이어서 작업에 어려움이 있습니다.
			//
			// < 현재해결방법 >
			// - 현재 컷씬에서 파츠의 이펙트는 나오지않고 있으므로, 정령이펙트만 강제로 숨김처리하도록 합니다.
			//
			// < 차후해결한다면...>
			// - 컷씬에서 Action Property 와 Key Property 로 엑터의 위치를 변경하는 경우에 변경된 위치를 파티클에서
			// 가져올 수 있도록 해야함.
			DnPartsHandle hParts = pPlayerActor->GetCashParts( CDnParts::PartsTypeEnum::CashFairy );
			if( hParts )
				hParts->ShowRenderBase( false );
		}
	}
	else
		pAc->Show( bShow );
	
//	CDnPlayerActor * pAC = static_cast< CDnPlayerActor * >( pActor );
//	pAC->Show( bShow );	
}
