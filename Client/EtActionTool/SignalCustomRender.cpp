#include "StdAfx.h"
#include "SignalCustomRender.h"
#include "ActionSignal.h"
#include "SCRHit.h"
#include "SCRSound.h"
#include "SCRParticle.h"
#include "SCREnvironmentEffect.h"
#include "SCRProjectile.h"
#include "SCRFX.h"
#include "SCRDecal.h"
#include "SCRJump.h"
#include "SCRVelocity.h"
#include "SCRAlphaBlend.h"
#include "SCRShaderCustomParameter.h"
#include "SCRMoveY.h"
#include "CSCRRotate.h" // #56216
#include "SCRMarkProjectile.h"
#include "SCGravity.h"


std::vector<CSignalCustomRender *> CSignalCustomRender::s_pVecList;

void CSignalCustomRender::RegisterClass()
{
	s_pVecList.push_back( new CSCRHit("Hit") );
	s_pVecList.push_back( new CSCRSound("Sound") );
	s_pVecList.push_back( new CSCRParticle("Particle") );
	s_pVecList.push_back( new CSCREnvironmentEffect("EnvironmentEffect") );
	s_pVecList.push_back( new CSCRProjectile("Projectile") );
	s_pVecList.push_back( new CSCRFX("FX") );
	s_pVecList.push_back( new CSCRDecal("Decal") );
	s_pVecList.push_back( new CSCRJump("Jump") );
	s_pVecList.push_back( new CSCRVelocity("VelocityAccel") );
	s_pVecList.push_back( new CSCRAlphaBlend("AlphaBlending") );
	s_pVecList.push_back( new CSCRShaderCustomParameter("ShaderCustomParameter") );
	s_pVecList.push_back( new CSCRMoveY( "MoveY" ) );
	s_pVecList.push_back( new CSCRRotate( "Rotate" ) ); // #56216
	s_pVecList.push_back( new CSCRMarkProjectile( "MarkProjectile" ) );
	s_pVecList.push_back( new CSCRGravity( "Gravity" ) );
}

CSignalCustomRender *CSignalCustomRender::AllocCustomRender( const char *szSignalName )
{
	for( DWORD i=0; i<s_pVecList.size(); i++ ) {
		if( strcmp( s_pVecList[i]->GetSignalName(), szSignalName ) == NULL ) return s_pVecList[i]->Clone();
	}
	return NULL;
}

void CSignalCustomRender::UnRegisterClass()
{
	SAFE_DELETE_PVEC( s_pVecList );
}

bool CSignalCustomRender::CheckSignal( float fStart, float fEnd )
{
	if( fEnd >= (float)m_pSignal->GetStartFrame() && fEnd < (float)m_pSignal->GetEndFrame() && 
		( fStart < (float)m_pSignal->GetStartFrame() || fStart >= (float)m_pSignal->GetEndFrame() ) ) {
			return true;
	}

	if( (float)m_pSignal->GetStartFrame() >= fStart && (float)m_pSignal->GetStartFrame() < fEnd && 
		(float)m_pSignal->GetEndFrame() >= fStart && (float)m_pSignal->GetEndFrame() < fEnd ) {
			return true;
	}

	if( (int)fStart != (int)fEnd && (int)fEnd >= m_pSignal->GetStartFrame() && (int)fEnd < m_pSignal->GetEndFrame() ) {
		return true;
	}
	if( fStart == fEnd && (int)fEnd >= m_pSignal->GetStartFrame() && (int)fEnd < m_pSignal->GetEndFrame()  ) {
		return true;
	}

	return false;
}


// #56216.
void CSignalCustomRender::CheckStartEndSignal( float curFrame )
{
	// 종료.
	if( (float)m_pSignal->GetEndFrame() <= curFrame )
	{
		OnEndSignal();
		return;
	}

	// 시작.
	if( (float)m_pSignal->GetStartFrame() <= curFrame )
	{
		OnStartSignal();
		return;
	}

	
}