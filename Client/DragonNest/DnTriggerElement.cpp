#include "StdAfx.h"
#include "DnTriggerElement.h"
#include "DnMainFrame.h"
#include "DNClientScriptAPI.h"
#include "DnTrigger.h"
#include "DnTriggerObject.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnTriggerElement::CDnTriggerElement( CEtTriggerObject *pObject )
: CEtTriggerElement( pObject, ((CDnTrigger*)pObject->GetTrigger())->GetLuaState() )
{
}

CDnTriggerElement::~CDnTriggerElement()
{
}

void CDnTriggerElement::RegisterLuaApi()
{
	if( !m_bSelfOpenLua && ((CDnTrigger*)m_pTriggerObj->GetTrigger())->IsRegisterLuaAPI() ) return;
	if( !m_bSelfOpenLua )
		((CDnTrigger*)m_pTriggerObj->GetTrigger())->SetRegisterLuaAPI( true );

	CEtTriggerElement::RegisterLuaApi();
	DefAllAPIFunc( m_pLuaState );

	lua_tinker::set<CDnMainFrame*>( m_pLuaState, "g_room", &CDnMainFrame::GetInstance() );
	lua_tinker::set<CEtTriggerElement*>( m_pLuaState, "g_element", this );
}

bool CDnTriggerElement::CallLuaFunction()
{
	if( !m_bSelfOpenLua ) {
		if( m_pLuaState ) lua_tinker::set<CEtTriggerElement*>( m_pLuaState, "g_element", this );
	}

	return CEtTriggerElement::CallLuaFunction();
}