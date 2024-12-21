
#pragma once

#ifdef _US
#define MAIN_URL	L"http://dragonnest.nexon.net/ending"
#elif _RU
#define MAIN_URL	L"http://dn.mail.ru/static/dn.mail.ru/client/exit.html"
#endif

enum eDlgPosIndex
{
	eDPI_MainWebHtml,
	eDPI_SubWebHtml,
	eDPI_CloseBtn,
	eDPI_MAX
};

struct SCtrlRect
{
	CRect ctrlRect[eDPI_MAX];

	SCtrlRect()
	{
		ctrlRect[eDPI_MainWebHtml].SetRect(0, 0, 740, 300);
		ctrlRect[eDPI_SubWebHtml].SetRect(430, 40, 730, 290);
		ctrlRect[eDPI_CloseBtn].SetRect(711, 7, 736, 32);
	}

	CRect& GetCtrlRect(eDlgPosIndex idx)
	{
		if (idx < 0 || idx >= eDPI_MAX)
		{
			_ASSERT(0);
			return ctrlRect[0];
		}

		return ctrlRect[idx];
	}
};
