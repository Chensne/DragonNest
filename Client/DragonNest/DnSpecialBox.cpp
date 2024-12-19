#include "StdAfx.h"
#include "DnSpecialBox.h"
#include "TradeSendPacket.h"
#include "DnInterface.h"
#include "DnMainMenuDlg.h"
#include "DnMainDlg.h"
#include "DnEventReceiverTabDlg.h"
#include "DnSpecialBoxListDlg.h"

#if defined(PRE_SPECIALBOX) 

CDnSpecialBox::CDnSpecialBox(void)
{
}

CDnSpecialBox::~CDnSpecialBox(void)
{
}

// Receive Func
void CDnSpecialBox::OnRecvSpecialBoxList( SCSpecialBoxList *pPacket )
{
	if( pPacket && pPacket->nRet == ERROR_NONE )
	{
		CDnMainMenuDlg *pMainMenuDlg = GetInterface().GetMainMenuDialog();
		if( pMainMenuDlg )
		{
			CDnEventReceiverTabDlg *pEventReceiverTabDlg = static_cast<CDnEventReceiverTabDlg*>( pMainMenuDlg->GetMainDialog( CDnMainMenuDlg::EVENT_RECEIVER_DIALOG ) );
			if( pEventReceiverTabDlg )
			{
				pEventReceiverTabDlg->ClearSpecialBoxInfo();

				for( int i=0; i<pPacket->cCount; i++ )
				{
					pEventReceiverTabDlg->AddSpecialBoxInfo( &pPacket->BoxInfo[i] );
				}

				pEventReceiverTabDlg->RefreshSpecialList();
			}

			pMainMenuDlg->ToggleShowDialog( CDnMainMenuDlg::EVENT_RECEIVER_DIALOG );
		}
	}
	else
	{
		GetInterface().ServerMessageBox( pPacket->nRet );
	}
}

void CDnSpecialBox::OnRecvSpecialBoxItemList( SCSpecialBoxItemList *pPacket )
{
	if( pPacket && pPacket->nRet == ERROR_NONE )
	{
		CDnMainMenuDlg *pMainMenuDlg = GetInterface().GetMainMenuDialog();
		if( pMainMenuDlg )
		{
			CDnEventReceiverTabDlg *pEventReceiverTabDlg = static_cast<CDnEventReceiverTabDlg*>( pMainMenuDlg->GetMainDialog( CDnMainMenuDlg::EVENT_RECEIVER_DIALOG ) );
			if( pEventReceiverTabDlg )
			{
				pEventReceiverTabDlg->GetSpecialListDlg()->SetSpecialBoxListItem( pPacket );
				pEventReceiverTabDlg->GetSpecialListDlg()->Show( true );
			}
		}
	}
	else
	{
		GetInterface().ServerMessageBox( pPacket->nRet );
	}
}

void CDnSpecialBox::OnRecvSpecialBoxReceiveItem( SCReceiveSpecialBoxItem *pPacket )
{
	if( pPacket && pPacket->nRet == ERROR_NONE )
	{
		CDnMainMenuDlg *pMainMenuDlg = GetInterface().GetMainMenuDialog();
		if( pMainMenuDlg )
		{
			CDnEventReceiverTabDlg *pEventReceiverTabDlg = static_cast<CDnEventReceiverTabDlg*>( pMainMenuDlg->GetMainDialog( CDnMainMenuDlg::EVENT_RECEIVER_DIALOG ) );
			if( pEventReceiverTabDlg )
			{
				pEventReceiverTabDlg->GetSpecialListDlg()->OnCompleteReceiveItem();
			}
		}
	}
	else
	{
		GetInterface().ServerMessageBox( pPacket->nRet );
	}
}

void CDnSpecialBox::OnRecvSpecialBoxNotify( SCNotifySpecialBox *pPacket )
{
	if( pPacket )
	{
		if( GetInterface().GetMainBarDialog() ) 
			GetInterface().GetMainBarDialog()->SetEventReceiverAlarm( pPacket->bNew );
	}
}

#endif