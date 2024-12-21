#include "StdAfx.h"
#include "DnPassiveSkillSlotDlg.h"
#include "DnQuickSlotButton.h"
#include "DnActor.h"
#include "DnSkill.h"
#include "DnLocalPlayerActor.h"
#include "DnItemTask.h"
#include "DnPlayerActor.h"
#include "DnMainDlg.h"
#include "Boost/bind.hpp"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnPassiveSkillSlotDlg::CDnPassiveSkillSlotDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
	: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback )
{
	m_nCreateOrder = 0;
	m_fGap = 0.f;
}

CDnPassiveSkillSlotDlg::~CDnPassiveSkillSlotDlg(void)
{
}

void CDnPassiveSkillSlotDlg::Initialize(bool bShow)
{
	BaseClass::Initialize( CEtResourceMng::GetInstance().GetFullName( "PassiveSkillSlotDlg.ui" ).c_str(), bShow );	
	if( m_vecSlotButton.size() >= 2 ) {
		m_fGap = m_vecSlotButton[1]->GetUICoord().fX - m_vecSlotButton[0]->GetUICoord().fX;
	}
}

void CDnPassiveSkillSlotDlg::InitCustomControl( CEtUIControl *pControl )
{
	CDnQuickSlotButton *pQuickSlotButton = static_cast<CDnQuickSlotButton*>(pControl);
	pQuickSlotButton->SetSlotType( ST_SKILL );
	pQuickSlotButton->SetSlotIndex( (int)m_vecSlotButton.size() );
	/*float fScale = 0.75f;
	pQuickSlotButton->SetSize( pQuickSlotButton->GetUICoord().fWidth*fScale, pQuickSlotButton->GetUICoord().fHeight*fScale );*/
	pQuickSlotButton->Show( false );
	m_vecSlotButton.push_back( pQuickSlotButton );
	m_vecCreateOrder.push_back( -1 );
}

void CDnPassiveSkillSlotDlg::Process( float fElapsedTime )
{
	BaseClass::Process( fElapsedTime );

	static std::vector< int > VisibleIndices;
	VisibleIndices.clear();
	
	for( int i = 0; i < (int)m_vecSlotButton.size(); i++) {
		MIInventoryItem *pItem = m_vecSlotButton[i]->GetItem();
		if( !pItem || pItem->GetCoolTime() <= 0.f ) {
			m_vecSlotButton[i]->Show( false );
			m_vecSlotButton[i]->ResetSlot();
		}
		else if( m_vecSlotButton[i]->IsShow() ) {
			VisibleIndices.push_back( i );
		}
	}

	struct CompareFunc {
		const std::vector<int> m_vecOrder;
		CompareFunc( const std::vector<int> vecOrder) : m_vecOrder ( vecOrder ) {}
		bool operator () ( const int &lhs, const int &rhs) const
		{
			return m_vecOrder[lhs] < m_vecOrder[rhs];
		}
	};

	std::sort( VisibleIndices.begin(), VisibleIndices.end(), CompareFunc( m_vecCreateOrder ) );

	int xPos = 0;
	for each( int nIndex in VisibleIndices ) {
		SUICoord UICoord;
		GetDlgCoord(UICoord);
		float fPos = m_fGap*(xPos -(VisibleIndices.size()-1) * 0.5f ) + (UICoord.fWidth-m_fGap) * 0.5f ;
		float fSmoothPos = m_vecSlotButton[ nIndex ]->GetUICoord().fX + ( fPos - m_vecSlotButton[ nIndex ]->GetUICoord().fX ) * min(1.0f, 10.f * fElapsedTime);
		m_vecSlotButton[ nIndex ]->SetPosition( fSmoothPos, m_vecSlotButton[ nIndex ]->GetUICoord().fY );
		m_vecSlotButton[ nIndex ]->SetRegist( false );
		xPos++;
	}
}

void CDnPassiveSkillSlotDlg::AddPassiveSkill( DnSkillHandle hSkill )
{
	int i, nSize;

	if(IsExistSkill(hSkill->GetClassID())) // Rotha  추가하기전에 이미 올라가있는 스킬인지 동일한지 검사한후 같다면 이전에 돌고있는 스킬을 리셋해준다.
	{
		ResetSlotFromSkillClassID(hSkill->GetClassID());
	}

	nSize = (int)m_vecSlotButton.size();
	for( i = 0; i < nSize; i++) {
		if( !m_vecSlotButton[i]->IsShow() ) {
			m_vecSlotButton[i]->SetQuickItem( (MIInventoryItem*)hSkill.GetPointer() );
			SUICoord UICoord;
			GetDlgCoord(UICoord);
			m_vecSlotButton[i]->SetPosition( (UICoord.fWidth- m_fGap ) * 0.5f, m_vecSlotButton[i]->GetUICoord().fY );
			m_vecSlotButton[i]->Show( true );			
			m_vecCreateOrder[i] = m_nCreateOrder++;			
			break;
		}
	}

}

#if defined(PRE_FIX_61821)
void CDnPassiveSkillSlotDlg::ReplacePassiveSkill(DnSkillHandle hSkill)
{
	if (!hSkill)
		return;

	for( int i = 0; i < (int)m_vecSlotButton.size(); i++)
	{
		MIInventoryItem *pItem = m_vecSlotButton[i]->GetItem();

		if(pItem && pItem->GetType() == MIInventoryItem::Skill)
		{
			if(pItem->GetClassID() == hSkill->GetClassID())
			{
				m_vecSlotButton[i]->SetQuickItem((MIInventoryItem*)hSkill.GetPointer());
				break;
			}
		}
	}
}
#endif // PRE_FIX_61821

bool CDnPassiveSkillSlotDlg::IsExistSkill(int Skill_ID)
{
	for( int i = 0; i < (int)m_vecSlotButton.size(); i++) {
		MIInventoryItem *pItem = m_vecSlotButton[i]->GetItem();

		if(pItem && pItem->GetType() == MIInventoryItem::Skill)
		{
			if(pItem->GetClassID() == Skill_ID)
				return true;
		}
	}
	return false;
}

void CDnPassiveSkillSlotDlg::ResetSlotFromSkillClassID(int Skill_ID)
{
	for( int i = 0; i < (int)m_vecSlotButton.size(); i++) {
		MIInventoryItem *pItem = m_vecSlotButton[i]->GetItem();

		if(pItem && pItem->GetType() == MIInventoryItem::Skill)
		{
			if(pItem->GetClassID() == Skill_ID)
			{
				m_vecSlotButton[i]->Show( false );
				m_vecSlotButton[i]->ResetSlot();
			}
		}
	}
}