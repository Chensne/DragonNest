#include "StdAfx.h"
#include "DnRenderAvatar.h"
#include "DnActorClassDefine.h"
#include "DnPlayerActor.h"
#include "DnItemTask.h"
#include "DnCharStatusDlg.h"
#include "DnTableDB.h"
#include "TaskManager.h"
#include "DnLoadingTask.h"
#include "DnInterface.h"
#include "DnPetActor.h"
#ifdef PRE_FIX_MEMOPT_EXT
#include "DnCommonUtil.h"
#endif


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

std::vector<CDnRenderAvatarNew::sTempTargetInfo> CDnRenderAvatarNew::s_vecTempTarget;
int CDnRenderAvatarNew::s_nTempTargetRefCount = 0;
CEtColorAdjustTexFilter *CDnRenderAvatarNew::s_pColorAdjFilter = NULL;
EtLightHandle CDnRenderAvatarNew::s_hTempLight[2];
EtTextureHandle CDnRenderAvatarNew::s_hTempEnviTexture;
EtColor CDnRenderAvatarNew::s_TempAmbient = 0xFFFFFFFF;
EtColor CDnRenderAvatarNew::s_TempLightDiffuse[2] = { 0xFFFFFFFF, 0xFFFFFFFF };
EtVector3 CDnRenderAvatarNew::s_TempLightDir[2] = { EtVector3( -0.58f, -0.58f, -0.58f ), EtVector3( 0.0f, -1.0f, 0.0f ) };
bool CDnRenderAvatarNew::s_bInitializeStaticObject = false;

CDnRenderAvatarNew::CDnRenderAvatarNew()
{
	m_bUseColorAdjFilter = false;
	m_bUseBorderAlpha = false;
	m_fDefaultCameraYPos = 56.0f;
	m_fDefaultCameraZPos = 150.0f;
	m_eActorType = ActorEnum::None;
	InitCameraControl();

	m_bUseTempLight = false;

	s_nTempTargetRefCount += 1;
	m_nDefaultViewPortWidth = 0;
	m_nDefaultViewPortHeight = 0;
	m_nDirLightCount = 0;
}

CDnRenderAvatarNew::~CDnRenderAvatarNew()
{
	ClearRenderObject();
	SAFE_RELEASE_SPTR( m_hActor );
	SAFE_RELEASE_SPTR( m_hEtc );

	// 대부분이 공유해서 써서 중간에 릴리즈 없이 계속 들고있는 형태로 간다. 만약 중간해제도 필요하면 다른식으로 바꿔야한다.
	s_nTempTargetRefCount -= 1;
	if( s_nTempTargetRefCount == 0 )
	{
		for( int i = 0; i < (int)s_vecTempTarget.size(); ++i )
		{
			SAFE_RELEASE_SPTR( s_vecTempTarget[i].hTexture );
		}
		s_vecTempTarget.clear();
		FinalizeStaticObject();
	}
}

void CDnRenderAvatarNew::InitializeStaticObject()
{
	if( s_bInitializeStaticObject ) return;

	// 현재 월드에 라이트가 없을때 사용할 임시 라이트객체는 미리 생성.
	CEtLight *pLight[2];
	SLightInfo sDefaultLightInfo;
	for( int i = 0; i < 2; ++i )
	{
		pLight[i] = new CEtLight();
		pLight[i]->SetLightInfo( &sDefaultLightInfo );
		s_hTempLight[i] = pLight[i]->GetMySmartPtr();
	}

	// 디자이너분이 알려준 Env파일 로드.
	CEnvInfo EnvInfo;
	bool bResult = EnvInfo.Load( CEtResourceMng::GetInstance().GetFullName( "CashShop01.env" ).c_str() );
	if( bResult )
	{
		// Color Filter
		s_pColorAdjFilter = (CEtColorAdjustTexFilter*)EternityEngine::CreateFilter( SF_COLORADJUSTTEX );
		s_pColorAdjFilter->SetParam( *EnvInfo.GetColorAdjShadow(), *EnvInfo.GetColorAdjMidtones(), *EnvInfo.GetColorAdjHighlights(), EnvInfo.GetColorAdjSaturation() );
		s_pColorAdjFilter->Enable( false );

		// Environment Texture Setting
		if( EnvInfo.GetEnvironmentTexture() && strlen(EnvInfo.GetEnvironmentTexture()) > 0 )
			s_hTempEnviTexture = EternityEngine::LoadTexture( EnvInfo.GetEnvironmentTexture() );

		// Ambient
		EtColor Ambient = EnvInfo.GetAmbient();
		s_TempAmbient = EtColor( Ambient.b, Ambient.g, Ambient.r, 1.0f );

		// Light
		CEnvInfo::LightInfoStruct *pStruct;
		int nDirLightCount = EnvInfo.GetLightCount();
		nDirLightCount = min( nDirLightCount, 2 );
		for( int i = 0; i < nDirLightCount; ++i )
		{
			pStruct = EnvInfo.GetLight(i);
			pStruct->pInfo->Diffuse.r *= pStruct->pInfo->Diffuse.a;
			pStruct->pInfo->Diffuse.g *= pStruct->pInfo->Diffuse.a;
			pStruct->pInfo->Diffuse.b *= pStruct->pInfo->Diffuse.a;
			s_TempLightDiffuse[i] = pStruct->pInfo->Diffuse;
			s_TempLightDir[i] = pStruct->pInfo->Direction;
		}
		EnvInfo.Reset();
	}
	else
	{
		// Color Filter
		s_pColorAdjFilter = (CEtColorAdjustTexFilter*)EternityEngine::CreateFilter( SF_COLORADJUSTTEX );
		s_pColorAdjFilter->SetParam( EtVector4(-0.071f, -0.107f, -0.071f, 0.0f), EtVector4(0.0f, 0.0f, 0.0f, 0.0f), EtVector4(0.821f, 0.393f, 0.464f, 0.0f), 0.9f );
		s_pColorAdjFilter->Enable( false );

		// Ambient
		s_TempAmbient = EtColor( 0.568f, 0.752f, 0.850f, 1.0f );

		// Light
		for( int i = 0; i < 2; ++i )
		{
			s_TempLightDiffuse[i] = ( i == 0 ? EtColor( 1.164f, 1.116f, 0.828f, 1.2f ) : EtColor( 0.28f, 0.44f, 0.81f, 1.0f ) );
			s_TempLightDir[i] = ( i == 0 ? EtVector3( -0.143f, -0.806f, -0.573f ) : EtVector3( -0.607f, -0.716f, 0.343f ) );
		}
	}

	// 환경맵은 설정 안해두곤 적용 안된다고 자꾸 그래서 이렇게 강제로 체크해서 넣는다.
	if( !s_hTempEnviTexture )
		s_hTempEnviTexture = EternityEngine::LoadTexture( "Environment_Texture06.dds" );

	s_bInitializeStaticObject = true;
}

void CDnRenderAvatarNew::FinalizeStaticObject()
{
	if( !s_bInitializeStaticObject ) return;

	SAFE_DELETE( s_pColorAdjFilter );
	SAFE_RELEASE_SPTR( s_hTempLight[0] );
	SAFE_RELEASE_SPTR( s_hTempLight[1] );
	SAFE_RELEASE_SPTR( s_hTempEnviTexture );

	s_bInitializeStaticObject = false;
}

void CDnRenderAvatarNew::Initialize( int nWidth, int nHeight, SCameraInfo &CameraInfo, int nViewPortWidth, int nViewPortHeight, EtFormat Format, bool bUseColorAdjFilter, bool bUseBorderAlpha )
{
	CEtRTTRenderer::Initialize( CameraInfo, nWidth, nHeight, nViewPortWidth, nViewPortHeight, Format );

	// 실제 Initialize 될때 처리하는거로 한다.
	InitializeStaticObject();

	m_bUseColorAdjFilter = bUseColorAdjFilter;
	m_bUseBorderAlpha = bUseBorderAlpha;

	// 디폴트 카메라 설정. z값은 어차피 잘리지만 않게 너무 근접하지만 않으면 되기때문에 고정값 사용하겠다.
	m_hCamera->LookAt( EtVector3(0.0f, m_fDefaultCameraYPos, m_fDefaultCameraZPos), EtVector3(0.0f, m_fDefaultCameraYPos, 0.0f), EtVector3(0.0f, 1.0f, 0.0f) );
	m_hCamera->CalcInvViewMat();

	m_nDefaultViewPortWidth = nViewPortWidth;
	m_nDefaultViewPortHeight = nViewPortHeight;
}

void CDnRenderAvatarNew::CameraLookAt( EtVector3 &Eye, EtVector3 &At, EtVector3 &Up )
{
	m_hCamera->LookAt( Eye, At, Up );
}

bool CDnRenderAvatarNew::SetActor( DnActorHandle hPlayer, bool bShowWeapon, bool bForceCashWeapon )
{
	// 빌리지태스크 OnLoadRecvCharUserEnteredCallback 참고,.(우선 몬스터 제외. 살펴보기랑 캐시샵 쓸거부터 한다.)
	DnActorHandle hActor = CreateActor( hPlayer->GetClassID(), false, false, false );
	if( !hActor ) return false;
	hActor->Show( false );
	//hActor->SetSoftAppear( true );

	hActor->Initialize();

	// AttachItem으로 파츠 장착.
	m_hActor = hActor;
	CheckType();

	if( m_eActorType == ActorEnum::PlayerActor )
	{
		CDnPlayerActor *pPlayer = (CDnPlayerActor *)hPlayer.GetPointer();
		CDnPlayerActor *pActor = (CDnPlayerActor *)hActor.GetPointer();

		// Note: 마을에서 누군가 내 시야에 들어왔을 때 여기서 그 녀석의 직업 히스토리가 셋팅되는 게 아니고 가장 최근 직업만 셋팅됩니다.
		// 추후에 필요하다면 히스토리를 설정해야합니다..
		pActor->SetWeaponViewOrder( 0, ( bForceCashWeapon ) ? true : pPlayer->IsViewWeaponOrder(0) );
		pActor->SetWeaponViewOrder( 1, ( bForceCashWeapon ) ? true : pPlayer->IsViewWeaponOrder(1) );
		for( int i = 0; i < CDnParts::SwapParts_Amount; ++i )
			pActor->SetPartsViewOrder( i, ( bForceCashWeapon ) ? true : pPlayer->IsViewPartsOrder(i) );
		pActor->SetHideHelmet( ( bForceCashWeapon ) ? false : pPlayer->IsHideHelmet() );

		int nDefaultPartsInfo[4];
		for( int i = 0; i < 4; i++ ) {
			nDefaultPartsInfo[ i ] = pPlayer->GetDefaultPartsInfo( (CDnParts::PartsTypeEnum)(CDnParts::Body + i) );
		}
		pActor->SetDefaultPartsInfo( nDefaultPartsInfo );
		pActor->SetPartsColor( MAPartsBody::HairColor, pPlayer->GetPartsColor(MAPartsBody::HairColor) );
		pActor->SetPartsColor( MAPartsBody::EyeColor, pPlayer->GetPartsColor(MAPartsBody::EyeColor) );
		pActor->SetPartsColor( MAPartsBody::SkinColor, pPlayer->GetPartsColor(MAPartsBody::SkinColor) );

		// Normal_Stand 상태의 소서리스로 생성될때 장착한 책이 닫혀있는 채로 등뒤에 있으려면, BattleMode-false해두고 AttachWeapon을 호출해야한다.
		// 이렇게 해야 DnWeapon::ResetDefaultAction 호출되면서,
		// CDnPlayerActor::OnResetAttachWeaponAction 를 호출하는데,
		// 이때 책 액션을 Normal_Stand로 시작하게 해준다.
		// 즉 무기 Attach하기전에 BattleMode부터 설정해두는게 좋다.
		pActor->SetBattleMode( false );

		// 플레이어의 경우엔 먼저 RTTRenderMode 설정을 하도록 한다.(탈것이나 펫은 먼저 할 필요 없다.)
		// 이렇게 해야 칼리 무기 Attach할때 왼손무기를 RenderTarget용으로 설정할 수 있다.
		pActor->SetRTTRenderMode( true, this );

		// 참고로 예전 캐시샵은 누드파츠가 아니라면 디폴트파츠 부르는 형태라서
		// 등급에 따른 외형변화 적용된 노말파츠 모습이 제대로 나오지 않는다.
		for( int i = 0; i < CDnParts::PartsTypeEnum_Amount; i++ )
		{
			DnPartsHandle hParts = pPlayer->GetParts( (CDnParts::PartsTypeEnum)i );
			if( !hParts ) continue;

			if( (CDnParts::PartsTypeEnum)i == CDnParts::Helmet && pActor->IsHideHelmet() )
				continue;
			AttachItem( hParts->GetClassID(), 0, 0, hParts->GetLookItemID() );
		}

		if( bShowWeapon )
		{
			for( int i = 0; i < CDnWeapon::EquipSlot_Amount; i++ )
			{
				DnWeaponHandle hWeapon = pPlayer->GetWeapon( i );
				if( !hWeapon ) continue;

				AttachItem( hWeapon->GetClassID(), hWeapon->GetEnchantLevel() );
			}
		}

		for( int i = 0; i < CDnParts::CashPartsTypeEnum_Amount; i++ )
		{
			DnPartsHandle hParts = pPlayer->GetCashParts( (CDnParts::PartsTypeEnum)i );
			if( !hParts ) continue;

			if( i < CDnParts::SwapParts_Amount && pActor->IsViewPartsOrder(i) == false )
				continue;

			if( (CDnParts::PartsTypeEnum)i == CDnParts::CashHelmet && pActor->IsHideHelmet() )
				continue;

			AttachItem( hParts->GetClassID(), 0, ( (i == CDnParts::CashRing2) ? 1 : 0 ), hParts->GetLookItemID() );
		}

		if( bShowWeapon )
		{
			for( int i = 0; i < CDnWeapon::EquipSlot_Amount; i++ )
			{
				DnWeaponHandle hWeapon = pPlayer->GetCashWeapon( i );
				if( !hWeapon ) continue;

				if( pActor->IsViewWeaponOrder(i) == false )
					continue;

				AttachItem( hWeapon->GetClassID(), hWeapon->GetEnchantLevel(), 0, hWeapon->GetLookItemID() );
			}
		}

#ifdef PRE_MOD_WARRIOR_RIGHTHAND
		// 로그인 캐릭터 선택화면과 같이 CombineParts 를 안하기때문에 워리어 건틀렛꼈을때 오른쪽 장갑메시 하이드 하는 것을 강제로 호출한다.
		pActor->OnCombineParts();
#endif	// #ifdef PRE_MOD_WARRIOR_RIGHTHAND

		//EtVector2 vDir( sin( EtToRadian( pPacket->fRotate ) ), cos( EtToRadian( pPacket->fRotate ) ) );
		EtVector2 vDir( 0.0f, 1.0f );
		pActor->Look( vDir );

		if( CDnActor::s_hLocalActor )
		{
			LOCAL_TIME LocalTime = CDnActor::s_hLocalActor->GetLocalTime();
			EtMatrix WorldMat = *pActor->GetMatEx();
			pActor->MAActorRenderBase::Process( &WorldMat, LocalTime, 0.0f );
		}

		pActor->SetActionQueue( "Normal_Stand" );
		pActor->SetGuildSelfView( pPlayer->GetGuildSelfView() );

		//pActor->CombineParts();		// 렌더아바타에선 컴바인 하지 않는다.
		pActor->SetHP( 1 );		// 몇몇 코드에선 HP 0이하인걸로 직접 판단하는 데가 있어서 패스되지 말라고 강제로 1 넣어준다.
		hActor->Show( true );	// 렌더타겟에 그리기는 해야하기 때문에,

		bool bShowFieldActor = false;
		if( bShowFieldActor )
		{
			// 프로세스를 돌리면, 여러가지 처리(충돌부터 높이처리, 죽은지 판단)등을 하기때문에, 0, 0, 0 에 세워놓고 싶어도 지형위치 위로 올라오게 된다.
			// 아무래도 프로세스 안돌리고 직접 제어하는게 나을듯.

			pActor->SetPosition( *hPlayer->GetPosition() );	// 필드에 띄워서 확인해보기 위해 위치 설정.
			pActor->SetHP( 100 );	// 프로세스 안돌리면 ProcessDie 로 넘어가지 않기 때문에,
			pActor->SetProcess( true );	// 실제 프로세스 돌릴필요 없으니(근데 파티클 보이게 하려면... 돌려야 하지 않나?, 진짜 프로세스는 하는일 많으니 골라서 돌리려구?)
		}

		MAPartsBody *pActorPartsBody = dynamic_cast<MAPartsBody*>(hActor.GetPointer());
		pActorPartsBody->SetIgnoreDeleteWaitOnepiece( true );
		AddObject( pActorPartsBody->GetObjectHandle() );
		//AddObject( pActorPartsBody->GetCombineObject() );	// 컴바인오브젝트엔 알파(머리)와 무기가 빠진다.
		SetRenderFrameCount( -1 );	// -1로 설정해두면 오브젝트 풀릴때까지 계속 렌더링한다.

#ifdef PRE_ADD_CASH_COSTUME_AURA
		pActor->ComputeRTTModeCostumeAura(hPlayer);
#endif
	}
	else if( m_eActorType == ActorEnum::VehicleActor )
	{

		CDnVehicleActor *pVehicleActor = (CDnVehicleActor *)hPlayer.GetPointer();
		CDnVehicleActor *pActor = (CDnVehicleActor *)hActor.GetPointer();

		// 플레이어의 경우엔 파츠로 이루어져있어서 파츠에만 설정했지만, 말의 경우엔 본체부터 다 해야한다.
		if( pActor->GetObjectHandle() ) pActor->GetObjectHandle()->SetRenderType( RT_TARGET );

		// 말정보 얻어와서 셋팅
		for( int i = Vehicle::Slot::Saddle; i < Vehicle::Slot::Max; ++i )
		{
			if( !pVehicleActor->GetMyPlayerActor() || pVehicleActor->GetMyPlayerActor()->IsDestroy() ) continue;
			if( !pVehicleActor->GetMyPlayerActor()->GetVehicleInfo().Vehicle[i].nItemID ) continue;
			pActor->EquipItem( pVehicleActor->GetMyPlayerActor()->GetVehicleInfo().Vehicle[i] );

			DnSimplePartsHandle hParts = pActor->GetVehicleParts( (Vehicle::Parts::eVehicleParts)(i - Vehicle::Slot::Saddle) );
			if( hParts && hParts->GetObjectHandle() ) hParts->GetObjectHandle()->SetRenderType( RT_TARGET );

			if( i == Vehicle::Slot::Hair )	// 갈기의 경우엔 강제로 알파블렌드, 투패스 켠다.
			{
				if( hParts && hParts->GetObjectHandle() && hParts->GetObjectHandle()->GetSkinInstance() )
				{
					hParts->GetObjectHandle()->GetSkinInstance()->EnableAlphaBlend( true );
					hParts->GetObjectHandle()->GetSkinInstance()->SetRenderAlphaTwoPass( true );
				}
			}
		}
		pActor->ChangeHairColor( pVehicleActor->GetMyPlayerActor()->GetVehicleInfo().dwPartsColor1 );

		EtVector2 vDir( 0.0f, 1.0f );
		pActor->Look( vDir );

		// 렌더아바타에서 사용되는 액터는 프로세스리스트에 포함되지 않고 별도로 돌리기때문에, CDnActor::ProcessClass에서도 제외된다.
		// ProcessClass에서 MAActorRenderBase::Process함수가 호출되는데 이것도 제외되는 것.
		// 이것 때문에 애니 프레임 정할때 저장된 LocalTime을 사용하는데, 이 LocalTime이 초기값 상태라,
		// 애니메이션 루프가 한번 이상 돌고나서야 제대로 애니메이션 적용이 되게 된다.
		// 그래서 애니 계산하는 곳에다가 아래처럼 강제로 로컬타임을 주도록 하겠다.
		// 플레이어의 경우엔 Normal_Stand라 거의 티나지 않았던거지 결국은 같은 문제를 가지고 있었다.
		if( CDnActor::s_hLocalActor )
		{
			LOCAL_TIME LocalTime = CDnActor::s_hLocalActor->GetLocalTime();
			EtMatrix WorldMat = *pActor->GetMatEx();
			pActor->MAActorRenderBase::Process( &WorldMat, LocalTime, 0.0f );
			pActor->SetMyPlayerActor(CDnActor::s_hLocalActor);
		}

		pActor->SetRTTRenderMode( true, this );
		pActor->SetActionQueue( "Stand" );
		pActor->SetHP( 1 );		// 몇몇 코드에선 HP 0이하인걸로 직접 판단하는 데가 있어서 패스되지 말라고 강제로 1 넣어준다.
		hActor->Show( true );	// 렌더타겟에 그리기는 해야하기 때문에,

		AddObject( pActor->GetObjectHandle() );
		SetRenderFrameCount( -1 );	// -1로 설정해두면 오브젝트 풀릴때까지 계속 렌더링한다.

	}
	else if( m_eActorType == ActorEnum::PetActor )
	{
		CDnPetActor* pPlayerPetActor = (CDnPetActor *)hPlayer.GetPointer();
		CDnPetActor* pPetActor = (CDnPetActor *)hActor.GetPointer();

		pPetActor->SetDefaultMesh();
		if( pPetActor->GetObjectHandle() ) pPetActor->GetObjectHandle()->SetRenderType( RT_TARGET );

		for( int i=Pet::Slot::Accessory1; i<Pet::Slot::Max; ++i )
		{
			if( !pPlayerPetActor->GetPetInfo().Vehicle[i].nItemID ) continue;
			pPetActor->EquipItem( pPlayerPetActor->GetPetInfo().Vehicle[i] );

			DnSimplePartsHandle hParts = pPetActor->GetPetParts( (Vehicle::Parts::eVehicleParts)(i - Pet::Slot::Accessory1) );
			if( hParts && hParts->GetObjectHandle() ) hParts->GetObjectHandle()->SetRenderType( RT_TARGET );
		}

		pPetActor->ChangeColor( ePetParts::PET_PARTS_BODY, pPlayerPetActor->GetPetInfo().dwPartsColor1 );
		pPetActor->ChangeColor( ePetParts::PET_PARTS_NOSE, pPlayerPetActor->GetPetInfo().dwPartsColor2 );

		EtVector2 vDir( 0.0f, 1.0f );
		pPetActor->Look( vDir );

		LOCAL_TIME LocalTime = CDnActor::s_hLocalActor->GetLocalTime();
		EtMatrix WorldMat = *pPetActor->GetMatEx();
		pPetActor->MAActorRenderBase::Process( &WorldMat, LocalTime, 0.0f );
		pPetActor->SetMyMaster( dynamic_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer()) );

		pPetActor->SetRTTRenderMode( true, this );
		pPetActor->SetActionQueue( "Stand" );
		pPetActor->SetHP( 1 );		// 몇몇 코드에선 HP 0이하인걸로 직접 판단하는 데가 있어서 패스되지 말라고 강제로 1 넣어준다.
		hActor->Show( true );	// 렌더타겟에 그리기는 해야하기 때문에,
		AddObject( pPetActor->GetObjectHandle() );
		SetRenderFrameCount( -1 );	// -1로 설정해두면 오브젝트 풀릴때까지 계속 렌더링한다.
	}

	// 각 파츠의 링크본 처리부분에서 액터의 위치를 구해와 링크된 자식들의 행렬 계산을 하는데,
	// 프로세스를 돌리지 않기 때문에 이렇게 별도로 초기위치를 설정해놓아야한다.
	if( hActor->GetObjectHandle() )
	{
		EtMatrix Mat;
		EtMatrixIdentity( &Mat );
		hActor->GetObjectHandle()->SetWorldMat( Mat );
	}

	return true;
}

bool CDnRenderAvatarNew::SetActor( int nClassID, bool bShowWeapon )
{
	DNTableFileFormat* pActorSox = GetDNTable( CDnTableDB::TACTOR );

	// 빌리지태스크 OnLoadRecvCharUserEnteredCallback 참고,.(우선 몬스터 제외. 살펴보기랑 캐시샵 쓸거부터 한다.)
	DnActorHandle hActor = CreateActor( nClassID, false, false, false );
	if( !hActor ) return false;
	hActor->Show( false );
	//hActor->SetSoftAppear( true );

	hActor->Initialize();

	// DefaultCreate테이블에 있는 각 파츠들 로드하면서 Attach해야하므로, m_hActor에 생성 후 바로 넣어둔다.
	m_hActor = hActor;
	CheckType();

	if( m_eActorType == ActorEnum::PlayerActor )
	{
		// 그냥 hActor->Initialize까지 해두면 플레이어 캐릭터의 경우 머리 없는채로 파츠도 제대로 안붙어서 나온다.
		// 그래서 디폴트파츠 읽어서 따로 설정해줘야한다.
		CDnPlayerActor *pActor = (CDnPlayerActor *)hActor.GetPointer();

		pActor->SetWeaponViewOrder( 0, true );
		pActor->SetWeaponViewOrder( 1, true );
		for( int i = 0; i < CDnParts::SwapParts_Amount; ++i )
			pActor->SetPartsViewOrder( i, true );
		pActor->SetHideHelmet( false );

		char *szLabels[CDnParts::PartsTypeEnum_Amount] = { "_Face", "_Hair", "_Helmet", "_Body", "_Leg", "_Hand", "_Foot", NULL, NULL, NULL, NULL };
		char szBuffer[128] = { 0, };
		DNTableFileFormat*  pDefaultCreateTable = GetDNTable( CDnTableDB::TDEFAULTCREATE );

		int nDefaultPartsInfo[4];
		nDefaultPartsInfo[ CDnParts::Body-CDnParts::DefaultPartsType_Min ] = pDefaultCreateTable->GetFieldFromLablePtr( nClassID, "_DefaultBody" )->GetInteger();
		nDefaultPartsInfo[ CDnParts::Leg-CDnParts::DefaultPartsType_Min ] = pDefaultCreateTable->GetFieldFromLablePtr( nClassID, "_DefaultLeg" )->GetInteger();
		nDefaultPartsInfo[ CDnParts::Hand-CDnParts::DefaultPartsType_Min ] = pDefaultCreateTable->GetFieldFromLablePtr( nClassID, "_DefaultHand" )->GetInteger();
		nDefaultPartsInfo[ CDnParts::Foot-CDnParts::DefaultPartsType_Min ] = pDefaultCreateTable->GetFieldFromLablePtr( nClassID, "_DefaultFoot" )->GetInteger();
		pActor->SetDefaultPartsInfo( nDefaultPartsInfo );

		// 우선 디폴트 파츠 중에 맨 앞에 것만 보여주도록 한다.
		float fIntensity = 1.1f;
		DWORD dwColorR10G10B10 = 0;
		D3DCOLOR dwHairColor = D3DCOLOR_XRGB(
			pDefaultCreateTable->GetFieldFromLablePtr( nClassID, "_HairColor1R" )->GetInteger(),
			pDefaultCreateTable->GetFieldFromLablePtr( nClassID, "_HairColor1G" )->GetInteger(),
			pDefaultCreateTable->GetFieldFromLablePtr( nClassID, "_HairColor1B" )->GetInteger() );
		CDnParts::ConvertD3DCOLORToR10G10B10( &dwColorR10G10B10, dwHairColor, fIntensity );
		pActor->SetPartsColor( MAPartsBody::HairColor, dwColorR10G10B10 );

		fIntensity = 1.0f;
		D3DCOLOR dwEyeColor = D3DCOLOR_XRGB(
			pDefaultCreateTable->GetFieldFromLablePtr( nClassID, "_EyeColor1R" )->GetInteger(),
			pDefaultCreateTable->GetFieldFromLablePtr( nClassID, "_EyeColor1G" )->GetInteger(),
			pDefaultCreateTable->GetFieldFromLablePtr( nClassID, "_EyeColor1B" )->GetInteger() );
		CDnParts::ConvertD3DCOLORToR10G10B10( &dwColorR10G10B10, dwEyeColor, fIntensity );
		pActor->SetPartsColor( MAPartsBody::EyeColor, dwColorR10G10B10 );

		int nA = (int)(pDefaultCreateTable->GetFieldFromLablePtr( nClassID, "_SkinColor1A" )->GetFloat() * 255);
		int nR = (int)(pDefaultCreateTable->GetFieldFromLablePtr( nClassID, "_SkinColor1R" )->GetFloat() * 255);
		int nG = (int)(pDefaultCreateTable->GetFieldFromLablePtr( nClassID, "_SkinColor1G" )->GetFloat() * 255);
		int nB = (int)(pDefaultCreateTable->GetFieldFromLablePtr( nClassID, "_SkinColor1B" )->GetFloat() * 255);
		DWORD dwSkinColor = D3DCOLOR_ARGB( nA, nR, nG, nB );
		pActor->SetPartsColor( MAPartsBody::SkinColor, dwSkinColor );

		pActor->SetBattleMode( false );
		pActor->SetRTTRenderMode( true, this );

		for( int nIndex = 0; nIndex < CDnParts::PartsTypeEnum_Amount; ++nIndex )
		{
			if( NULL == szLabels[nIndex] )
				continue;

			sprintf_s( szBuffer, "%s%d", szLabels[nIndex], 1 );
			int nPartItemID = pDefaultCreateTable->GetFieldFromLablePtr( nClassID, szBuffer )->GetInteger();
			AttachItem( nPartItemID );
		}

		if( bShowWeapon )
		{
			for( int nIndex = 0; nIndex < CDnWeapon::EquipSlot_Amount; ++nIndex )
			{
				sprintf_s( szBuffer, "_Weapon%d", nIndex+1 );
				int nWeaponItemID = pDefaultCreateTable->GetFieldFromLablePtr( nClassID, szBuffer )->GetInteger();
				AttachItem( nWeaponItemID );
			}
		}

#ifdef PRE_MOD_WARRIOR_RIGHTHAND
		// 로그인 캐릭터 선택화면과 같이 CombineParts 를 안하기때문에 워리어 건틀렛꼈을때 오른쪽 장갑메시 하이드 하는 것을 강제로 호출한다.
		pActor->OnCombineParts();
#endif	// #ifdef PRE_MOD_WARRIOR_RIGHTHAND

		EtVector2 vDir( 0.0f, 1.0f );
		pActor->Look( vDir );

		if( CDnActor::s_hLocalActor )
		{
			LOCAL_TIME LocalTime = CDnActor::s_hLocalActor->GetLocalTime();
			EtMatrix WorldMat = *pActor->GetMatEx();
			pActor->MAActorRenderBase::Process( &WorldMat, LocalTime, 0.0f );
		}

		pActor->SetActionQueue( "Normal_Stand" );

		//pActor->CombineParts();		// 렌더아바타에선 컴바인 하지 않는다.
		pActor->SetHP( 1 );		// 몇몇 코드에선 HP 0이하인걸로 직접 판단하는 데가 있어서 패스되지 말라고 강제로 1 넣어준다.
		hActor->Show( true );	// 렌더타겟에 그리기는 해야하기 때문에,

		MAPartsBody *pActorPartsBody = dynamic_cast<MAPartsBody*>(hActor.GetPointer());
		pActorPartsBody->SetIgnoreDeleteWaitOnepiece( true );
		AddObject( pActorPartsBody->GetObjectHandle() );
		//AddObject( pActorPartsBody->GetCombineObject() );	// 컴바인오브젝트엔 알파(머리)와 무기가 빠진다.
		SetRenderFrameCount( -1 );	// -1로 설정해두면 오브젝트 풀릴때까지 계속 렌더링한다.
	}
	else if( m_eActorType == ActorEnum::VehicleActor )
	{

		CDnVehicleActor *pActor = (CDnVehicleActor *)hActor.GetPointer();
		if( pActor->GetObjectHandle() ) pActor->GetObjectHandle()->SetRenderType( RT_TARGET );

		EtVector2 vDir( 0.0f, 1.0f );
		pActor->Look( vDir );

		if( CDnActor::s_hLocalActor )
		{
			LOCAL_TIME LocalTime = CDnActor::s_hLocalActor->GetLocalTime();
			EtMatrix WorldMat = *pActor->GetMatEx();
			pActor->MAActorRenderBase::Process( &WorldMat, LocalTime, 0.0f );
		}

		pActor->SetRTTRenderMode(true,this);
		pActor->SetActionQueue( "Stand" );
		pActor->SetHP( 1 );		// 몇몇 코드에선 HP 0이하인걸로 직접 판단하는 데가 있어서 패스되지 말라고 강제로 1 넣어준다.
		hActor->Show( true );	// 렌더타겟에 그리기는 해야하기 때문에,

		AddObject( pActor->GetObjectHandle() );
		SetRenderFrameCount( -1 );	// -1로 설정해두면 오브젝트 풀릴때까지 계속 렌더링한다.

	}
	else if( m_eActorType == ActorEnum::PetActor )
	{
		CDnPetActor *pPetActor = (CDnPetActor *)hActor.GetPointer();
		pPetActor->SetDefaultMesh();
		if( pPetActor->GetObjectHandle() ) pPetActor->GetObjectHandle()->SetRenderType( RT_TARGET );

		EtVector2 vDir( 0.0f, 1.0f );
		pPetActor->Look( vDir );

		if( CDnActor::s_hLocalActor )
		{
			LOCAL_TIME LocalTime = CDnActor::s_hLocalActor->GetLocalTime();
			EtMatrix WorldMat = *pPetActor->GetMatEx();
			pPetActor->MAActorRenderBase::Process( &WorldMat, LocalTime, 0.0f );
		}

		pPetActor->SetRTTRenderMode( true, this );
		pPetActor->SetActionQueue( "Stand" );
		hActor->Show( true );

		AddObject( pPetActor->GetObjectHandle() );
		SetRenderFrameCount( -1 );	// -1로 설정해두면 오브젝트 풀릴때까지 계속 렌더링한다.
	}

	// 각 파츠의 링크본 처리부분에서 액터의 위치를 구해와 링크된 자식들의 행렬 계산을 하는데,
	// 프로세스를 돌리지 않기 때문에 이렇게 별도로 초기위치를 설정해놓아야한다.
	if( hActor->GetObjectHandle() )
	{
		EtMatrix Mat;
		EtMatrixIdentity( &Mat );
		hActor->GetObjectHandle()->SetWorldMat( Mat );
	}

	return true;
}

void CDnRenderAvatarNew::CheckType()
{
	m_eActorType = ActorEnum::None;

	if( m_hActor )
		CheckActorType();
	else if( m_hEtc )
		CheckEtcType();
}

void CDnRenderAvatarNew::CheckActorType()
{
	CDnPlayerActor *pPlayerActor = dynamic_cast<CDnPlayerActor *>(m_hActor.GetPointer());

	CDnVehicleActor *pVehicleActor = dynamic_cast<CDnVehicleActor *>(m_hActor.GetPointer());
	CDnPetActor *pPetActor = dynamic_cast<CDnPetActor *>(m_hActor.GetPointer());

	if( pPlayerActor && !pVehicleActor )
		m_eActorType = ActorEnum::PlayerActor;
	else if( !pPlayerActor && pVehicleActor )
		m_eActorType = ActorEnum::VehicleActor;
	else if( !pPlayerActor && pPetActor )
		m_eActorType = ActorEnum::PetActor;
}

void CDnRenderAvatarNew::CheckEtcType()
{
	m_eActorType = ActorEnum::EtcObject;
}

bool CDnRenderAvatarNew::ResetActor( bool bInitCamera )
{
	if( m_hActor )
	{
		m_vecPartsInfo.clear();
		ClearRenderObject();
		//m_hActor->SetDestroy();	// SetProcess를 켜놔야 Destroy체크 후 ProcessClass에서 지우는 루틴이 수행된다. SetProcess 안되있으면 지워지지 않는다.
		SAFE_RELEASE_SPTR( m_hActor );	// SetProcess를 켜놓지 않으므로 이렇게 직접 해제해야한다.
		m_eActorType = ActorEnum::None;
		SetRenderFrameCount( 0 );	// -1로 설정해뒀던거 다시 0으로 해서 RTT렌더할때 들어오지 않게 한다.
	}
	if( bInitCamera ) InitCameraControl();
	return true;
}

int CDnRenderAvatarNew::GetActorID()
{
	if( m_hActor )
		return m_hActor->GetClassID();
	return 0;
}

bool CDnRenderAvatarNew::AttachItem( int nItemID, int nEnchantLevel, int nSubArg1, int nLookItemID )
{
	if( !m_hActor ) return true;

	if( m_eActorType == ActorEnum::PlayerActor )
	{
		CDnPlayerActor *pActor = (CDnPlayerActor *)m_hActor.GetPointer();
		DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TITEM );
		if( !pSox ) return false;
		if( !pSox->IsExistItem( nItemID ) ) return false;

#ifdef PRE_ADD_CASH_COSTUME_AURA
		pActor->SetAuraRenderType(RT_TARGET);
#endif

		eItemTypeEnum ItemType = CDnItem::GetItemType( nItemID );
		if( ItemType == ITEMTYPE_HAIRDYE )
		{
			DWORD dwColor = (DWORD)pSox->GetFieldFromLablePtr( nItemID, "_TypeParam1" )->GetInteger();
			pActor->SetPartsColor( MAPartsBody::HairColor, dwColor );
		}
		else if( ItemType == ITEMTYPE_EYEDYE )
		{
			DWORD dwColor = (DWORD)pSox->GetFieldFromLablePtr( nItemID, "_TypeParam1" )->GetInteger();
			pActor->SetPartsColor( MAPartsBody::EyeColor, dwColor );
		}
		else if( ItemType == ITEMTYPE_SKINDYE )
		{
			DWORD dwColor = (DWORD)pSox->GetFieldFromLablePtr( nItemID, "_TypeParam1" )->GetInteger();
			pActor->SetPartsColor( MAPartsBody::SkinColor, dwColor );
		}
		else if( ItemType == ITEMTYPE_PARTS )
		{
			DnPartsHandle hParts = CDnParts::CreateParts( nItemID, 0, 0, 0, 0, 0, false, nLookItemID);
			if( hParts )
			{
				hParts->SetRTTRenderMode( true, this );
				bool bCash = CDnItem::IsCashItem( nItemID );

				// 액터를 그대로 쓰는거기 때문에 별다른 처리 안해도 될 줄 알았는데,
				// 속도때문에 Combine하지 않고 처리해야하다보니, 한벌옷 관련해서 문제가 있었다.
				// Combine할때의 한벌옷 처리는 모두 다 되어있지만, Combine하지 않을때의 한벌옷 처리는 안되있는 것.
				// 이건 Combine하지 않는 로그인 캐릭터 선택화면에서도 마찬가지 문제다.
				// 대신 로그인 화면에서는 일반적인 캐시옷들 쭉 입혀본 후 한벌옷을 입힌다거나,(그 반대의 경우. 한벌옷 입은 후 일반적인 캐시파츠 장착)
				// 하지 않기때문에 별 문제 없었던 것이다.
				//
				// 현재 Combine하지 않기때문에 새로운 파츠를 장착할때 예전 정보를 따로 기억해두지 않고 지우는데,
				// 이로 인해 한벌옷장착된 상태에서 다른 캐시상의파츠를 AttachItem할때 예전에 입던 하의, 장갑, 바지의 정보가 없다는 것이다.
				//
				// 이걸 방지하려면, 캐릭터 장비템 정보를 들고있는 것처럼 노말파츠, 캐시파츠 다 구분해서 기억해둬야하는데,
				// 이건 아이템태스크의 장비기능을 다 빼오는 격이 되버린다.
				// 최대한 렌더링루틴쪽으로만 건드리려했던 렌더아바타코드라,(그래서 AttachItem은 지원해도 DetachItem은 일부러 뺀거다.)
				// 이 기능은 지원하지 않기로 한다.
				//
				// 그래도 최대한 자연스럽게 표현하기 위해 한벌옷에 대해서만 예외처리해서 정보 들고있도록 하겠다.

				if( !bCash )
				{
					// 렌더아바타에서는 CombineParts 를 안하기때문에 노말템 들어올 경우 매칭되는 캐시템을 검사해
					// 캐시템이 있다면 장착하지 않고 넘긴다.
					bool bEquip = true;
					if( hParts->GetPartsType() >= CDnParts::Helmet && hParts->GetPartsType() <= CDnParts::Foot )
					{
						if( pActor->GetCashParts( (CDnParts::PartsTypeEnum)( hParts->GetPartsType() - 2 ) ) )
						{
							bEquip = false;
						}
					}

					// 혹시 장착된 한벌옷 부위인지 확인.(보니까 한벌옷 서브파츠들이 이쪽으로 들어오는듯?)
					for( int i = 0; i < (int)m_vecPartsInfo.size(); ++i )
					{
						if( m_vecPartsInfo[i].nType == hParts->GetPartsType() )
						{
							bEquip = false;
							break;
						}
					}

					if( bEquip )
					{
						pActor->DetachParts( hParts->GetPartsType() );
						pActor->AttachParts( hParts, hParts->GetPartsType(), true, false );
						if( hParts->GetObjectHandle() ) hParts->GetObjectHandle()->SetRenderType( RT_TARGET );

						if( hParts->GetPartsType() == CDnParts::Hair )
						{
							// 머리카락의 경우에 툴에서 알파블렌딩이 설정 안되있을때가 있다. 그냥 강제로 설정한다.
							if( hParts->GetObjectHandle() && hParts->GetObjectHandle()->GetSkinInstance() )
								hParts->GetObjectHandle()->GetSkinInstance()->EnableAlphaBlend( true );
						}
					}
					else
					{
						SAFE_RELEASE_SPTR( hParts );
					}
				}
				else
				{
					// 만약 한벌옷 입고 있었던 중이라면,
					if( m_vecPartsInfo.size() )
					{
						// 현재 장착하는 파츠가 한벌옷에 해당되는 부위인지 확인한다.
						// 지금은 모든 한벌옷이 다 같은 서브파츠를 가지고 있지만, 만약 이게 달라진다면,
						// hParts->GetPartsType()만 검사하는게 아니라, 서브파츠까지 다 꺼내와서 검사해야할거다.
						bool bDetachOnepiece = false;
						for( int i = 0; i < (int)m_vecPartsInfo.size(); ++i )
						{
							if( (m_vecPartsInfo[i].nType - 2) == hParts->GetPartsType() )
							{
								bDetachOnepiece = true;
								break;
							}
						}

						if( bDetachOnepiece )
						{
							std::vector<SPartsInfo> vecPartsInfo;
							vecPartsInfo = m_vecPartsInfo;
							m_vecPartsInfo.clear();

							// 먼저 한벌옷을 해제하고,(첫번째 인덱스가 한벌옷 파츠 부위다.)
							pActor->DetachCashParts( (CDnParts::PartsTypeEnum)( vecPartsInfo[0].nType - 2 ) );

							// 예전 정보로 다 돌린 후
							// 먼저 타입을 다 넣어뒀으니 해당하는 타입에 맞는 아이템ID만 가져와 재저장하면 된다.
							for( int i = 0; i < (int)vecPartsInfo.size(); ++i )
								AttachItem( vecPartsInfo[i].nItemID );

							// 일반적인 캐시템 장착하듯이 아래 루틴 쭉 수행.
						}
					}

#ifdef PRE_ADD_TRANSPARENCY_COSTUME
					bool bTransparentSkin = CDnParts::IsTransparentSkin( hParts->GetClassID(), pActor->GetClassID() );
#endif	// #ifdef PRE_ADD_TRANSPARENCY_COSTUME

					// 입으려는 옷이 한벌옷이라면, 한벌옷에 해당되는 자리의 파츠들을 별도로 기억해둔다.
					// 서브파츠만 하면 자신슬롯이 빠지니 자신도 따로 추가해둔다.
					// (이걸 기억해둬야 한벌옷 입고 있다가 캐시장갑 입을때 이전에 입던 상의로 되돌릴 수 있다.)
					if( hParts->IsExistSubParts() )
					{
						SPartsInfo Info;
						Info.nType = (int)hParts->GetPartsType() + 2;	// 한벌옷은 캐시템이니 +2해서 노말템 타입으로 맞춘다.
						Info.nItemID = 0;
						m_vecPartsInfo.push_back( Info );
						for( int i = 0; i < hParts->GetSubPartsCount(); ++i )
						{
							Info.nType = (int)hParts->GetSubPartsIndex(i) + 2;	// 서브타입 적힌것도 캐시템의 타입이니 +2해서 노말템 타입으로 맞춘다.
							m_vecPartsInfo.push_back( Info );
						}

						// 먼저 타입을 다 넣어뒀으니 해당하는 타입에 맞는 아이템ID만 가져와 재저장하면 된다.
						for( int i = 0; i < (int)m_vecPartsInfo.size(); ++i )
						{
							if( pActor->GetCashParts( (CDnParts::PartsTypeEnum)( m_vecPartsInfo[i].nType - 2 ) ) )
							{
								m_vecPartsInfo[i].nItemID = pActor->GetCashParts( (CDnParts::PartsTypeEnum)( m_vecPartsInfo[i].nType - 2 ) )->GetClassID();
								pActor->DetachCashParts( (CDnParts::PartsTypeEnum)( m_vecPartsInfo[i].nType - 2 ) );
#ifdef PRE_ADD_TRANSPARENCY_COSTUME
								// 투명한벌옷으로 입혀야할때는 일반파츠모습을 보여줘야한다.
								if( bTransparentSkin )
								{
									DnPartsHandle hNormalParts = pActor->GetParts( (CDnParts::PartsTypeEnum)( m_vecPartsInfo[i].nType ) );
									if( hNormalParts && !hNormalParts->GetObjectHandle() )
									{
										DNTableFileFormat* pActorSox = GetDNTable( CDnTableDB::TACTOR );
	#ifdef PRE_FIX_MEMOPT_EXT
										std::string szAniName;
										CommonUtil::GetFileNameFromFileEXT(szAniName, pActorSox, pActor->GetClassID(), "_AniName");
	#else
										std::string szAniName = pActorSox->GetFieldFromLablePtr( pActor->GetClassID(), "_AniName" )->GetString();
	#endif
										hNormalParts->CreateObject( m_hActor, szAniName.c_str() );
									}
								}
#endif	// #ifdef PRE_ADD_TRANSPARENCY_COSTUME
							}
							else if( pActor->GetParts( (CDnParts::PartsTypeEnum)( m_vecPartsInfo[i].nType ) ) )
							{
								m_vecPartsInfo[i].nItemID = pActor->GetParts( (CDnParts::PartsTypeEnum)( m_vecPartsInfo[i].nType ) )->GetClassID();
#ifdef PRE_ADD_TRANSPARENCY_COSTUME
								if( !bTransparentSkin ) pActor->GetParts( (CDnParts::PartsTypeEnum)( m_vecPartsInfo[i].nType ) )->FreeObject();
#else
								pActor->DetachParts( (CDnParts::PartsTypeEnum)( m_vecPartsInfo[i].nType ) );
#endif	// #ifdef PRE_ADD_TRANSPARENCY_COSTUME
							}
							// 원래 제대로 액터쪽에서 디태치처리 되었다면 빈 슬롯이더라도 디폴트파츠 다 들어있을 것이다.
							// 그래도 혹시 모르니 아래 처리 해둔다.
							else if( m_vecPartsInfo[i].nType >= CDnParts::DefaultPartsType_Min && m_vecPartsInfo[i].nType <= CDnParts::DefaultPartsType_Max )
							{
								m_vecPartsInfo[i].nItemID = pActor->GetDefaultPartsInfo( (CDnParts::PartsTypeEnum)m_vecPartsInfo[i].nType );
							}
						}
					}

					// 렌더아바타에서는 CombineParts 를 안하기때문에 여기서 노말템을 빼주는식으로 합니다.
					if( hParts->GetPartsType() >= CDnParts::CashHelmet && hParts->GetPartsType() <= CDnParts::CashFoot )
					{
#ifdef PRE_ADD_TRANSPARENCY_COSTUME
						// 투명 한벌옷때문에 Detach하지 않고 FreeObject하는걸로 바꾼다. 그래야 나중에 필요할때 다시 CreateObject할 수 있다.
						DnPartsHandle hNormalParts = pActor->GetParts( (CDnParts::PartsTypeEnum)( hParts->GetPartsType() + 2 ) );
						if( hNormalParts && !bTransparentSkin ) hNormalParts->FreeObject();
#else
						if( pActor->GetParts( (CDnParts::PartsTypeEnum)( hParts->GetPartsType() + 2 ) ) )
							pActor->DetachParts( (CDnParts::PartsTypeEnum)( hParts->GetPartsType() + 2 ) );
#endif	// #ifdef PRE_ADD_TRANSPARENCY_COSTUME
					}

					if( nSubArg1 == 0 )
					{
						pActor->DetachCashParts( hParts->GetPartsType() );
						pActor->AttachCashParts( hParts, hParts->GetPartsType(), true, false );
					}
					else if( nSubArg1 == 1 )
					{
						if( hParts->GetPartsType() == CDnParts::CashRing )
						{
							pActor->DetachCashParts( CDnParts::CashRing2 );
							pActor->AttachCashParts( hParts, CDnParts::CashRing2, true, false );
						}
					}
					if( hParts->GetObjectHandle() ) hParts->GetObjectHandle()->SetRenderType( RT_TARGET );
#ifdef PRE_ADD_TRANSPARENCY_COSTUME
					else if( bTransparentSkin )
					{
						DnPartsHandle hNormalParts = pActor->GetParts( (CDnParts::PartsTypeEnum)( hParts->GetPartsType() + 2 ) );
						if( hNormalParts && !hNormalParts->GetObjectHandle() )
						{
							DNTableFileFormat* pActorSox = GetDNTable( CDnTableDB::TACTOR );
	#ifdef PRE_FIX_MEMOPT_EXT
							std::string szAniName;
							CommonUtil::GetFileNameFromFileEXT(szAniName, pActorSox, pActor->GetClassID(), "_AniName");
	#else
							std::string szAniName = pActorSox->GetFieldFromLablePtr( pActor->GetClassID(), "_AniName" )->GetString();
	#endif
							hNormalParts->CreateObject( m_hActor, szAniName.c_str() );
						}
					}
#endif	// #ifdef PRE_ADD_TRANSPARENCY_COSTUME

					// 우선 파츠에 대해서만 한다. 웨폰은 이런 경우 거의 없는 듯.
					if( hParts->GetPartsType() == CDnParts::CashHelmet )
					{
						// 머리장식 중 일부 파츠들엔 메시끝부분에 알파텍스처를 사용하는데도 알파블렌딩이 툴에서 설정 안되있는 경우가 있다.
						// 이렇게 해도 제대로 나오도록 강제로 활성화시킨다.
						// 하지만 보니까, PostProcess에서 알파값 그대로 유지하도록 처리하고나선,
						// 테두리 알파 이상하게 보이던 문제가 거의 해결된거 같다.
						// 그래서 강제로 알파블렌드 켜는걸 해제하도록 하겠다.
						//if( hParts->GetObjectHandle() && hParts->GetObjectHandle()->GetSkinInstance() )
						//	hParts->GetObjectHandle()->GetSkinInstance()->EnableAlphaBlend( true );
					}
					else if( hParts->GetPartsType() == CDnParts::CashWing )
					{
						// 날개의 경우엔 강제로 알파블렌드, 투패스 켜서 깔끔하게 나오게 한다.
						if( hParts->GetObjectHandle() && hParts->GetObjectHandle()->GetSkinInstance() )
						{
							hParts->GetObjectHandle()->GetSkinInstance()->EnableAlphaBlend( true );
							hParts->GetObjectHandle()->GetSkinInstance()->SetRenderAlphaTwoPass( true );
						}
					}
				}
			}

#ifdef PRE_MOD_WARRIOR_RIGHTHAND
			// 로그인 캐릭터 선택화면과 같이 CombineParts 를 안하기때문에 워리어 건틀렛꼈을때 오른쪽 장갑메시 하이드 하는 것을 강제로 호출한다.
			pActor->OnCombineParts();
#endif	// #ifdef PRE_MOD_WARRIOR_RIGHTHAND
		}
		else if( ItemType == ITEMTYPE_WEAPON )
		{
			DnWeaponHandle hWeapon = CDnWeapon::CreateWeapon( nItemID, 0, 0, nEnchantLevel, 0, 0, false, false, true, ITEMCLSID_NONE );
			if( hWeapon )
			{
				hWeapon->SetRTTRenderMode( true, this );
				bool bCash = CDnItem::IsCashItem( nItemID );

				int nEquipIndex = CDnCharStatusDlg::EQUIPTYPE_2_EQUIPINDEX( hWeapon->GetEquipType() ) - EQUIP_WEAPON1;
				ASSERT( nEquipIndex == 0 || nEquipIndex == 1 );

				if( !bCash )
				{
					pActor->DetachWeapon( nEquipIndex );
					pActor->AttachWeapon( hWeapon, nEquipIndex, true );
				}
				else
				{
					pActor->DetachCashWeapon( nEquipIndex );
					pActor->AttachCashWeapon( hWeapon, nEquipIndex, true, false );
				}

				if( hWeapon->GetObjectHandle() )
				{
					// 일반메이스와 캐시프레일을 조합할 경우가 문제다.
					// 메이스애니를 비롯해서 메이스 액트파일까지 새로 만들어야하는 등, 리소스단에서 할일이 너무 많아지므로 강제로 시뮬레이션 안쓰는거 하지 않는다.
					//hWeapon->GetObjectHandle()->DisableSimulation();
					hWeapon->GetObjectHandle()->SetRenderType( RT_TARGET );
				}
			}
		}
		// 얼굴상품이랑 머리모양 상품.
		else if( ItemType == ITEMTYPE_FACIAL || ItemType == ITEMTYPE_HAIRDRESS )
		{
			int nParam = pSox->GetFieldFromLablePtr( nItemID, "_TypeParam1" )->GetInteger();
			AttachItem( nParam );

			// 알파 객체들을 거리에 따라 정렬하지 않기 때문에, Hair바꿀경우 캐시모자를 다시 장착시켜 Hair보다 더 뒤에 렌더링되게 한다.
			if( ItemType == ITEMTYPE_HAIRDRESS )
			{
				if( pActor->GetCashParts( CDnParts::CashHelmet ) )
				{
					DnPartsHandle hHelmet = pActor->GetCashParts( CDnParts::CashHelmet );
					if (!hHelmet)
					{
#ifdef _WORK
						std::wstring str = FormatW(L"DnRenderAvatar::AttachItem - Character CashHelmet does not exist : (%d)", nItemID);
						GetInterface().AddChatMessage(CHATTYPE_NOTICE, L"", str.c_str(), true);
#endif
						return false;
					}

					int nAttachHelmetId = (hHelmet->GetLookItemID() != ITEMCLSID_NONE) ? hHelmet->GetLookItemID() : hHelmet->GetClassID();
					pActor->DetachCashParts( CDnParts::CashHelmet );
					AttachItem(nAttachHelmetId);
				}
			}
		}
	}
	else if( m_eActorType == ActorEnum::VehicleActor )
	{
		// 현재 탈것은 생성된 액터를 넘겨받아 생성하는 SetActor만 사용되서 아직 AttachItem은 미구현상태다.
	}

	return true;
}

bool CDnRenderAvatarNew::DetachItem( int nItemID )
{
	if( !m_hActor ) return true;

	if( m_eActorType == ActorEnum::PlayerActor )
	{
		CDnPlayerActor *pActor = (CDnPlayerActor *)m_hActor.GetPointer();
		DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TITEM );
		if( !pSox ) return false;
		if( !pSox->IsExistItem( nItemID ) ) return false;

		eItemTypeEnum ItemType = CDnItem::GetItemType( nItemID );
		if( ItemType == ITEMTYPE_PARTS )
		{
			// 캐시템 낀 상태에서 캐시템 바꾸는건 이미 다 처리되어있기때문에 신경 안써도 된다.
			// 캐시템 낀 상태에서 일반템으로 바꾸기 위해서 현재의 캐시파츠를 강제로 디태치 시키는 것만 있으면 된다.
			if( CDnItem::IsCashItem( nItemID ) )
			{
				CDnParts::PartsTypeEnum PartsType = CDnParts::GetPartsType( nItemID );
				pActor->DetachCashParts( PartsType );

				// 한벌옷입고있을때는 추가로 처리해야한다.
				bool bDetachOnepiece = false;
				if( m_vecPartsInfo.size() )
				{
					// 현재 해제하는 파츠가 한벌옷에 해당되는 부위인지 확인한다.
					int nItemID = 0;
					for( int i = 0; i < (int)m_vecPartsInfo.size(); ++i )
					{
						if( (m_vecPartsInfo[i].nType - 2) == PartsType )
						{
							bDetachOnepiece = true;
							break;
						}
					}

					if( bDetachOnepiece )
					{
						std::vector<SPartsInfo> vecPartsInfo;
						vecPartsInfo = m_vecPartsInfo;
						m_vecPartsInfo.clear();

						for( int i = 0; i < (int)vecPartsInfo.size(); ++i )
							AttachItem( vecPartsInfo[i].nItemID );
					}
				}
			}
		}
		else if( ItemType == ITEMTYPE_WEAPON )
		{
			if( CDnItem::IsCashItem( nItemID ) )
			{
				int nEquipIndex = CDnCharStatusDlg::EQUIPTYPE_2_EQUIPINDEX( CDnWeapon::GetEquipType( nItemID ) ) - EQUIP_WEAPON1;
				ASSERT( nEquipIndex == 0 || nEquipIndex == 1 );
				pActor->DetachCashWeapon( nEquipIndex );
			}
		}
	}
	return true;
}

bool CDnRenderAvatarNew::RefreshEquip( DnActorHandle hActor )
{
	if( !hActor ) return true;
	if( !m_hActor ) return true;

	if( m_eActorType == ActorEnum::PlayerActor )
	{
	}
	else if( m_eActorType == ActorEnum::VehicleActor )
	{
		CDnVehicleActor *pVehicleActor = (CDnVehicleActor *)hActor.GetPointer();
		CDnVehicleActor *pActor = (CDnVehicleActor *)m_hActor.GetPointer();

		// 말정보 얻어와서 셋팅
		for( int i = Vehicle::Slot::Saddle; i < Vehicle::Slot::Max; ++i )
		{
			if( !pVehicleActor->GetMyPlayerActor() || pVehicleActor->GetMyPlayerActor()->IsDestroy()) continue;
			if( !pVehicleActor->GetMyPlayerActor()->GetVehicleInfo().Vehicle[i].nItemID )
			{
				Vehicle::Parts::eVehicleParts ePartsType = Vehicle::Parts::Default;
				switch(i)
				{
				case Vehicle::Slot::Saddle:	ePartsType = Vehicle::Parts::Saddle; break;
				case Vehicle::Slot::Hair: ePartsType = Vehicle::Parts::Hair; break;
				}
				pActor->UnEquipItem( ePartsType );
			}
			else
			{
				pActor->EquipItem( pVehicleActor->GetMyPlayerActor()->GetVehicleInfo().Vehicle[i] );

				DnSimplePartsHandle hParts = pActor->GetVehicleParts( (Vehicle::Parts::eVehicleParts)(i - Vehicle::Slot::Saddle) );
				if( hParts && hParts->GetObjectHandle() ) hParts->GetObjectHandle()->SetRenderType( RT_TARGET );

				if( i == Vehicle::Slot::Hair )
				{
					if( hParts->GetObjectHandle() && hParts->GetObjectHandle()->GetSkinInstance() )
					{
						hParts->GetObjectHandle()->GetSkinInstance()->EnableAlphaBlend( true );
						hParts->GetObjectHandle()->GetSkinInstance()->SetRenderAlphaTwoPass( true );
					}
				}
			}
		}
		pActor->ChangeHairColor( pVehicleActor->GetMyPlayerActor()->GetVehicleInfo().dwPartsColor1 );
	}
	else if( m_eActorType == ActorEnum::PetActor )
	{
		CDnPetActor* pPetActor = (CDnPetActor *)hActor.GetPointer();
		CDnPetActor *pActor = (CDnPetActor *)m_hActor.GetPointer();

		for( int i = Pet::Slot::Accessory1; i < Pet::Slot::Max; ++i )
		{
			if( !pPetActor->GetPetInfo().Vehicle[i].nItemID )
			{
				Pet::Parts::ePetParts ePartsType = Pet::Parts::Default;
				switch( i )
				{
					case Pet::Slot::Accessory1: ePartsType = Pet::Parts::PetAccessory1; break;
					case Pet::Slot::Accessory2: ePartsType = Pet::Parts::PetAccessory2; break;
				}
				pActor->UnEquipItem( ePartsType );
			}
			else
			{
				pActor->EquipItem( pPetActor->GetPetInfo().Vehicle[i] );

				DnSimplePartsHandle hParts = pActor->GetPetParts( (Vehicle::Parts::eVehicleParts)(i - Pet::Slot::Accessory1) );
				if( hParts && hParts->GetObjectHandle() ) hParts->GetObjectHandle()->SetRenderType( RT_TARGET );
			}
		}

		pActor->ChangeColor( ePetParts::PET_PARTS_BODY, pPetActor->GetPetInfo().dwPartsColor1 );
		pActor->ChangeColor( ePetParts::PET_PARTS_NOSE, pPetActor->GetPetInfo().dwPartsColor2 );
	}

	return true;
}

void CDnRenderAvatarNew::InitCameraControl()
{
	m_nRotateYaw = 0;
	m_fSmoothRotateYaw = 0.0f;
	m_nZoomLevel = 0;
	m_fSmoothZoomLevel = 0.0f;
	m_fPanX = 0.0f;
	m_fPanY = 0.0f;
	m_fSmoothPanX = 0.0f;
	m_fSmoothPanY = 0.0f;
}

void CDnRenderAvatarNew::CalcUVCoord( float fUIWidth, float fUIHeight )
{
	int nWidth = int(fUIWidth * DEFAULT_UI_SCREEN_WIDTH);
	int nHeight = int(fUIHeight * DEFAULT_UI_SCREEN_HEIGHT);

	if( fUIWidth == 0.0f || fUIHeight == 0.0f )
		nWidth = nHeight = 0;

	if( nWidth == nHeight )
	{
		m_RTTUVCoord.fX = m_RTTUVCoord.fY = 0.0f;
		m_RTTUVCoord.fWidth = m_RTTUVCoord.fHeight = 1.0f;
	}
	else if( nWidth < nHeight )
	{
		m_RTTUVCoord.fY = 0.0f;
		m_RTTUVCoord.fHeight = 1.0f;
		m_RTTUVCoord.fWidth = fUIWidth / fUIHeight;
		m_RTTUVCoord.fX = (1.0f - m_RTTUVCoord.fWidth) / 2.0f;
	}
	else if( nWidth > nHeight )
	{
		m_RTTUVCoord.fX = 0.0f;
		m_RTTUVCoord.fWidth = 1.0f;
		m_RTTUVCoord.fHeight = fUIHeight / fUIWidth;
		m_RTTUVCoord.fY = (1.0f - m_RTTUVCoord.fHeight) / 2.0f;
	}
}

void CDnRenderAvatarNew::Process( float fElapsedTime )
{
	LOCAL_TIME LocalTime = 0;

	if( CDnActor::s_hLocalActor )
	{
		LocalTime = CDnActor::s_hLocalActor->GetLocalTime();
	}
	else
	{
		CTask *pTask = CTaskManager::GetInstance().GetTask( "ItemTask" );
		if(pTask)
			LocalTime = pTask->GetLocalTime();
	}

	if( m_eActorType == ActorEnum::PlayerActor )
	{
		if( !m_hActor )
			return;

		// -- 필요한 프로세스 --
		// 액터가 상속받은 "ActionBase"의 프로세스 액션 (몬스터 등이 여기다가 직접 파티클 달아서 렌더)
		// "캐시무기" 프로세스, "일반무기" 프로세스
		// "파츠" 프로세스
		CDnActor*		pActor  = (CDnActor*)m_hActor.GetPointer();
		CDnPlayerActor*	pPlayer = (CDnPlayerActor*)m_hActor.GetPointer();

		pActor->CDnActionBase::ProcessAction( LocalTime, fElapsedTime );
#ifdef PRE_ADD_CASH_COSTUME_AURA 
		pPlayer->ShowCostumeAura();
		CDnEtcObject::ProcessClass( LocalTime, fElapsedTime );
#endif
		pActor->CDnActionSignalImp::Process( LocalTime, fElapsedTime );

		// 애니메이션 계산
		pActor->MAActorRenderBase::PreProcess( LocalTime, fElapsedTime );

		// 아래 통틀어서 MAPartsBody의 Process를 돌릴까?
		EtMatrix WorldMat = *pActor->GetMatEx();
		pActor->MAActorRenderBase::Process( &WorldMat, LocalTime, fElapsedTime );

		// 일반 무기
		for( int i = 0; i < CDnWeapon::EquipSlot_Amount; i++ )
		{
			DnWeaponHandle hWeapon = pPlayer->GetWeapon( i );
			if( !hWeapon ) continue;

			hWeapon->Process( LocalTime, fElapsedTime );
		}

		// 우선 캐시파츠만 따로 돌려본다.
		for( int i = 0; i < CDnParts::CashPartsTypeEnum_Amount; i++ )
		{
			DnPartsHandle hParts = pPlayer->GetCashParts( (CDnParts::PartsTypeEnum)i );
			if( !hParts ) continue;

			hParts->Process( LocalTime, fElapsedTime );
		}

		// 캐시무기도 추가.
		for( int i = 0; i < CDnWeapon::EquipSlot_Amount; i++ )
		{
			DnWeaponHandle hWeapon = pPlayer->GetCashWeapon( i );
			if( !hWeapon ) continue;

			hWeapon->Process( LocalTime, fElapsedTime );
		}

#ifdef PRE_MOD_KALI_WEAPON
		// CDnPlayerActor::Process에 너무 많은게 들어있어서 위처럼 필요한 Process함수만 따로 호출하다보니,
		// TDnPlayerKali::Process안에 있는 LeftWeapon-Process처리를 할 수가 없다.
		// 그렇다고 CDnPlayerActor::Process를 호출할 순 없어서, 이렇게 별도의 함수를 호출하는거로 하겠다.
		pPlayer->ProcessAdditionalWeapon( LocalTime, fElapsedTime );
#endif
	}
	else if( m_eActorType == ActorEnum::VehicleActor || m_eActorType == ActorEnum::PetActor )
	{
		if( !m_hActor )
			return;

		CDnActor *pActor = (CDnActor *)m_hActor.GetPointer();

		pActor->CDnActionBase::ProcessAction( LocalTime, fElapsedTime );
		pActor->CDnActionSignalImp::Process( LocalTime, fElapsedTime );

		// 애니메이션 계산
		pActor->MAActorRenderBase::PreProcess( LocalTime, fElapsedTime );

		EtMatrix WorldMat = *pActor->GetMatEx();
		pActor->MAActorRenderBase::Process( &WorldMat, LocalTime, fElapsedTime );
	}
	else if( m_eActorType == ActorEnum::EtcObject )
	{
		if( !m_hEtc )
			return;

		CDnEtcObject * pObject = m_hEtc.GetPointer();
		if( NULL == pObject )
			return;
		EtObjectHandle hObject = pObject->GetObjectHandle();
		if( CEtObject::Identity() == hObject )
			return;

		LOCAL_TIME LocalTime = CDnActor::s_hLocalActor->GetLocalTime();

		pObject->CDnActionBase::ProcessAction( LocalTime, fElapsedTime );

		pObject->CDnActionSignalImp::Process( LocalTime, fElapsedTime );

		EtMatrix WorldMat = *pObject->GetMatEx();
		pObject->CDnActionRenderBase::Process( &WorldMat, LocalTime, fElapsedTime );
		return;
	}

	// 카메라 셋팅.
	// 회전
	float fCurrentRotateYaw = EtToRadian( (float)m_nRotateYaw / ROTATE_SCALE );
	float fDistYaw = (fCurrentRotateYaw - m_fSmoothRotateYaw );
	m_fSmoothRotateYaw += fDistYaw * EtMin(1.0f, (fElapsedTime * 15.f));

	EtMatrix CamMat, CamPos, CamRotate, CamPI;
	EtMatrixRotationY( &CamPI, ET_PI );	// 약간 위 z값 양수 거리에서(화면에서 먼쪽) 원점을 바라봐야하므로 처음부터 반대 보게 해둔다.
	EtMatrixTranslation( &CamPos, 0.0f, m_fDefaultCameraYPos, m_fDefaultCameraZPos );
	EtMatrixRotationY( &CamRotate, -m_fSmoothRotateYaw );	// 반대로 뒀기 때문에 Yaw적용 역시 반대로.
	EtMatrixMultiply( &CamMat, &CamPI, &CamPos );
	EtMatrixMultiply( &CamMat, &CamMat, &CamRotate );
	UpdateCamera( CamMat );

	// 줌
	static float fScale = 1.2f;
	float fZoomLevel[5] = {	1.0f, 
		fScale, 
		fScale*fScale, 
		fScale*fScale*fScale,
		fScale*fScale*fScale*fScale };

	m_fSmoothZoomLevel += ( m_nZoomLevel * 0.999f - m_fSmoothZoomLevel ) * EtMin( 1.0f, fElapsedTime * 25.0f );	// m_nZoomLevel에 따라 0.0 ~ 4.0 사이값.
	float fMod = fmodf( m_fSmoothZoomLevel, 1.0f );	// 1.0, 2.0, 3.0, 4.0일때는 보간안할거고 사이에 있으면 보간해야하니 보간값 구하고,
	float fResultScale = fZoomLevel[ (int)m_fSmoothZoomLevel ] * (1.0f-fMod) + fZoomLevel[ (int)(m_fSmoothZoomLevel+1) ] * fMod;	// 보간.

	// 위치.
	static float fXLimit = 1.4f;
	static float fYLimit = 2.0f;
	float fPanLimitWidth = float(m_nDefaultViewPortWidth) / fResultScale * 0.5f * ( EtMin(fXLimit, fResultScale) - 1.0f );
	float fPanLimitHeight = float(m_nDefaultViewPortHeight) / fResultScale * 0.5f * ( EtMin(fYLimit, fResultScale) - 1.0f );
	m_fPanX = EtClamp( m_fPanX, -fPanLimitWidth, fPanLimitWidth );
	m_fPanY = EtClamp( m_fPanY, -fPanLimitHeight, fPanLimitHeight );
	m_fSmoothPanX += ( m_fPanX - m_fSmoothPanX ) * EtMin( 1.0f, fElapsedTime * 10.0f );
	m_fSmoothPanY += ( m_fPanY - m_fSmoothPanY ) * EtMin( 1.0f, fElapsedTime * 10.0f );

	// 카메라 설정.
	m_hCamera->SetOrthogonalView( int(m_nDefaultViewPortWidth / fResultScale), int(m_nDefaultViewPortHeight / fResultScale), m_fSmoothPanX, -m_fSmoothPanY );
}

bool CDnRenderAvatarNew::IsLoading()
{
	bool bLoading = false;
	if( CDnLoadingTask::IsActive() )
	{
		// 진짜 로딩화면중이라면 텍스처 생성하지 않는다.
		if( CDnLoadingTask::GetInstance().IsEnableBackgroundLoading() == false )
			bLoading = true;
		if( GetInterface().IsPVPLobby() )
			bLoading = false;
	}
	return bLoading;
}

void CDnRenderAvatarNew::BeginRTT()
{
	// 렌더아바타쪽엔 필터링이랑 관련해서 글로벌 라이트 및 환경들을 바꾸는게 많아서
	// 라이트맵 구울때는 동작하지 않게 해야한다.
	if( IsLoading() ) return;

	CEtRTTRenderer::BeginRTT();

	// 컬러필터만 적용시킬게 아니라, 월드에 적용되는 라이트도 같이 바꿔줘야한다. 아래값은 프레리에서 설정된 값을 가져온 것이다.(필터도 프레리꺼다.)
	m_TempAmbient = *CEtLight::GetGlobalAmbient();
	CEtLight::SetGlobalAmbient( &s_TempAmbient );

	// 원래 아래 세팅은 Initialize함수에서 하던건데,
	// 환경로딩보다 RenderAvatar::Initialize가 먼저 호출되면, 괜한 라이트 두개가 더 추가되어버려서,
	// 이렇게 BeginRTT에서 체크하기로 했다.
	// 라이트객체가 아예 없는 경우(맵 로딩 안해서 환경이 로드되지 않았을때) 직접 임시라이트를 생성해서 처리한다.
	if( CEtLight::GetDirLightCount() == 0 )
	{
		if( s_hTempLight[0] && s_hTempLight[1] )
		{
			CEtLight::ForcePushDirLightInfo( s_hTempLight[0].GetPointer() );
			CEtLight::ForcePushDirLightInfo( s_hTempLight[1].GetPointer() );
			m_bUseTempLight = true;
		}
	}

	m_nDirLightCount = CEtLight::GetDirLightCount();
	m_nDirLightCount = min( m_nDirLightCount, 2 );
	for( int i = 0; i < m_nDirLightCount; ++i )
	{
		m_TempLightDiffuse[i] = CEtLight::GetDirLightInfo(i)->Diffuse;
		m_TempLightDir[i] = CEtLight::GetDirLightInfo(i)->Direction;
		CEtLight::GetDirLightInfo(i)->Diffuse = s_TempLightDiffuse[i];
		CEtLight::GetDirLightInfo(i)->Direction = s_TempLightDir[i];
	}

	// Environment Texture Setting
	if( CEtTexture::GetEnvTexture() == NULL && s_hTempEnviTexture )
	{
		CEtTexture::SetEnvTexture( s_hTempEnviTexture->GetTexturePtr() );
	}
}

void CDnRenderAvatarNew::EndRTT()
{
	if( IsLoading() ) return;

	CEtLight::SetGlobalAmbient( &m_TempAmbient );
	for( int i = 0; i < m_nDirLightCount; ++i )
	{
		CEtLight::GetDirLightInfo(i)->Diffuse = m_TempLightDiffuse[i];
		CEtLight::GetDirLightInfo(i)->Direction = m_TempLightDir[i];
	}
	if( m_bUseTempLight )
	{
		CEtLight::ForceClearDirLightInfo();
		m_bUseTempLight = false;
	}
	CEtRTTRenderer::EndRTT();
}

void CDnRenderAvatarNew::RenderRTT( float fElapsedTime )
{
	if( IsLoading() ) return;

	if( m_hActor )
	{
		DnActorHandle hActor = m_hActor;

		if( m_eActorType == ActorEnum::PlayerActor )
		{
			MAPartsBody *pPartsBody = dynamic_cast<MAPartsBody*>(hActor.GetPointer());
			EtObjectHandle hObject = pPartsBody->GetObjectHandle();
			hObject->InitRender();
			CEtRTTRenderer::RenderRTT( fElapsedTime );

			// 후처리 통해서 컬러필터 적용
			PostProcess();

			// 각 태스크 EternityEngine::RenderFrame( fDelta ); 호출할때
			// 그 전에 매트릭스 계산 안되어있으면 CEtObject::InitRenderObjectList(); 함수 호출할때 그 프레임에 사용할 매트릭스가 계산되서 저장된다.
			// 이때 각 CEtAniObject에 대해 CEtAniObject::InitRender가 호출되는데 이때 CEtAniObject::CalcAni로 계산되는 것.
			//
			// 위처럼 UI Process단에서 하면, 해당 프레임에서 한번도 계산되지 않은 상태이기때문에, -1 이 들어있게 된다.
			// 제대로 호출해야할 부분에서 호출되면 값 들어있을거다.
		}
		else if( m_eActorType == ActorEnum::VehicleActor || m_eActorType == ActorEnum::PetActor )
		{
			CDnVehicleActor *pActor = (CDnVehicleActor *)hActor.GetPointer();
			EtObjectHandle hObject = pActor->GetObjectHandle();

			hObject->InitRender();
			CEtRTTRenderer::RenderRTT( fElapsedTime );
			PostProcess();

		}
	}
	else if( m_hEtc )
	{
		if( m_eActorType == ActorEnum::EtcObject )
		{
			CDnEtcObject * pObject = m_hEtc.GetPointer();
			if( NULL == pObject )
				return;
			EtObjectHandle hObject = pObject->GetObjectHandle();
			if( CEtObject::Identity() == hObject )
				return;

			CEtRTTRenderer::InitRenderObject();
			CEtRTTRenderer::RenderRTT( fElapsedTime );
			PostProcess();
		}
	}
}

void CDnRenderAvatarNew::PostProcess()
{
	// 투명한 판에 그리는거라 일반적인 블렌딩 방식으로는 알파오브젝트가 흐리게 나올 수 밖에 없다.
	// 게다가 드네는 색상조정 필터가 있어서 먼저 UI를 그리고 그 위에 파티클을 그리는건 할수가 없었다.(바탕에 UI그린것까지 필터를 먹어서...)
	// 그래서 생각한게,
	// 투명한 판에 그리되 처음 그려진 알파값을 그대로 유지하는 것이다.(필터를 먹이던 BorderAlpha를 주던.)
	// 이걸 위해서 알파블렌딩을 D3DBLEND_ONE, D3DBLEND_ONE 으로 사용하게 되었다.
	// 어차피 원본텍스처에서 타겟텍스처로 1회 그리는거기때문에,
	// 컬러값, 알파값 모두 그대로 한번만 복사되면 알파값을 잃어버리는 거 없이 제대로 그려지게 된다.

	if( m_bUseColorAdjFilter )
	{
		// 임시텍스처에 현재 RTT 내용을 복사한 후
		EtTextureHandle hTempTarget = GetTempRenderTarget( m_nWidth, m_nHeight, m_Format );
		hTempTarget->Copy( GetRTT() );

		// 프레리에서 쓰는걸로 통일해서 사용한다.
		float fSat = 0.f;
		fSat = s_pColorAdjFilter->GetSceneSaturation();
		s_pColorAdjFilter->SetSceneSaturation( 1.0f );
		bool bEnableZ = GetEtDevice()->EnableZ( false );	
		bool bAlphaEnable = GetEtDevice()->EnableAlphaBlend( true );
		// 바탕이 0x0이므로 One-One으로 그려서 Source 텍스처의 픽셀을 그대로 옮긴다.
		GetEtDevice()->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
		GetEtDevice()->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
		GetEtDevice()->SetDepthStencilSurface( NULL );
		s_pColorAdjFilter->SetTexture( hTempTarget->GetMyIndex() );
		s_pColorAdjFilter->DrawFilter( GetRTT(), 1, EtVector2(0, 0), EtVector2(1, 1) , true );
		GetEtDevice()->RestoreRenderTarget();
		GetEtDevice()->RestoreDepthStencil();
		GetEtDevice()->EnableZ( bEnableZ );
		GetEtDevice()->EnableAlphaBlend( bAlphaEnable );
		s_pColorAdjFilter->SetSceneSaturation( fSat );
	}

	if( m_bUseBorderAlpha )
	{
		// 임시텍스처에 현재 RTT 내용을 복사한 후
		EtTextureHandle hTempTarget = GetTempRenderTarget( m_nWidth, m_nHeight, m_Format );
		hTempTarget->Copy( GetRTT() );

		// RTT텍스처를 렌더타겟으로 재설정.
		bool bEnableZ = GetEtDevice()->EnableZ( false );
		bool bAlphaEnable = GetEtDevice()->EnableAlphaBlend( true );
		// 바탕이 0x0이므로 One-One으로 그려서 Source 텍스처의 픽셀을 그대로 옮긴다.
		GetEtDevice()->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
		GetEtDevice()->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
		GetEtDevice()->SetDepthStencilSurface( NULL );

		GetEtDevice()->SetRenderTarget( GetRTT()->GetSurfaceLevel() );
		GetEtDevice()->ClearBuffer( 0x0, 1.0f, 0, true, false, false );

		// 버텍스 알파처리해서 경계 알파처리.
		STextureDiffuseVertex2D Vertices[ 16 ];
		int nWidth, nHeight;
		EtViewPort Viewport;

		GetEtDevice()->SetFVF( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
		GetEtDevice()->SetPixelShader( NULL );
		GetEtDevice()->SetVertexShader( NULL );
		GetEtDevice()->SetTexture( 0, hTempTarget->GetTexturePtr() );
		GetEtDevice()->GetViewport( &Viewport );

		nWidth = Viewport.Width;
		nHeight = Viewport.Height;

		static float fBorder = 0.04f;	// 테두리에서 4% 경계 알파처리.
		float XPos[4] = { m_RTTUVCoord.fX, 0.0f, 0.0f, m_RTTUVCoord.fX + m_RTTUVCoord.fWidth };	// RTTUVCoord에 값 설정했다고 가정
		float YPos[4] = { m_RTTUVCoord.fY, 0.0f, 0.0f, m_RTTUVCoord.fY + m_RTTUVCoord.fHeight };
		XPos[1] = XPos[0] + (XPos[3]-XPos[0]) * fBorder;
		XPos[2] = XPos[3] - (XPos[3]-XPos[0]) * fBorder;
		YPos[1] = YPos[0] + (YPos[3]-YPos[0]) * fBorder;
		YPos[2] = YPos[3] - (YPos[3]-YPos[0]) * fBorder;

		float UVXPos[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		float UVYPos[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		for( int i = 0; i < 4; ++i )
		{
			UVXPos[i] = XPos[i];
			UVYPos[i] = YPos[i];
			XPos[i] = XPos[i] * nWidth - 0.5f;
			YPos[i] = YPos[i] * nHeight - 0.5f;
		}

		for( int i = 0; i < 4; i++)
		{
			Vertices[i*4+0].Position.y = Vertices[i*4+1].Position.y = Vertices[i*4+2].Position.y = Vertices[i*4+3].Position.y = YPos[i];
			Vertices[ i ].Position.x = Vertices[4 + i].Position.x = Vertices[8 + i].Position.x = Vertices[12 + i].Position.x = XPos[i];
			Vertices[ i ].Position.z = Vertices[4 + i].Position.z = Vertices[8 + i].Position.z = Vertices[12 + i].Position.z = 0.0f;
			Vertices[ i ].Position.w = Vertices[4 + i].Position.w = Vertices[8 + i].Position.w = Vertices[12 + i].Position.w = 1.0f;
			Vertices[ i ].Color = Vertices[4 + i].Color = Vertices[8 + i].Color = Vertices[12 + i].Color = D3DCOLOR_ARGB(255, 255, 255, 255);

			Vertices[i*4+0].TexCoordinate.y = Vertices[i*4+1].TexCoordinate.y = Vertices[i*4+2].TexCoordinate.y = Vertices[i*4+3].TexCoordinate.y = UVYPos[i];
			Vertices[ i ].TexCoordinate.x = Vertices[4 + i].TexCoordinate.x = Vertices[8 + i].TexCoordinate.x = Vertices[12 + i].TexCoordinate.x = UVXPos[i];
		}
		Vertices[ 0 ].Color = Vertices[ 1 ].Color = Vertices[ 2 ].Color = Vertices[ 3 ].Color = Vertices[ 4 ].Color = Vertices[ 7 ].Color = D3DCOLOR_ARGB(0, 255, 255, 255);
		Vertices[ 8 ].Color = Vertices[ 11 ].Color = Vertices[ 12 ].Color = Vertices[ 13 ].Color = Vertices[ 14 ].Color = Vertices[ 15 ].Color = D3DCOLOR_ARGB(0, 255, 255, 255);

		unsigned short Indices[54] = {
			0, 5, 4, 0, 1, 5, 1, 2, 5, 6, 5, 2, 2, 3, 6, 7, 6, 3,
			4, 5, 8, 9, 8, 5, 5, 6, 9, 10, 9, 6, 6, 7, 10, 11, 10, 7,
			8, 9, 12, 13, 12, 9, 9, 10, 13, 14, 13, 10, 10, 15, 14, 10, 11, 15
		};
		GetEtDevice()->DrawIndexedPrimitiveUP( PT_TRIANGLELIST, 0, 16, 18, Indices, FMT_INDEX16, Vertices, sizeof( STextureDiffuseVertex2D ) );

		// 타겟 복구
		GetEtDevice()->RestoreRenderTarget();
		GetEtDevice()->RestoreDepthStencil();
		GetEtDevice()->EnableZ( bEnableZ );
		GetEtDevice()->EnableAlphaBlend( bAlphaEnable );
	}
}

void CDnRenderAvatarNew::SetFrontView()
{
	m_nRotateYaw = (m_nRotateYaw + (ROTATE_SCALE * 180) * (m_nRotateYaw >= 0 ? 1 : -1 ) ) / (ROTATE_SCALE*360) * (ROTATE_SCALE*360);
}

void CDnRenderAvatarNew::SetRearView()
{
	m_nRotateYaw = (m_nRotateYaw ) / (ROTATE_SCALE*360) * (ROTATE_SCALE*360) + (ROTATE_SCALE * 180) * (m_nRotateYaw >= 0 ? 1 : -1 ) ;
}

void CDnRenderAvatarNew::ZoomIn()
{
	if( m_nZoomLevel < 4 )
		++m_nZoomLevel;
}

void CDnRenderAvatarNew::ZoomOut()
{
	if( m_nZoomLevel > 0 )
		--m_nZoomLevel;
}

void CDnRenderAvatarNew::Panning( float fX, float fY )
{
	m_fPanX += fX / 5.0f;
	m_fPanY += fY / 5.0f;
}

EtTextureHandle CDnRenderAvatarNew::GetTempRenderTarget( int nWidth, int nHeight, EtFormat Format )
{
	for( int i = 0; i < (int)s_vecTempTarget.size(); ++i )
	{
		if( s_vecTempTarget[i].nWidth == nWidth &&
			s_vecTempTarget[i].nHeight == nHeight &&
			s_vecTempTarget[i].Format == Format )
		{
			return s_vecTempTarget[i].hTexture;
		}
	}

	sTempTargetInfo Info;
	Info.nWidth = nWidth;
	Info.nHeight = nHeight;
	Info.Format = Format;
	Info.hTexture = CEtTexture::CreateRenderTargetTexture( nWidth, nHeight, Format );
	s_vecTempTarget.push_back( Info );
	return Info.hTexture;
}

void CDnRenderAvatarNew::SetEtc( const char *szSkinName, const char *szAniName, const char *szActName )
{
	m_hEtc = (new CDnEtcObject)->GetMySmartPtr();
	m_hEtc->Initialize( szSkinName, szAniName, szActName );

	CheckType();

	CDnEtcObject * pObject = m_hEtc.GetPointer();
	if( NULL == pObject )
		return;

	m_hEtc->SetRTTRenderMode( true, this );
	m_hEtc->SetActionQueue( "Stand" );
	m_hEtc->Show( true );
	AddObject( pObject->GetObjectHandle() );
	SetRenderFrameCount( -1 );

	return;
}

void CDnRenderAvatarNew::ResetEtc( bool bInitCamera )
{
	if( m_hActor )
	{
		SAFE_RELEASE_SPTR( m_hEtc );
		m_eActorType = ActorEnum::None;
		SetRenderFrameCount( 0 );
	}

	if( bInitCamera )
		InitCameraControl();
}