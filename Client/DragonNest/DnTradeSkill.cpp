#include "StdAfx.h"
#include "DnTradeSkill.h"
#include "TradeSendPacket.h"
#include "DnSkillStoreTabDlg.h"
#include "DnSkill.h"
#include "DnActor.h"
#include "DnInterface.h"
#include "DnSkillTask.h"
#include "DnInCodeResource.h"
#include "DnNpcActor.h"
#include "DnLocalPlayerActor.h"


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnTradeSkill::CDnTradeSkill(void)
	: m_pSkillStoreDialog(NULL)
{
}

CDnTradeSkill::~CDnTradeSkill(void)
{
}

void CDnTradeSkill::OnRecvOpenSkillShop( SCSkillShopOpen* pPacket )
{
	DWORD nUID = CDnLocalPlayerActor::GetTakeNpcUID();
	DnActorHandle hNpc = CDnActor::FindActorFromUniqueID(nUID);

	if ( hNpc ) {
		CDnNPCActor* pActor = dynamic_cast<CDnNPCActor*>(hNpc.GetPointer());
		if ( pActor ) {
			pActor->OnSoundPlay( CDnNPCActor::OpenShop );
		}
	}
	GetInterface().OpenSkillStoreDialog();
}
