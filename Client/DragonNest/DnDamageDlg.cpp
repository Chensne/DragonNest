#include "StdAfx.h"
#include "DnDamageDlg.h"
#include "DnCamera.h"
#include "VelocityFunc.h"
#include "DnActor.h"
#include "DnWorld.h"
#include "DnInterface.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnDamageDlg::CDnDamageDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
	: CDnCountDlg( dialogType, pParentDialog, nID, pCallback  )
	, m_fXGap(0.0f)
	, m_fYGap(0.0f)
	, m_fIncrese(0.0f)
	, m_fVelocity(0.0f)
	, m_fValue(0.0f)
	, m_fTotalElapsedTime(0.0f)
	, m_DamageType(CDnDamageCount::Damage_Normal)
	, m_fDistanceFromCamera(0.f)
	, m_ColorType(CT_NONE)
	, m_isMyDamage( false )
{	
}

CDnDamageDlg::~CDnDamageDlg(void)
{
}

void CDnDamageDlg::Initialize( bool bShow )
{
	if( !m_pCountCtl )
	{
		CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "DamageDlg.ui" ).c_str(), bShow );
	}
	else
	if (!m_hTexture)
	{
		//#42010 - 데미지 UI가 화면에 한동안 보여지지 않는 경우 
		//CEtUIDialog::Process()함수에서 CEtUIDialog::ProcessMemoryOptimize()가 호출되어 m_hTexture가 해제되는 경우가 발생됨.
		//m_hTexture가 해제 된 경우 다시 초기화 하도록한다..
		//CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "DamageDlg.ui" ).c_str(), bShow );
		//FreeDialogTexture()로 텍스쳐만 해제 되는 경우라 LoadDialogTexture()만 호출 하도록 변경
		LoadDialogTexture();
	}
	InitialUpdate();
	m_bShow = bShow;

	m_isMyDamage = false;
}

void CDnDamageDlg::InitialUpdate()
{	
	bool bUseCustomColor = true;
	switch( m_ColorType ) {
		case CT_NONE :
			m_pCountCtl = GetControl<CDnDamageCount>("ID_WHITE");
			bUseCustomColor = false;
			break;
		case CT_PARTYATK :
			m_pCountCtl = GetControl<CDnDamageCount>("ID_PARTYATK");
			break;
		case CT_MYATK :
			m_pCountCtl = GetControl<CDnDamageCount>("ID_MYATK");
			break;
		case CT_MYCRITICAL :
			m_pCountCtl = GetControl<CDnDamageCount>("ID_MYCRITICAL");
			break;
		case CT_MYDAMAGE :
			m_pCountCtl = GetControl<CDnDamageCount>("ID_MYDAMAGE");
			break;
		case CT_MYMANA : 
			m_pCountCtl = GetControl<CDnDamageCount>("ID_MYMANA");
			break;
		case CT_MYHEAL :
			m_pCountCtl = GetControl<CDnDamageCount>("ID_MYHEAL");
			break;
		case CT_PARTYHEAL : 
			m_pCountCtl = GetControl<CDnDamageCount>("ID_PARTYHEAL");
			break;
		case CT_PARTYDAMAGE :
			m_pCountCtl = GetControl<CDnDamageCount>("ID_PARTYDAMAGE");
			break;
		case CT_BLUE1 :
			m_pCountCtl = GetControl<CDnDamageCount>("ID_BLUE1");
			break;
		case CT_RED1 :
			m_pCountCtl = GetControl<CDnDamageCount>("ID_RED1");
			break;
		case CT_FIRE:
			m_pCountCtl = GetControl<CDnDamageCount>("ID_FIRE");
			break;
		case CT_WATER:
			m_pCountCtl = GetControl<CDnDamageCount>("ID_WATER");
			break;
		case CT_LIGHT:
			m_pCountCtl = GetControl<CDnDamageCount>("ID_LIGHT");
			break;
		case CT_DARK:
			m_pCountCtl = GetControl<CDnDamageCount>("ID_DARK");
			break;
		case CT_STAGELIMIT:
			m_pCountCtl = GetControl<CDnDamageCount>("ID_LIMIT");
			break;
		default:
			_ASSERT( false );
			break;
	}

	if( m_pCountCtl ) {
		m_pCountCtl->Show(false);
		m_pCountCtl->Init();
		m_pCountCtl->UseCustomColor( bUseCustomColor );
	}
}

void CDnDamageDlg::Process( float fElapsedTime )
{
	if( m_fDelayTime > 0.0f )
	{
		m_fTotalElapsedTime += fElapsedTime;

		if( m_DamageType == CDnDamageCount::Damage_Normal )
		{
			float fCurFrame = m_fTotalElapsedTime*100.0f;

			if (m_hAni)
			{
				if( m_hAni->GetAniLength( 0 ) < m_fTotalElapsedTime*70.f )
				{
					m_fDelayTime = 0.0f;
				}
				else
				{
					m_hAni->CalcAniDistance( 0, fCurFrame, 0.0f, m_vAniPos );
					UpdateCount();
				}
			}
			else
			{
				_ASSERT(0);
				m_fDelayTime = 0.0f;
			}
		}
		else
		{
			m_fDelayTime -= fElapsedTime;
			UpdateDamagePos();

			SUICoord dlgCoord;
			GetDlgCoord( dlgCoord );
			dlgCoord.SetPosition( m_vDamagePos.x - m_fXGap, m_vDamagePos.y );
			SetDlgCoord( dlgCoord );
		}
	}

	CEtUIDialog::Process( fElapsedTime );
}

void CDnDamageDlg::Render( float fElapsedTime )
{
	if( GetInterface().IsOpenBlind() )
		return;

	CEtUIDialog::Render( fElapsedTime );
}

void CDnDamageDlg::SetDamage( EtVector3 vPos, int nDamage, EtAniHandle hAni, float fFontSize )
{
	m_pCountCtl->SetValue( nDamage );
	m_pCountCtl->SetFontSize( fFontSize );		
#ifdef _RDEBUG
	g_Log.Log(LogType::_FILELOG, "[DMGERR] CDnDamageDlg::SetDamage()\n");
#endif
	SetPosition( vPos );
	m_DamageType = CDnDamageCount::Damage_Normal;
	m_hAni = hAni;
}

void CDnDamageDlg::SetPlus()
{
	m_pCountCtl->SetPlus(true);
}

void CDnDamageDlg::SetChangeFont()
{
	m_pCountCtl->ChangeFont(true);
}

void CDnDamageDlg::SetCritical( EtVector3 vPos )
{
	((CDnDamageCount*)m_pCountCtl)->SetCritical();	
	m_pCountCtl->SetFontSize(1.0f);
	SetPosition( vPos );
	m_DamageType = CDnDamageCount::Damage_Critical;
}

void CDnDamageDlg::SetResist( EtVector3 vPos )
{
	((CDnDamageCount*)m_pCountCtl)->SetResist();	
	m_pCountCtl->SetFontSize( 1.0f);
	SetPosition( vPos );
	m_DamageType = CDnDamageCount::Damage_Resist;
}

void CDnDamageDlg::SetCriticalRes( EtVector3 vPos )
{
	((CDnDamageCount*)m_pCountCtl)->SetCriticalRes();	
	m_pCountCtl->SetFontSize( 1.0f);
	SetPosition( vPos );
	m_DamageType = CDnDamageCount::Damage_CriticalRes;
}

#ifdef PRE_ADD_DECREASE_EFFECT
void CDnDamageDlg::SetDecreaseEffect( EtVector3 vPos )
{
	((CDnDamageCount*)m_pCountCtl)->SetDecreaseEffect();	
	m_pCountCtl->SetFontSize( 1.0f);
	SetPosition( vPos );
	m_DamageType = CDnDamageCount::Damage_DecreaseEffect;
}
#endif // PRE_ADD_DECREASE_EFFECT

static EtVector3 s_vPrevPos(0,0,0);
static int s_nDupCount = 0;
void CDnDamageDlg::SetPosition( EtVector3 vPos )
{
	m_fTotalElapsedTime = 0.0f;
	m_fIncrese = 0.0f;
	m_fVelocity = 0.0f;
	m_fDelayTime = 0.8f;
	m_fValue = 0.0033f;

#ifdef _RDEBUG
	g_Log.Log(LogType::_FILELOG, "[DMGERR] CDnDamageDlg::SetPosition() : vPos(%f %f %f) / s_vPrevPos(%f %f %f) / s_nDupCount(%d)\n", vPos.x, vPos.y, vPos.z, s_vPrevPos.x, s_vPrevPos.y, s_vPrevPos.z, s_nDupCount);
#endif

	if( EtVec3LengthSq(&(s_vPrevPos-vPos)) <= 0.0001f ) {		
		s_nDupCount++;
		m_vActorPos = vPos + EtVector3( -8.f + 16.f * (s_nDupCount&1), 12.f * s_nDupCount, -8.f + 16.f * (s_nDupCount&2));
	}
	else {
		s_vPrevPos = vPos;
		m_vActorPos = vPos;
		s_nDupCount = 0;
	}
	m_vAniPos.y = 0.0f;

	UpdateDamagePos();

	if( CDnCamera::GetActiveCamera() )
	{
		EtVector3 vDist = m_vActorPos - CDnCamera::GetActiveCamera()->GetMatEx()->GetPosition();
		m_fDistanceFromCamera = EtVec3Length( &vDist );		
	}

	SUICoord dlgCoord;
	GetDlgCoord( dlgCoord );
	dlgCoord.SetSize( m_pCountCtl->GetControlWidth(), m_pCountCtl->GetControlHeight() );
	m_fXGap = (dlgCoord.fWidth * 0.5f );
	dlgCoord.SetPosition( m_vDamagePos.x - m_fXGap, m_vDamagePos.y );
	SetDlgCoord( dlgCoord );
}

void CDnDamageDlg::UpdateCount()
{
	UpdateDamagePos();

	SUICoord dlgCoord;
	GetDlgCoord( dlgCoord );

	float fScale = 1.f;
	BYTE cAlpha = 255;
	if( m_fTotalElapsedTime < 0.5f ) {
		fScale = 1.f + (0.5f-m_fTotalElapsedTime)*2.f;
		cAlpha = (BYTE)min(255, 55 + (200 * (m_fTotalElapsedTime / 0.5f)) );
	}

	m_pCountCtl->SetAlpha( cAlpha );

	m_fXGap = (dlgCoord.fWidth * 0.5f) * fScale;
	m_pCountCtl->SetScale( fScale ) ;

	dlgCoord.SetPosition( m_vDamagePos.x - m_fXGap, m_vDamagePos.y - m_fIncrese );	
	SetDlgCoord( dlgCoord );
}

void CDnDamageDlg::UpdateDamagePos()
{
	const EtVector3 *pPlayerPos(NULL);

	DnActorHandle hPlayer = CDnActor::s_hLocalActor;
	if( hPlayer )	pPlayerPos = hPlayer->GetPosition();
	else			pPlayerPos = CDnWorld::GetInstance().GetUpdatePosition();

	if( !pPlayerPos )
	{
		m_fDelayTime = 0.0f;
		m_pCountCtl->Show( false );
		Show( false );
		return;
	}

	EtVector3 v = m_vActorPos - (*pPlayerPos);
	float fDist = EtVec3Length(&v);
	if( fDist > 1000.0f ) 
	{
		HideDamage();
		return;
	}

	EtMatrix matViewProj;
	DnCameraHandle hCamera = CDnCamera::GetActiveCamera();
	if( hCamera )
	{
		matViewProj = *hCamera->GetCameraHandle()->GetViewProjMat();
	}
	else 
	{
		EtCameraHandle hCam = CEtCamera::GetActiveCamera();
		if( !hCam )
		{
			HideDamage();
			return;
		}

		matViewProj = *CEtCamera::GetActiveCamera()->GetViewProjMat();
	}

	EtVector3 vTemp;
	vTemp = m_vActorPos;
	EtVec3TransformCoord( &m_vDamagePos, &vTemp, &matViewProj );
	float fScaleY = 130.f * min(1.7f, max(1.2f, 1200.f / m_fDistanceFromCamera )); // 구지 카메라 거리에 따라 올라가는 속도를 조절할 필요가 있나 싶습니다만..
	
	m_vDamagePos.y += m_vAniPos.y / fScaleY;


	if( m_vDamagePos.z >= 1.0f )
	{
		HideDamage();
		return;
	}

	//자신의 데미지는 아래로 떨어지도록..
	static float fStartPosY = 0.4f;
	static float fScaleTemp = 0.7f;
	if (m_isMyDamage)
	{	
		m_vDamagePos.x = (m_vDamagePos.x*0.5f)+0.5f;
		m_vDamagePos.y = -(m_vDamagePos.y* fScaleTemp) + fStartPosY;
	}
	else
	{
		m_vDamagePos.x = (m_vDamagePos.x*0.5f)+0.5f;
		m_vDamagePos.y = (m_vDamagePos.y*0.5f)+0.5f;
		m_vDamagePos.y = 1.0f - m_vDamagePos.y;
	}
}

void CDnDamageDlg::HideDamage()
{
	m_fDelayTime = 0.0f;
	m_pCountCtl->Show( false );
	Show( false );
	m_bShow = false;
}

void CDnDamageDlg::Show( bool bShow )
{
	CDnCountDlg::ShowCount( bShow );

	// 가만히 멈춰있는 235339 형식의 몹같은 경우 데미지 수치가 중복된다고 판단되어 
	// SetPosition() 함수에서 계속 위로 올라가서 찍히게 되므로
	// 이전 데미지 수치가 사라질 때 중복 카운트를 없애주도록 한다.
	if( false == bShow )
	{
		--s_nDupCount;
		if( s_nDupCount < 0 )
			s_nDupCount = 0;
	}
}

void CDnDamageDlg::SetHasElement(int hasElement)
{
	if (m_pCountCtl)
	{
		((CDnDamageCount*)m_pCountCtl)->SetHasElement(hasElement);
	}
}
