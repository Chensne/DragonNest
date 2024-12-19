
#pragma once

#include "MemPool.h"

class CSlangFilter
{
public :
	CSlangFilter();
	~CSlangFilter();

	enum {
		FILTER_NONE = 0,
		FILTER_IRREGULAR_HANAGUL = 1,
		FILTER_INCOMPLETION_HANAGUL = 2,
	} ;

	void InsertSlang(const TCHAR *words, const TCHAR * replacewords=NULL);
	int ReplaceSlang(const TCHAR *src, TCHAR *out, int outlen, int type=FILTER_NONE);

private :
	struct _WORD {
		TCHAR code;
		bool tail;
		TCHAR * replaceword;

		_WORD * next;
		_WORD * child[16];
	} ;

	_WORD * InsertTree(_WORD * tree, TCHAR code);
	int MatchSlang(const TCHAR * szText, const TCHAR ** replace_str) const;
	_WORD * Find(_WORD * tree, TCHAR code) const;

	_WORD m_Root;
	std::vector <TCHAR*> m_MemList;
} ;