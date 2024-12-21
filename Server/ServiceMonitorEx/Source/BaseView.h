#pragma once

#include "Define.h"
#include "OXLayoutManager.h"


class CBaseView : public CFormView
{
	DECLARE_DYNAMIC(CBaseView)

public:
	explicit CBaseView(UINT nIDTemplate);
	virtual ~CBaseView();

public:
	void Set(INT pPageIndex) { ASSERT(0 <= pPageIndex); m_PageIndex = pPageIndex; }
	BOOL IsSet() const { return(EV_PAGEIDX_DEF != m_PageIndex); }

	INT GetPageIndex() const { return m_PageIndex; }
	UINT GetIdTemplate() const { return m_nIDTemplate; }

private:
	INT m_PageIndex;
	UINT m_nIDTemplate;
	
};
