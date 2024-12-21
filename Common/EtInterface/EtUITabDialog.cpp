#include "StdAfx.h"
#include "EtUITabDialog.h"
#include "EtUIRadioButton.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CEtUITabDialog::CEtUITabDialog( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback, bool bAutoCursor )
	: CEtUIDialog( dialogType, pParentDialog, nID, pCallback, bAutoCursor )
{
}

CEtUITabDialog::~CEtUITabDialog(void)
{
}

void CEtUITabDialog::AddTabDialog( CEtUIControl *pControl, CEtUIDialog *pDialog )
{
	ASSERT(pControl&&pDialog&&"CEtUITabDialog::AddTabDialog");

	if (pControl->GetType() != UI_CONTROL_RADIOBUTTON)
	{
		_ASSERT(0);
		return;
	}

	CEtUIRadioButton* pRadioBtn = static_cast<CEtUIRadioButton*>(pControl);

	int tabID = pRadioBtn->GetTabID();

#if defined (_DEBUG) || defined (_RDEBUG)
	std::vector< std::pair<DWORD, CEtUIControl*> >::const_iterator iter = m_vecTabControl.begin();
	for(;iter != m_vecTabControl.end(); ++iter)
	{
		const  std::pair<DWORD, CEtUIControl*> &id_ctl_pair = *iter;
		if (id_ctl_pair.first == tabID)
		{
			_ASSERT(0 && "TabDialog ID already exists!");
			break;
		}
	}
#endif

	m_vecTabControl.push_back( std::pair<DWORD, CEtUIControl*>(tabID , pControl) );
	m_groupTabDialog.AddDialog( static_cast<CEtUIRadioButton*>(pControl)->GetTabID(), pDialog );
}

void CEtUITabDialog::ChangeDialog( CEtUIControl * pControl, CEtUIDialog * pDialog )
{
	m_groupTabDialog.ChangeDialog( static_cast<CEtUIRadioButton*>(pControl)->GetTabID(), pDialog );
}

void CEtUITabDialog::SetCheckedTab( DWORD dwControlID )
{
	int nVecSize = (int)m_vecTabControl.size();

	for( int i=0; i<nVecSize; ++i )
	{
		std::pair<DWORD, CEtUIControl*> &id_ctl_pair = m_vecTabControl[i];

		if( id_ctl_pair.first == dwControlID )
		{
			static_cast<CEtUIRadioButton*>(id_ctl_pair.second)->SetChecked(true);
			return;
		}
	}
}

CEtUIRadioButton* CEtUITabDialog::GetTabControl(DWORD dwTabID)
{
	if (dwTabID < 0 || dwTabID >= (DWORD)m_vecTabControl.size())
		return NULL;

	int nVecSize = (int)m_vecTabControl.size();
	for( int i=0; i<nVecSize; ++i )
	{
		std::pair<DWORD, CEtUIControl*> &id_ctl_pair = m_vecTabControl[i];

		if( id_ctl_pair.first == dwTabID )
			return static_cast<CEtUIRadioButton*>(id_ctl_pair.second);
	}

	return NULL;
}

DWORD CEtUITabDialog::GetCurrentTabID()
{
	int nVecSize = (int)m_vecTabControl.size();

	for( int i=0; i<nVecSize; ++i )
	{
		std::pair<DWORD, CEtUIControl*> &id_ctl_pair = m_vecTabControl[i];

		if( static_cast<CEtUIRadioButton*>(id_ctl_pair.second)->IsChecked() )
		{
			return id_ctl_pair.first;
		}
	}

	return UINT_MAX;
}

const CEtUIRadioButton* CEtUITabDialog::GetCurrentTabControl() const
{
	int nVecSize = (int)m_vecTabControl.size();

	for( int i=0; i<nVecSize; ++i )
	{
		const std::pair<DWORD, CEtUIControl*> &id_ctl_pair = m_vecTabControl[i];
		const CEtUIRadioButton* pBtn = static_cast<const CEtUIRadioButton*>(id_ctl_pair.second);
		if (pBtn && pBtn->IsChecked())
			return pBtn;
	}

	return NULL;
}

void CEtUITabDialog::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	ASSERT(pControl&&"CEtUITabDialog::ProcessCommand, pControl is NULL!");

	if( nCommand == EVENT_RADIOBUTTON_CHANGED ) 
	{
		m_groupTabDialog.ShowDialog( static_cast<CEtUIRadioButton*>(pControl)->GetTabID(), true );
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CEtUITabDialog::Show( bool bShow )
{ 
	CEtUIDialog::Show( bShow );

	DWORD dwDlgID(UINT_MAX);

	if( bShow )
	{
		dwDlgID = GetCurrentTabID();
	}
	else
	{
		dwDlgID = m_groupTabDialog.GetShowDialogID();
	}

	if( dwDlgID != UINT_MAX )
	{
		m_groupTabDialog.ShowDialog( dwDlgID, bShow );
	}
}

void CEtUITabDialog::MoveDialog( float fX, float fY ) 
{
	CEtUIDialog::MoveDialog( fX, fY );
	m_groupTabDialog.MoveDialog( fX, fY);
}

int CEtUITabDialog::GetFrontTabID()
{
	if( m_vecTabControl.empty() ) {
		return -1;
	}
	return m_vecTabControl.front().first;
}
