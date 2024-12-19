#include "stdafx.h"
#include "EtUIDialog.h"
#include "DnActorPreview.h"
#include "DnPlayerActor.h"

#ifdef _CASHSHOP_UI

CDnActorPreview::CDnActorPreview()
{
	m_pParent		= NULL;
	m_pSurface		= NULL;
	m_nSessionID	= -1;
	m_fAniFrame		= 0.f;

	m_nAngle		= 0;
	m_fMouseX		= 0.f;
	m_fMouseY		= 0.f;
}

CDnActorPreview::~CDnActorPreview()
{

}

void CDnActorPreview::Initialize(CEtUIDialog* pParentDlg, const SUICoord& charViewCoord, float screenWidth, float screenHeight)
{
	if (pParentDlg == NULL)
	{
		_ASSERT(0);
		return;
	}

	m_pParent			= pParentDlg;
	m_CharViewCoord		= charViewCoord;
	m_ScreenWidth		= screenWidth;
	m_ScreenHeight		= screenHeight;

	DWORD dwWidth	= (DWORD)(m_CharViewCoord.fWidth * m_ScreenWidth);
	DWORD dwHeight	= (DWORD)(m_CharViewCoord.fHeight * m_ScreenWidth);

	m_hRTTexture	= CEtTexture::CreateRenderTargetTexture(dwWidth, dwHeight, FMT_A8R8G8B8);
	m_hRTDepth		= CEtDepth::CreateDepthStencil( dwWidth, dwHeight, FMT_D24S8, GetEtDevice()->GetSupportedAAType() );
	SAFE_RELEASE( m_pSurface );
	m_pSurface		= GetEtDevice()->CreateRenderTarget( dwWidth, dwHeight, FMT_A8R8G8B8, POOL_DEFAULT, GetEtDevice()->GetSupportedAAType() );
}

void CDnActorPreview::Show(bool bShow)
{
	if (m_bShow == bShow)
		return;

	m_bShow = bShow;

	if (bShow == false)
	{
		if (m_hActor)
		{
			CDnPlayerActor *pPlayerActor = dynamic_cast<CDnPlayerActor*>(m_hActor.GetPointer());
			if (pPlayerActor)
				pPlayerActor->SetBakePreview(false, this);
		}
		else
		{
			DnActorHandle hActor = CDnActor::FindActorFromUniqueID(m_nSessionID);
			CDnPlayerActor *pPlayerActor = dynamic_cast<CDnPlayerActor*>(hActor.GetPointer());
			if (pPlayerActor)
				pPlayerActor->SetBakePreview(false, this);
		}
		m_nSessionID = -1;
		SAFE_RELEASE_SPTR( m_hActor );
	}
}

void CDnActorPreview::Render(float fElapsedTime)
{
	if (m_bShow && m_pParent)
		m_pParent->DrawSprite(m_hRTTexture, SUICoord(0,0,1,1) , 0xffffffff, m_CharViewCoord);
}

void CDnActorPreview::SetActor(DnActorHandle hActor)
{
	m_hActor = hActor;
}

void CDnActorPreview::CloneActor(DnActorHandle hOriginal)
{
	if (hOriginal == NULL)
	{
		_ASSERT(0);
		return;
	}
/*
	DNTableFileFormat *pActorSOX = GetDNTable( CDnTableDB::TACTOR );
	char *szLabelList[CDnParts::PartsTypeEnum_Amount] = { "_Face", "_Hair", "_Helmet", "_Body", "_Leg", "_Hand", "_Foot", NULL, NULL, NULL, NULL };
	char szTemp[128];

	int classID = hOriginal->GetClassID();
	DnActorHandle hActor = CreateActor(classID, false, true);
	((CDnPlayerActor*)hActor.GetPointer())->SetJobHistory(classID);
	hActor->ShowHeadName( false );
	MAPartsBody *pPartsBody = dynamic_cast<MAPartsBody *>(hActor.GetPointer());

	char *pStaticName = pActorSOX->GetFieldFromLablePtr(classID, "_StaticName")->GetString();
	char *szDummySkinName = pActorSOX->GetFieldFromLablePtr(classID, "_SkinName")->GetString();

	sprintf_s( szTemp, "%s\\%s_login.ani", pStaticName, pStaticName );
	pPartsBody->LoadSkin( CEtResourceMng::GetInstance().GetFullName( szDummySkinName ).c_str(), CEtResourceMng::GetInstance().GetFullName( szTemp ).c_str() );
	sprintf_s( szTemp, "%s\\%s_login.act", pStaticName, pStaticName );
	hActor->LoadAction( CEtResourceMng::GetInstance().GetFullName( szTemp ).c_str() );
	hActor->Initialize();

	hActor->GetObjectHandle()->SetCalcPositionFlag(CALC_POSITION_X|CALC_POSITION_Y|CALC_POSITION_Z);
	CCrossVector Cross;
	Cross = m_TreeOffset;

	hActor->SetPosition( Cross.m_PosVector );
	hActor->Look( EtVec3toVec2( Cross.m_ZVector ) );
	hActor->SetLevel(1);
	hActor->SetHP( hActor->GetMaxHP() );

	DefaultPartsStruct Struct;
	for( int k=0; k<CDnParts::PartsTypeEnum_Amount; k++ ) {
		if( szLabelList[k] == NULL ) continue;
		for( int j=0; j<10; j++ ) {
			sprintf_s( szLabel, "%s%d", szLabelList[k], j + 1 );
			int nTableID = pSox->GetFieldFromLablePtr( i, szLabel )->GetInteger();
			if( nTableID <= 0 ) continue;

			Struct.nVecList[k].push_back( nTableID );
			Struct.hVecParts[k].push_back( CDnParts::CreateParts( nTableID, 0 ) );
		}
		Struct.nSelectPartsIndex[k] = ( Struct.nVecList[k].empty() ) ? -1 : 0;
	}
	Struct.fHairColor[0] = Struct.fHairColor[1] = Struct.fHairColor[2] = 1.f;

	hActor->SetActionQueue( "CreateSelect_Cancle_Idle" );

	for( int j=0; j<2; j++ ) {
		sprintf_s( szLabel, "_Weapon%d", j + 1 );
		int nWeaponIndex = pSox->GetFieldFromLablePtr( i, szLabel )->GetInteger();
		if( nWeaponIndex != -1 ) {
			Struct.hWeapon[j] = CDnWeapon::CreateWeapon( nWeaponIndex, 0 );
			hActor->AttachWeapon( Struct.hWeapon[j], j, false );
		}
		Struct.nWeapon[j] = nWeaponIndex;
	}

	m_VecCreateDefaultPartsList.push_back( Struct );
	m_hVecCreateActorList.push_back( hActor );*/
}

void CDnActorPreview::BakeInspectView(CDnPlayerActor *pPlayerActor, float fElapsedTime)
{
	EtAniObjectHandle hObject =  pPlayerActor->GetObjectHandle();
	if( !hObject ) return;

	EtSurface *pBackupRenderTarget = GetEtDevice()->GetRenderTarget();
	EtSurface *pBackupDepthTarget = GetEtDevice()->GetDepthStencilSurface();

	EtViewPort BackupViewport;
	GetEtDevice()->GetViewport(&BackupViewport);
	GetEtDevice()->SetRenderTarget(m_pSurface);
	GetEtDevice()->SetDepthStencilSurface(m_hRTDepth->GetDepthBuffer());
	bool bZEnable = GetEtDevice()->EnableZ(true);
	GetEtDevice()->ClearBuffer( 0x0, 1.0f, 0, true, true, false);	
	EtViewPort viewport;
	viewport.X = 0;
	viewport.Y = 0;
	viewport.Width = (DWORD)(m_CharViewCoord.fWidth * m_ScreenWidth);
	viewport.Height = (DWORD)(m_CharViewCoord.fHeight * m_ScreenHeight);
	viewport.MinZ = 0;
	viewport.MaxZ = 1;
	GetEtDevice()->SetViewport(&viewport);

	// Light Setting
	EtColor AmbientColor = EternityEngine::GetGlobalAmbient();
	EternityEngine::SetGlobalAmbient( &EtColor(1.0f,1.0f, 1.15f, 1.f));
	EtLightHandle hLight;
	if( CEtLight::GetItemCount() == 0 ) {
		SLightInfo LightInfo;
		LightInfo.Direction = EtVector3( -0.143f, -0.806f, 0.574f );
		hLight = EternityEngine::CreateLight(&LightInfo);
	}
	// Camera Setting

	SCameraInfo CameraInfo;
	CameraInfo.fWidth = (float)viewport.Width;
	CameraInfo.fHeight = (float)viewport.Height;
	CameraInfo.Target = CT_RENDERTARGET_NO_GENERATE_BACKBUFFER;
	CameraInfo.fNear = 10.f;
	CameraInfo.fFar = 500.f;

	CameraInfo.Type = CT_ORTHOGONAL;

	static float fAspect = (float)DEFAULT_UI_SCREEN_HEIGHT / DEFAULT_UI_SCREEN_WIDTH;
	CameraInfo.fViewWidth = fAspect * 115.f;
	CameraInfo.fViewHeight = 115.f;

	EtCameraHandle hCamera = EternityEngine::CreateCamera( &CameraInfo );

	EtMatrix CamMat, ProjMat;
	EtMatrixTranslation(&CamMat, 0, 60.f, -200.f);
	hCamera->Update(&CamMat);

	EtCameraHandle hCameraBackup = CEtCamera::GetActiveCamera();
	CEtCamera::SetActiveCamera( hCamera.GetIndex() );		

	static std::vector< EtObjectHandle > s_vecRenderObjects;	// avoid memory allocation
	s_vecRenderObjects.clear();	

	bool bBattleMode = pPlayerActor->IsBattleMode();
	if( bBattleMode ) {
		pPlayerActor->SetBattleMode( false );
	}

	EtMatrix WorldMat;
	EtMatrixRotationY(&WorldMat, ET_PI + EtToRadian( m_nAngle*0.1f ) );	
	hObject->Update( &WorldMat );

	m_fAniFrame += fElapsedTime*60.f;	

	int nActionIndex = pPlayerActor->GetElementIndex("Normal_Stand");	
	if( nActionIndex == -1 ) nActionIndex = pPlayerActor->GetElementIndex("Stand");	
	int nAniIndex = pPlayerActor->GetAniIndex( pPlayerActor->GetElement( nActionIndex )->szLinkAniName.c_str() );
	hObject->SetAniFrame( nAniIndex, fmodf(m_fAniFrame, (float) hObject->GetAniLength( nAniIndex ) ) );
	hObject->CalcAni();

	if( hObject->GetSkin()->GetMeshHandle()->GetSubMeshCount() > 0 ) {
		s_vecRenderObjects.push_back( hObject );
	}
	for( int j = 0; j < (int)hObject->GetChild().size(); j++ ) {
		EtObjectHandle hChildObject = hObject->GetChild()[j];
		if( !hChildObject->IsShow() ) continue;
		if( hChildObject->GetSkin()->GetMeshHandle()->GetSubMeshCount() > 0 ) {
			s_vecRenderObjects.push_back( hChildObject );
			CEtAniObject *pAniObj = dynamic_cast<CEtAniObject*>(hChildObject.GetPointer());
			if( pAniObj ) {
				pAniObj->ResetAniFrame();
			}
		}
	}

	for( int i = 0; i < 2; i++) {
		if( pPlayerActor->GetWeapon(i) ) {
			int nWeaponActionIndex = pPlayerActor->GetWeapon(i)->GetElementIndex("Normal_Stand");
			if( nWeaponActionIndex == -1 ) nWeaponActionIndex = pPlayerActor->GetWeapon(i)->GetElementIndex("NormalStand");
			if( nWeaponActionIndex == -1 ) nWeaponActionIndex = pPlayerActor->GetWeapon(i)->GetElementIndex("Stand");
			if( nWeaponActionIndex == -1 ) nWeaponActionIndex = pPlayerActor->GetWeapon(i)->GetElementIndex("Idle");

			if( nWeaponActionIndex != -1 && pPlayerActor->GetWeapon(i)->GetElement( nWeaponActionIndex ) ) {
				int nWeaponAniIndex = pPlayerActor->GetWeapon(i)->GetAniIndex( pPlayerActor->GetWeapon(i)->GetElement( nWeaponActionIndex )->szLinkAniName.c_str() );
				if( nWeaponAniIndex != -1) {
					pPlayerActor->GetWeapon(i)->GetObjectHandle()->SetAniFrame( nWeaponAniIndex, fmodf(m_fAniFrame, (float) pPlayerActor->GetWeapon(i)->GetObjectHandle()->GetAniLength( nWeaponAniIndex ) ) );
					pPlayerActor->GetWeapon(i)->GetObjectHandle()->CalcAni();			
				}
			}
		}
	}

	if( hObject->IsShow() ) {
		hObject->InitRender();
	}
	else {
		hObject->ShowObject( true );
		hObject->InitRender();
		hObject->ShowObject( false );
	}

	bool bOldAlphaBlend = GetEtDevice()->EnableAlphaBlend( false );
	for each( EtObjectHandle hRenderObj in s_vecRenderObjects ) {
		for( int i = 0; i < hRenderObj->GetSkin()->GetMeshHandle()->GetSubMeshCount(); i++) {
			if( !hRenderObj->IsShowSubmesh( i ) ) continue;
			if( hRenderObj->GetSkinInstance()->GetCustomParam().empty() ) continue;
			EtMaterialHandle hMaterial = hRenderObj->GetSkin()->GetMaterialHandle(i);
			int nTechniqueIndex = (hRenderObj->GetSkin()->GetMeshHandle()->GetSubMesh(i)->GetLinkCount() == 0 ) ? 0 : 1;
			int nSaveMatIndex = -1;
			if( nTechniqueIndex != 0 ) {
				nSaveMatIndex = hRenderObj->GetSaveMatIndex();
				if( nSaveMatIndex == -1 ) continue;
			}
			CEtLight::SetDirLightAttenuation( 1.f );
			hMaterial->SetTechnique( nTechniqueIndex );
			int nPasses = 0;
			hMaterial->BeginEffect( nPasses );
			hMaterial->BeginPass( 0 );
			hMaterial->SetGlobalParams();
			if( nTechniqueIndex != 0 ) {
				hMaterial->SetWorldMatArray( hRenderObj->GetWorldMat(), nSaveMatIndex, 
					hRenderObj->GetSkin()->GetMeshHandle()->GetSubMesh(i)->GetLinkCount(), hRenderObj->GetSkin()->GetMeshHandle()->GetSubMesh(i)->GetLinkIndex() );
			}
			hMaterial->SetWorldMatParams( hRenderObj->GetWorldMat(), hRenderObj->GetWorldMat() );
			hMaterial->SetCustomParamList( hRenderObj->GetSkinInstance()->GetCustomParam()[i] );
			hMaterial->CommitChanges();
			hRenderObj->GetSkin()->GetMeshHandle()->GetSubMesh(i)->GetMeshStream()->Draw( hMaterial->GetVertexDeclIndex( nTechniqueIndex, 0 ) );
			hMaterial->EndPass();
			hMaterial->EndEffect();
		}
	}

	GetEtDevice()->EnableAlphaBlend( bOldAlphaBlend );
	///////////////////////////////////////////////////////////////////////////////////////
	GetEtDevice()->SetViewport( &BackupViewport );
	GetEtDevice()->SetRenderTarget( pBackupRenderTarget );
	GetEtDevice()->SetDepthStencilSurface( pBackupDepthTarget );		
	if( hCameraBackup ) {
		CEtCamera::SetActiveCamera( hCameraBackup.GetIndex() );
	}
	SAFE_RELEASE_SPTR( hCamera );

	GetEtDevice()->DumpBackBuffer( m_hRTTexture->GetSurfaceLevel(), m_pSurface);
	// º¹±¸
	if( bBattleMode ) {		
		pPlayerActor->SetBattleMode( true );
	}
	EternityEngine::SetGlobalAmbient( &AmbientColor);
	if( !bZEnable ) {
		GetEtDevice()->EnableZ( false );
	}
}

void CDnActorPreview::RefreshItemSlots()
{
	/*
	if( m_hActor ) return;

	for( int i = 0; i < (int)m_vecSlotButton.size(); i++) {
		m_vecSlotButton[ i ]->ResetSlot();
		m_vecSlotButton[ i ]->OnRefreshTooltip();
	}

	DnActorHandle hActor = CDnActor::FindActorFromUniqueID( m_nSessionID );
	if( !hActor || hActor->IsDestroy() ) {
		m_pStaticName->SetText( L"" );
		m_pStaticLevel->SetText( L"" );
		m_pStaticJob->SetText( L"" );
		m_pPVPIconTexCon->Show(false);
		m_pPVPRankNameStCon->SetText( L"" );
		return;
	}
	wchar_t wszTemp[256]={0};
	m_pStaticName->SetText( hActor->GetName() );
	swprintf_s( wszTemp, 256, L"%s %d", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 64 ), hActor->GetLevel() );
	m_pStaticLevel->SetText( wszTemp );	
	m_pStaticJob->SetText( DN_INTERFACE::STRING::GetClassString( hActor->GetClassID() ));

	m_vecEquipInfo.clear();
	SEquipInfo Info;

	MAPartsBody *pPartsBody = dynamic_cast<MAPartsBody*>(hActor.GetPointer());
	for( int nIndex = CDnParts::Face; nIndex < CDnParts::PartsTypeEnum_Amount; nIndex++) {
		DnPartsHandle hParts = pPartsBody->GetParts( (CDnParts::PartsTypeEnum)nIndex);
		if( !hParts ) continue;
		CDnItem *pItem = (CDnItem *)hParts.GetPointer();
		CDnPlayerActor *pDnPlayerActor = dynamic_cast<CDnPlayerActor*>(hActor.GetPointer());
		if( pDnPlayerActor->IsNudeParts( hParts ) ) continue;
		int nSlotIndex = CDnCharStatusDlg::EQUIPINDEX_2_SLOTINDEX( nIndex );
		if( nSlotIndex != -1 ) {
#ifdef _SPLIT_SLOT_COUNT
			m_vecSlotButton[nSlotIndex]->SetItem( pItem, CDnSlotButton::ITEM_ORIGINAL_COUNT );
#else
			m_vecSlotButton[nSlotIndex]->SetItem( pItem );
#endif
			m_vecSlotButton[nSlotIndex]->OnRefreshTooltip();
		}
	}

	for( int nIndex = CDnParts::CashHelmet; nIndex < CDnParts::CashPartsTypeEnum_Amount; nIndex++) {	
		DnPartsHandle hParts = pPartsBody->GetCashParts( (CDnParts::PartsTypeEnum)nIndex );
		if( !hParts ) continue;
		CDnItem *pItem = (CDnItem *)hParts.GetPointer();
		CDnPlayerActor *pDnPlayerActor = dynamic_cast<CDnPlayerActor*>(hActor.GetPointer());
		if( pDnPlayerActor->IsNudeParts( hParts ) ) continue;
		int nSlotIndex = CDnCharStatusDlg::CASH_EQUIPINDEX_2_SLOTINDEX( nIndex );
		if( nSlotIndex != -1 ) {
#ifdef _SPLIT_SLOT_COUNT
			m_vecCashSlotButton[ nSlotIndex ]->SetItem( pItem, CDnSlotButton::ITEM_ORIGINAL_COUNT );
#else
			m_vecCashSlotButton[ nSlotIndex ]->SetItem( pItem );
#endif
			m_vecCashSlotButton[ nSlotIndex ]->OnRefreshTooltip();
		}
		Info.nClassID = pItem->GetClassID();
		Info.nSetItemID = hParts->GetSetItemID();
		m_vecEquipInfo.push_back( Info );
	}

	for( int i = 0; i < 2; i++) {
		if( hActor->GetWeapon( i ) ) {
			CDnWeapon *pWeapon = hActor->GetWeapon( i ).GetPointer();
			int nSlotIndex = CDnCharStatusDlg::EQUIPINDEX_2_SLOTINDEX( CDnCharStatusDlg::EQUIPTYPE_2_EQUIPINDEX( pWeapon->GetEquipType() ) );
			if( nSlotIndex != -1 ) {
				m_vecSlotButton[ nSlotIndex ]->ResetSlot();
				m_vecSlotButton[ nSlotIndex ]->OnRefreshTooltip();
#ifdef _SPLIT_SLOT_COUNT
				m_vecSlotButton[ nSlotIndex ]->SetItem((CDnItem*)pWeapon, CDnSlotButton::ITEM_ORIGINAL_COUNT);
#else
				m_vecSlotButton[ nSlotIndex ]->SetItem( (CDnItem*)pWeapon );
#endif
				m_vecSlotButton[ nSlotIndex ]->OnRefreshTooltip();		
				Info.nClassID = pWeapon->GetClassID();
				Info.nSetItemID = pWeapon->GetSetItemID();
				m_vecEquipInfo.push_back( Info );
			}
		}
	}

	for( int i = 0; i < 2; i++) {
		CDnPlayerActor *pActor = dynamic_cast<CDnPlayerActor*>(hActor.GetPointer());
		if( pActor && pActor->GetCashWeapon( i ) ) {
		CDnWeapon *pWeapon = pActor->GetCashWeapon( i ).GetPointer();
		int nSlotIndex = CDnCharStatusDlg::CASH_EQUIPINDEX_2_SLOTINDEX( CDnCharStatusDlg::CASH_EQUIPTYPE_2_EQUIPINDEX( pWeapon->GetEquipType() ) );
		if( nSlotIndex != -1 ) {
		m_vecCashSlotButton[ nSlotIndex ]->ResetSlot();
		m_vecCashSlotButton[ nSlotIndex ]->OnRefreshTooltip();
		#ifdef _SPLIT_SLOT_COUNT
		m_vecCashSlotButton[ nSlotIndex ]->SetItem((CDnItem*)pWeapon, CDnSlotButton::ITEM_ORIGINAL_COUNT);
		#else
		m_vecCashSlotButton[ nSlotIndex ]->SetItem( (CDnItem*)pWeapon );
		#endif
		m_vecCashSlotButton[ nSlotIndex ]->OnRefreshTooltip();		
		Info.nClassID = pWeapon->GetClassID();
		Info.nSetItemID = pWeapon->GetSetItemID();
		m_vecEquipInfo.push_back( Info );
		}
		}
	}
	*/
}

#endif