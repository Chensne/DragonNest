#include "StdAfx.h"
#include "EtUIDialogGroup.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CEtUIDialogGroup::CEtUIDialogGroup(void)
	: m_pShowDialog(NULL)
	, m_dwShowDialogID(UINT_MAX)
{
}

CEtUIDialogGroup::~CEtUIDialogGroup(void)
{
	int nVecSize = (int)m_vecDialog.size();

	for( int i=0; i<nVecSize; ++i )
	{
		ID_DIALOG_PAIR &id_dlg_pair = m_vecDialog[i];
		SAFE_DELETE(id_dlg_pair.second);
	}

	m_vecDialog.clear();
}

void CEtUIDialogGroup::SetDialog( DWORD dwDialogID, CEtUIDialog *pDialog )
{
	ASSERT(pDialog&&"CEtUIDialogGroup::SetDialog, pDialog is NULL!");

	m_vecDialog.push_back( ID_DIALOG_PAIR( dwDialogID, pDialog ) );
	pDialog->Show( false );
}

void CEtUIDialogGroup::AddDialog( DWORD dwDialogID, CEtUIDialog *pDialog )
{
	ASSERT(pDialog&&"CEtUIDialogGroup::AddDialog, pDialog is NULL!");
	m_vecDialog.push_back( ID_DIALOG_PAIR( dwDialogID, pDialog ) );
}

bool CEtUIDialogGroup::ChangeDialog( DWORD dwDialogID, CEtUIDialog *pDialog )
{
	ASSERT(pDialog&&"CEtUIDialogGroup::ChangeDialog, pDialog is NULL!");

	int nVecSize = (int)m_vecDialog.size();

	for( int i=0; i<nVecSize; ++i )
	{
		ID_DIALOG_PAIR &id_dlg_pair = m_vecDialog[i];

		if( id_dlg_pair.first == dwDialogID )
		{
			id_dlg_pair.second = pDialog;
			return true;
		}
	}

	return false;
}

bool CEtUIDialogGroup::DelDialog( DWORD dwDialogID )
{
	int nVecSize = (int)m_vecDialog.size();

	for( int i=0; i<nVecSize; ++i )
	{
		ID_DIALOG_PAIR &id_dlg_pair = m_vecDialog[i];

		if( id_dlg_pair.first == dwDialogID )
		{
			SAFE_DELETE(id_dlg_pair.second);
			m_vecDialog.erase( m_vecDialog.begin()+i );
			return true;
		}
	}

	return false;
}

bool CEtUIDialogGroup::IsShowDialog( DWORD dwDialogID ) const
{
	int nVecSize = (int)m_vecDialog.size();
	for( int i=0; i<nVecSize; ++i )
	{
		const ID_DIALOG_PAIR &id_dlg_pair = m_vecDialog[i];

		if( id_dlg_pair.first == dwDialogID && id_dlg_pair.second )
		{
			return id_dlg_pair.second->IsShow();
		}
	}

	return false;
}

void CEtUIDialogGroup::ShowDialog( DWORD dwDialogID, bool bShow, bool bCloseAll )
{
	CEtUIDialog *pDialog(NULL);
	CEtUIDialog *pParentDialog(NULL);

	int nVecSize = (int)m_vecDialog.size();
	for( int i=0; i<nVecSize; ++i )
	{
		ID_DIALOG_PAIR &id_dlg_pair = m_vecDialog[i];

		if( id_dlg_pair.first == dwDialogID )
		{
			pDialog = id_dlg_pair.second;
		}
		else if( bCloseAll )
		{
			pParentDialog = id_dlg_pair.second->GetParentDialog();
			if( pParentDialog )
			{
				pParentDialog->ShowChildDialog( id_dlg_pair.second, false );
			}
			else
			{
				id_dlg_pair.second->Show( false );
			}
		}
	}

	if( pDialog )
	{
		pParentDialog = pDialog->GetParentDialog();
		if( pParentDialog )
		{
			pParentDialog->ShowChildDialog( pDialog, bShow );
		}
		else
		{
			pDialog->Show( bShow );
		}

		if( bShow )
		{
			m_dwShowDialogID = dwDialogID;
			m_pShowDialog = pDialog;
		}
		else
		{
			m_dwShowDialogID = UINT_MAX;
			m_pShowDialog = NULL;
		}
	}
}

CEtUIDialog* CEtUIDialogGroup::GetDialog( DWORD dwDialogID )
{
	int nVecSize = (int)m_vecDialog.size();

	for( int i=0; i<nVecSize; ++i )
	{
		ID_DIALOG_PAIR &id_dlg_pair = m_vecDialog[i];

		if( id_dlg_pair.first == dwDialogID )
		{
			return id_dlg_pair.second;
		}
	}

	return NULL;
}

void CEtUIDialogGroup::CloseAllDialog()
{
	CEtUIDialog *pDialog(NULL);

	int nVecSize = (int)m_vecDialog.size();
	for( int i=0; i<nVecSize; ++i )
	{
		ID_DIALOG_PAIR &id_dlg_pair = m_vecDialog[i];
		id_dlg_pair.second->Show(false);
	}

	m_dwShowDialogID = UINT_MAX;
	m_pShowDialog = NULL;
}

CEtUIDialog* CEtUIDialogGroup::GetDialogFromIndex( int nIndex )
{
	ID_DIALOG_PAIR &id_dlg_pair = m_vecDialog[nIndex];
	return id_dlg_pair.second;
}

void CEtUIDialogGroup::MoveDialog( float fX, float fY )
{
	int nVecSize = (int)m_vecDialog.size();
	for( int i=0; i<nVecSize; ++i ) {
		m_vecDialog[i].second->MoveDialog( fX, fY );
	}
}
