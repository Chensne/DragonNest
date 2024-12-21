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
	// #60295 - Clone���� ������ �÷��̾���� ����.
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

	// ���� ������ 0���� �ٲ���� ����� 0 �����Ӻ��� ���۵Ǵ� 1������¥�� �ñ׳��� ����.
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
		pActProp->ProcessAction( AbsoluteTime, fFrame);		// ù�������� �ñ׳��� ���õǴ°� ���� ���� ProcessAction �� �ѹ� �����ش�.
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
	// Ŭ�󿡼��� Speed �� �ƴ϶� FadeIn �� �ð���..

	GetInterface().FadeDialog( dwColor, 0x00000000, 1.0f/fSpeed, NULL, true );
}



void CDnCutSceneActorProcessor::FadeOut( float fSpeed, DWORD dwColor )
{
	dwColor |= 0xff000000;
	//GetInterface().FadeDialog( 0x00000000, 0xff000000, /*CGlobalInfo::GetInstance().m_fFadeDelta*/ fSpeed );
	// Ŭ�󿡼��� Speed �� �ƴ϶� FadeOut �� �ð���..

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

		// ����� ���� ���͸� ������ ���� ���.
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

			// �׼��� ���� ��쿣 �����Ǹ� �����Ѵ�.
			pActorInfo->hObject->SetCalcPositionFlag( 0 );
			pActorInfo->hObject->Update( pActorInfo->matExFinal );
			continue;
		}

		// �׼��� �ٲ� ���� �� �� ����, ���� frame ��� 0, 0 ���� �־���� ù��° �ñ׳��� ����.. 
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

		// Animation Distance ���� Y ���� matExFinalPos �� ������ ���־�� �Ѵ�?
		MatrixEx matExFinal = pActorInfo->matExFinal;
		EtVector3 vAniDistance;
		pActorInfo->hObject->CalcAniDistance( pActorInfo->vlAniIndex[iAniIndex], pActorInfo->fFrame, 0.0f, vAniDistance );
		matExFinal.MoveLocalYAxis( -vAniDistance.y );

		// �Ʒ� �Լ����� ���������� GetMatEx �Լ��� �Ѿ���� EtMatrixEx �� ����ϱ� ������ ���������� ������Ʈ ����
		MatrixEx* pMatExCross = pDnActor->GetMatEx();
		*pMatExCross = matExFinal;
		//*pCross = pActorInfo->matExFinal; 

		// ����׿�
		//OutputDebug( "[CutScene] %s Position : %2.2f %2.2f %2.2f \n", pActorInfo->strActorName.c_str(), pDnActor->GetPosition()->x, 
		//																							    pDnActor->GetPosition()->y, 
		//																							    pDnActor->GetPosition()->z );

		//CEtActionBase::ActionElementStruct* pStruct = pDnActor->GetElement( pActorInfo->iNowAniIndex );

		// �ƽſ����� �׼� ť�� ���� �ʴ´�.
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
		
		// ������ ��� NpcActor�� ������ �Ǿ AniDistance�� �ߺ� ���Ǵ� ���� �� �����..
		// �ܺο��� �ٸ��� ������ �Ǹ� �߸��� ����� �����Ƿ� ������ X,Z  Position �ִ� ����� ����. by realgaia
		//pActorInfo->hObject->SetCalcPositionFlag( 0 );
		pActorInfo->hObject->Update( pActorInfo->matExFinal );

		// �ñ׳� ó���� ����.
		//pDnActor->CDnActionBase::ProcessAction( LocalTime, fDelta );
//		pDnActor->CDnActionBase::ProcessAction( LocalTime, fDelta );
		pDnActor->MAFaceAniBase::Process( LocalTime, fDelta );
		pDnActor->CDnActionSignalImp::Process( LocalTime, fDelta );
		CDnWeapon::ProcessClass( LocalTime, fDelta );

		m_vlPrevFrame[iActor] = pActorInfo->fFrame;
	}
}


// #54681 - �÷��̾�ĳ������ Hideó��.
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
			// ���ﶩ ���⿡�� ���� ����Ʈ�� �������ϹǷ�, PlayerActor�� Show�� ȣ���ؼ� DnWeapon::ShowWeapon �Լ��� ȣ��ǰ� �ϴ°� �°�,
			pPlayerActor->Show( bShow );
		}
		else
		{
			// ���̰� �Ҷ� �ƽ��߿� ������ ���⸦ ����� �Ҷ��� �־ ������ Show���¸� ������Ű�°� �´�.
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
			// �и� ���� �ڵ尡 �������� ���Ѱ� ����ε�,
			// �̹� �̷��� �۾��� �Ǿ��ִ� �ƽŵ��� ���Ƽ�,
			// �׸��� Show�Լ��� ȣ���ؾ߸� �ؼ�, �̷��� �÷��� �ΰ� ó���ϵ��� �Ѵ�.


			// #71465 -���� �ܡ��ҷ��� ����Ų ������ �׷����� �̺�Ʈ ���񿡼� ǥ�õ��� �ʴ´�.			
			// < ������ >
			// �ƾ����� Action Property�� Key Property�� �̿��� ������ ��ġ�� ������ ��쿡
			// ����Ʈ���� ����� ��ġ�� ������ �� �� ���� ��Ȳ�� �߻��ϰԵǾ� ����Ʈ�� ��ġ�� ����� ���ŵ����ʾƼ�
			// ����Ʈ�� ���������̰ų� �������ʴ� ������ �߻��ϴ� ���Դϴ�.
			// ���ʿ� ����������� �κ��̾ �۾��� ������� �ֽ��ϴ�.
			//
			// < �����ذ��� >
			// - ���� �ƾ����� ������ ����Ʈ�� �������ʰ� �����Ƿ�, ��������Ʈ�� ������ ����ó���ϵ��� �մϴ�.
			//
			// < �����ذ��Ѵٸ�...>
			// - �ƾ����� Action Property �� Key Property �� ������ ��ġ�� �����ϴ� ��쿡 ����� ��ġ�� ��ƼŬ����
			// ������ �� �ֵ��� �ؾ���.
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
