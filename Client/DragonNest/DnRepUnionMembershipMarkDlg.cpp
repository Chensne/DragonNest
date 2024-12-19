#include "StdAfx.h"

#ifdef PRE_MOD_REPUTE_RENEW
#include "DnRepUnionMembershipMarkDlg.h"

const int NPC_UNION_ICON_TEXTURE_SIZE = 32;

CDnRepUnionMembershipMarkDlg::CDnRepUnionMembershipMarkDlg(UI_DIALOG_TYPE dialogType /* = UI_TYPE_FOCUS */, CEtUIDialog *pParentDialog /* = NULL */, 
													int nID /* = -1 */, CEtUICallback *pCallback /* = NULL */) :
													CEtUIDialog(dialogType, pParentDialog, nID, pCallback, true),
													m_pMark(NULL)
{
}

CDnRepUnionMembershipMarkDlg::~CDnRepUnionMembershipMarkDlg(void)
{
	SAFE_RELEASE_SPTR(m_hMarkTexture);
}

void CDnRepUnionMembershipMarkDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName("ReputeBuffMarkDlg.ui.ui").c_str(), bShow);
}

void CDnRepUnionMembershipMarkDlg::InitialUpdate()
{
	m_pMark = GetControl<CEtUITextureControl>("ID_TEXTUREL_MARK");

	SAFE_RELEASE_SPTR(m_hMarkTexture);
	m_hMarkTexture = LoadResource("Repute_SmallMark.dds", RT_TEXTURE, true);
}

void CDnRepUnionMembershipMarkDlg::SetUnionMark(int iconIdx)
{
	if (!m_hMarkTexture)
		return;

	m_pMark->SetTexture(m_hMarkTexture, (iconIdx % NPC_UNION_ICON_TEXTURE_SIZE) * NPC_UNION_ICON_TEXTURE_SIZE, (iconIdx / NPC_UNION_ICON_TEXTURE_SIZE) * NPC_UNION_ICON_TEXTURE_SIZE,
		NPC_UNION_ICON_TEXTURE_SIZE, NPC_UNION_ICON_TEXTURE_SIZE);
}
#endif // PRE_MOD_REPUTE_RENEW