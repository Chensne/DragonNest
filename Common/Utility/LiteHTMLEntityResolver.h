#pragma once
#include "LiteHTMLCommon.h"

#pragma warning(push, 4)

class CLiteHTMLEntityResolver
{
// Constructors
public:
	CLiteHTMLEntityResolver()
	{
	}

// Operations
public:
	static UINT resolveEntity(LPCWSTR lpszEntity, wchar_t &chSubst)
	{
		ASSERT(lpszEntity);
		//ASSERT(AfxIsValidString(lpszEntity));

		if( m_CharEntityRefs.empty() )
		{
			InitEntityRefs();
		}

		LPCWSTR	lpszBegin = lpszEntity, 
				lpszEnd = ::wcschr(lpszEntity, L';');
		wchar_t	chTemp = 0;
		
		// entity references always end with a semi-colon ';'
		if (lpszEnd == NULL)
			return (0);

		// skip leading white-space characters
		while (::iswspace(*lpszBegin))
			//lpszBegin = ::_tcsinc(lpszBegin);
			lpszBegin++;
		
		// remaining string (including semi-colon) 
		// must be at least 4 characters in length
		if (lpszEnd - lpszBegin < 3)
			return (0U);

		// entity references always begin with an ampersand '&' symbol
		if (*lpszBegin != L'&')
			return (0U);

		//lpszBegin = ::_tcsinc(lpszBegin);
		lpszBegin++;

		// numeric (decimal or hexadecimal) entity reference?
		if (*lpszBegin == L'#')
		{
			//lpszBegin = ::_tcsinc(lpszBegin);
			lpszBegin++;
			chTemp = *lpszBegin;
			int	radix = (::iswdigit(chTemp) ? 10 : 
						(chTemp == L'x' || 
							chTemp == L'X' ? 16 : 0));
			if (radix)
			{
				if (radix == 16)
					//lpszBegin = ::_tcsinc(lpszBegin);
					lpszBegin++;
				
				unsigned long	ulNum = ::wcstoul(lpszBegin, NULL, radix);
				chSubst = (wchar_t)ulNum;
				//lpszEnd = ::_tcsinc(lpszEnd);
				lpszEnd++;
				return UINT(lpszEnd - lpszEntity);
			}
		}

		// character entity reference?
		else
		{
			CStringW	strKey(lpszBegin, UINT(lpszEnd - lpszBegin));

			// because some character entity references are 
			// case-sensitive, we must fix them manually
			if (!strKey.CompareNoCase(L"eth") || 
				!strKey.CompareNoCase(L"thorn"))
			{
				if (::_istupper(strKey[0]))
					strKey.MakeUpper();
				else
					strKey.MakeLower();
			}
			else if (!strKey.CompareNoCase(L"Oslash"))
			{
				strKey.MakeLower();
				strKey.SetAt(0, L'O');
			}
			else if (!strKey.CompareNoCase(L"AElig"))
			{
				strKey.MakeLower();
				strKey.SetAt(0, L'A');
				strKey.SetAt(1, L'E');
			}
			else
			{
				CStringW	strT = strKey.Mid(1);
				strKey.MakeLower();
				if (strT.CompareNoCase(L"grave") == 0 || 
					strT.CompareNoCase(L"acute") == 0 || 
					strT.CompareNoCase(L"circ")  == 0 || 
					strT.CompareNoCase(L"uml")   == 0 || 
					strT.CompareNoCase(L"tilde") == 0 || 
					strT.CompareNoCase(L"cedil") == 0 || 
					strT.CompareNoCase(L"ring")  == 0)
				{
					strKey.SetAt(0, strT[0]);
				}
			}
			
			// is this a known entity reference?
			ENTITYREFS_MAP_ITER iter = m_CharEntityRefs.find(strKey);
			if( iter != m_CharEntityRefs.end() )
			{
				chSubst = iter->second;
				//lpszEnd = ::_tcsinc(lpszEnd);
				lpszEnd++;
				return UINT(lpszEnd - lpszEntity);
			}
		}

		return (0U);
	}

// Data Members
private:
	static ENTITYREFS_MAP	m_CharEntityRefs;

public:
	static void InitEntityRefs();
};
#pragma warning(pop)