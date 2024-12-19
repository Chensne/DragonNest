#include "Stdafx.h"
#include "SlangFilter.h"

CSlangFilter::CSlangFilter()
{
	memset(&m_Root, 0, sizeof(m_Root));
}


CSlangFilter::~CSlangFilter()
{
	std::vector <TCHAR*>::iterator ii;
	for (ii = m_MemList.begin(); ii != m_MemList.end(); ii++)
		CLfhHeap::GetInstance()->_DeAlloc((*ii));
}


void CSlangFilter::InsertSlang(const TCHAR *words, const TCHAR * replacewords)
{
	_WORD * tree = &m_Root, * child;
	for (int i=0; words[i] != NULL; i++)
	{
		child = Find(tree, words[i]);
		if (child == NULL)
			child = InsertTree(tree, words[i]);
		tree = child;
	}

	tree->tail = true;

	if (replacewords != NULL)
	{
		tree->replaceword = static_cast<TCHAR*>(CLfhHeap::GetInstance()->_Alloc((int)_tcslen(replacewords)+1));
		_tcscpy(tree->replaceword, replacewords);

		m_MemList.push_back(tree->replaceword);
	}	else
		tree->replaceword = NULL;
}


int CSlangFilter::ReplaceSlang(const TCHAR *src, TCHAR *out, int outlen, int type)
{
	int i, match, j=0, k;
	const TCHAR * rep ;
#ifdef _DEBUG
	int len = (int) _tcslen(src);
#endif
	for(i=0; src[i] && j+1 < outlen;)
	{
		match = MatchSlang(&src[i], &rep);

#ifdef _DEBUG
		_ASSERT(i+match<=len);
#endif

		if (match > 0)
		{
			if (rep != NULL)
			{
				int strl = (int) _tcslen(rep);
				if (strl + j + 1 < outlen)
				{
					memcpy(&out[j], rep, strl*sizeof(TCHAR));
					j += strl;
				}
			}
			else
			{
				for(k=0; k<match;)
				{
					if (outlen > j+1)
						out[j++] = '*';
#ifdef UNICODE
					k += 1;
#else
					k += (src[i+k]&0x80) ? 2 : 1;
#endif
				}
				_ASSERT(match == k);
			}
			i+=match;
		}
		else
		{
#ifdef UNICODE
			//허용문자 체크는 다른걸 사용한다.
#else
			if ((src[i] & 0x80) != NULL) //한글일 경우
			{
				if (src[i+1] == '\0')
					break;	// 만약을 대비해서....(이런 문자열이 넘어온다면 곤란)

				if (((unsigned char)src[i+1] >= 0xA1 && (unsigned char)src[i+1] <= 0xFE) &&
					(((unsigned char)src[i] >= 0xB0 && (unsigned char)src[i] <= 0xC8) ||	// 한글
					(type&FILTER_IRREGULAR_HANAGUL) == 0 ||
					//						((unsigned char)src[i] >= 0xCA && (unsigned char)src[i] <= 0xFD)) // 한자
					//						((type&FILTER_INCOMPLETION_HANAGUL) == 0 && ((unsigned char)src[i] >= 0xA4 && (unsigned char)src[i] <= 0xAC)))) // 낯자
					((type&FILTER_INCOMPLETION_HANAGUL) == 0 && (unsigned char)src[i] == 0xA4))) // 낯자
				{
					if (outlen > j+2)
					{
						out[j++] = src[i];
						out[j++] = src[i+1];
					}
				}	else
					out[j++] = '*'; //정상한글이아님
				i += 2;
			}
			else //한글이 아닐때!!!
#endif
			{
				if (outlen > j+1)
					out[j++] = src[i++];
			}
		}
	}
	_ASSERT(j<outlen);
	out[j] = 0;
	return j;
}

CSlangFilter::_WORD * CSlangFilter::Find(_WORD * tree, TCHAR code) const
{
	_WORD * t;
	t = tree->child[code&15];
	for(; t; t=t->next)
		if (t->code == code)
			break;
	return t;
}


int CSlangFilter::MatchSlang(const TCHAR * szText, const TCHAR ** replace_str) const
{
	const _WORD * t = &m_Root;
	TCHAR code;
	int i;
	int ret = 0;
	for(i=0; szText[i];)
	{
		code = szText[i];
		for(t = t->child[code&15]; t && !_tcsicmp(&t->code, &code); t = t->next);
		if (t == NULL)
			return ret;
		i++;
		if (t->tail == true)
		{
			*replace_str = t->replaceword;
			ret = i;
		}
	}

	return ret;
}


CSlangFilter::_WORD * CSlangFilter::InsertTree(_WORD * tree, TCHAR code)
{
	_WORD * ch = static_cast <_WORD*> (CLfhHeap::GetInstance()->_Alloc(sizeof(_WORD)));
	memset(ch, 0, sizeof(_WORD));
	ch->code = code;
	ch->next = tree->child[code&15];
	tree->child[code&15] = ch;

	return ch;
}