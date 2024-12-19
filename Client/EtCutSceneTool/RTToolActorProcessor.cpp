#include "stdafx.h"
#include "RTToolActorProcessor.h"
#include "RTCutSceneRenderer.h"
#include "DnCutSceneActor.h"
#include "DnCutSceneActProp.h"
#include "EtWorldSector.h"
#include "Fade.h"
#include <wx/wx.h>
#include "ToolData.h"




CRTToolActorProcessor::CRTToolActorProcessor(void) : m_pFade( NULL )
{

}

CRTToolActorProcessor::~CRTToolActorProcessor(void)
{
}



void CRTToolActorProcessor::AddObjectToPlay( S_CS_ACTOR_INFO* pNewObject )
{
	ICustomActorProcessor::AddObjectToPlay( pNewObject );

	m_vlPrevFrame.push_back( 0.0f );
	m_vlPrevActionIndex.push_back( -1 );
}


void CRTToolActorProcessor::AddPropToPlay( S_CS_PROP_INFO* pPropInfo )
{
	ICustomActorProcessor::AddPropToPlay( pPropInfo );
	
	CEtWorldSector* pSector = m_pWorld->GetSector( 0.0f, 0.0f );
	CEtWorldProp* pProp = pSector->GetPropFromCreateUniqueID( pPropInfo->iPropID );

	_ASSERT( pProp );
	if( pProp )
		m_vlpProps.push_back( pPropInfo );
}


void CRTToolActorProcessor::ShowSubtitle( const wchar_t* pSubtitle )
{
	TOOL_DATA.GetRenderer()->ShowSubtitle( pSubtitle );
}



void CRTToolActorProcessor::HideSubtitle( void )
{
	TOOL_DATA.GetRenderer()->HideSubtitle();
}



void CRTToolActorProcessor::SetPropAction( CEtWorldProp* pProp, const char* pActionName, LOCAL_TIME AbsoluteTime, float fFrame )
{
	CDnCutSceneActProp* pActProp = static_cast<CDnCutSceneActProp*>(pProp);
	pActProp->SetAction( pActionName, 0.0f, 0.0f );
}

void CRTToolActorProcessor::ShowProp( CEtWorldProp* pProp, bool bShow )
{
	CDnCutSceneWorldProp* pWorldProp = static_cast<CDnCutSceneWorldProp*>(pProp);
	pWorldProp->Show( bShow );
}


void CRTToolActorProcessor::CalcAniDistance( S_CS_ACTOR_INFO* pActorInfo, float fCurFrame, float fPrevFrame, EtVector3& DistVec )
{
	if( NULL == pActorInfo )
		return;

	if( pActorInfo->strNowActionName.empty() )
		return;

	pActorInfo->hObject->CalcAniDistance( pActorInfo->vlAniIndex.at(pActorInfo->iNowAniIndex),
										  fCurFrame, fPrevFrame, DistVec );
}



int CRTToolActorProcessor::GetActionIndex( int iActorIndex, const char* pActionName )
{
	if( (int)m_vlpActors.size() < iActorIndex )
		return -1;

	if( 0 == strlen(pActionName) )
		return -1;

	S_CS_ACTOR_INFO* pActorInfo = m_vlpActors.at( iActorIndex );
	return pActorInfo->iNowAniIndex;
}


float CRTToolActorProcessor::GetActorScale( int iActorIndex )
{
	if( (int)m_vlpActors.size() < iActorIndex )
		return 1.0f;

	S_CS_ACTOR_INFO* pActorInfo = m_vlpActors.at( iActorIndex );
	CDnCutSceneActor* pDnActor = static_cast<CDnCutSceneActor*>(pActorInfo->pActor);

	return pDnActor->GetScale();
}


void CRTToolActorProcessor::Process( LOCAL_TIME LocalTime, float fDelta )
{
	int iNumActor = (int)m_vlpActors.size();
	
	for( int iActor = 0; iActor < iNumActor; ++iActor )
	{
		S_CS_ACTOR_INFO* pActorInfo = GetActorInfoByIndex( iActor );

		if( pActorInfo->bHided )
		{
			//OutputDebug( "[CutScene] %s Hided\n", pActorInfo->strActorName.c_str() );
			continue;
		}

		//int iAniIndex = pDnActor->GetElementIndex( pActorInfo->strNowActionName.c_str() );
		if( pActorInfo->strNowActionName.empty() )
		{
			//OutputDebug( "[CutScene] %s iAniIndex == -1\n", pActorInfo->strActorName.c_str() );
			// 액션이 없는 경우엔 포지션만 셋팅한다.
			pActorInfo->hObject->Update( pActorInfo->matExFinal );
			continue;
		}


		CDnCutSceneActor* pDnCutSceneActor = static_cast<CDnCutSceneActor*>(pActorInfo->pActor);
		MatrixEx matExFinalPos = pActorInfo->matExFinal;
		
		EtVector3 vAniDistance;
		pActorInfo->hObject->CalcAniDistance( pActorInfo->vlAniIndex[pActorInfo->iNowAniIndex], 
											  pActorInfo->fFrame, 0.0f, vAniDistance );
		matExFinalPos.MoveLocalYAxis( -vAniDistance.y );

		pDnCutSceneActor->SetMatrixEx( matExFinalPos );

		if( pActorInfo->iNowAniIndex != m_vlPrevActionIndex.at(iActor) )
			pDnCutSceneActor->OnChangeAction();

		pDnCutSceneActor->ProcessSignal( pDnCutSceneActor->GetElement(pActorInfo->iNowAniIndex), LocalTime,
										 pActorInfo->fFrame, 
										 m_vlPrevFrame.at(iActor) );

		pActorInfo->hObject->SetAniFrame( pActorInfo->vlAniIndex[pActorInfo->iNowAniIndex], pActorInfo->fFrame );
		pActorInfo->hObject->Update( pActorInfo->matExFinal );

		pDnCutSceneActor->UpdateSignal( LocalTime, fDelta );

		m_vlPrevFrame[ iActor ] = pActorInfo->fFrame;
		m_vlPrevActionIndex[ iActor ] = pActorInfo->iNowAniIndex;
	}
}




void CRTToolActorProcessor::FadeIn( float fSpeed, DWORD dwColor )
{
	m_pFade->FadeIn( fSpeed, dwColor );
}



void CRTToolActorProcessor::FadeOut( float fSpeed, DWORD dwColor )
{
	m_pFade->FadeOut( fSpeed, dwColor );
}


// #54681 - 플레이어캐릭터의 Hide처리.
void CRTToolActorProcessor::ShowEffect( void * pActor, bool bShow )
{
	CDnCutSceneActor * pCSActor = static_cast< CDnCutSceneActor * >( pActor );
	pCSActor->Show( bShow );
}
