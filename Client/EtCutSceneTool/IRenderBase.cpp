#include "StdAfx.h"
#include "EternityEngine.h"
#include "IRenderBase.h"

#include "Timer.h"
#include "EtCamera.h"
#include "RTCutSceneRenderer.h"

IRenderBase::IRenderBase(void)
{

}

IRenderBase::~IRenderBase(void)
{

}


IRenderBase* IRenderBase::Create( HWND hWnd )
{
	return new CRTCutSceneRenderer( hWnd );
}