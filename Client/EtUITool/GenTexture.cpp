#include "Stdafx.h"
#include "GenTexture.h"

#include "UIToolTemplate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

struct STemplateTexture
{
	EtTextureHandle hTexture;
	int nIndex1;
	int nIndex2;
	SUICoord UVCoord;
};

bool TemplateTextureSortFunc( STemplateTexture Obj1, STemplateTexture Obj2 )
{
	int nWidth1 = 0, nWidth2 = 0, nHeight1 = 0, nHeight2 = 0;

	if( Obj1.hTexture )
	{
		nWidth1 = Obj1.hTexture->Width();
		nHeight1 = Obj1.hTexture->Height();
	}
	if( Obj2.hTexture )
	{
		nWidth2 = Obj2.hTexture->Width();
		nHeight2 = Obj2.hTexture->Height();
	}

	if( nWidth1 > nWidth2 )
	{
		return true;
	}
	else if( nWidth1 == nWidth2 )
	{
		if( nHeight1 > nHeight2 )
		{
			return true;
		}
	}

	return false;
}

bool EmptySpaceSortFunc( SEmptySpaceTexGen Obj1, SEmptySpaceTexGen Obj2 )
{
	if( Obj1.GetSpace() < Obj2.GetSpace() )
	{
		return true;
	}
	return false;
}

bool ArrangeTexture( std::vector< STemplateTexture > &vecTemplateTexture, std::vector< SEmptySpaceTexGen > &vecTexLayout, int &nWidth, int &nHeight )
{
	int i, j;

	std::vector< SEmptySpaceTexGen > vecEmptySpace;
	SEmptySpaceTexGen CurEmptySpace;

	CurEmptySpace.Set( 0, 0, nWidth, nHeight );
	vecEmptySpace.push_back( CurEmptySpace );
	vecTexLayout.resize( vecTemplateTexture.size() );
	for( i = 0; i < ( int )vecTemplateTexture.size(); i++ )
	{
		int nSameTexIndex;
		int nTexWidth, nTexHeight;

		if( ( !vecTemplateTexture[ i ].hTexture ) || ( vecTemplateTexture[ i ].hTexture->GetTexturePtr() == NULL ) )
		{
			SEmptySpaceTexGen TempEmptySpace;

			TempEmptySpace.Set( 0, 0, 0, 0 );
			vecTexLayout[ i ] = TempEmptySpace;
			continue;
		}

		nSameTexIndex = -1;
		for( j = 0; j < i; j++ )
		{
			if( vecTemplateTexture[ i ].hTexture == vecTemplateTexture[ j ].hTexture )
			{
				nSameTexIndex = j;
			}
		}

		if( nSameTexIndex == -1 )
		{
			float fBestSpaceRatio = 0.0f;
			int nBestFitIndex = -1;
			nTexWidth = vecTemplateTexture[ i ].hTexture->Width() + 2;
			nTexHeight = vecTemplateTexture[ i ].hTexture->Height() + 2;
			for( j = 0; j < ( int )vecEmptySpace.size(); j++ )
			{
				if( ( nTexWidth <= vecEmptySpace[ j ].nWidth ) && ( nTexHeight <= vecEmptySpace[ j ].nHeight ) )
				{
					float fRatio;

					fRatio = nTexWidth * nTexHeight / ( float )vecEmptySpace[ j ].nWidth * vecEmptySpace[ j ].nHeight;
					if( fRatio > fBestSpaceRatio )
					{
						fRatio = fBestSpaceRatio;
						nBestFitIndex = j;
					}
				}
			}
			if( nBestFitIndex != -1 )
			{
				SEmptySpaceTexGen Temp1, Temp2;
				int nModWidth, nModHeight;

				nModWidth = vecEmptySpace[ nBestFitIndex ].nWidth - nTexWidth;
				nModHeight = vecEmptySpace[ nBestFitIndex ].nHeight - nTexHeight;
				if( nModWidth >= nModHeight )
				{
					Temp1.nX = vecEmptySpace[ nBestFitIndex ].nX + nTexWidth;
					Temp1.nWidth = vecEmptySpace[ nBestFitIndex ].nWidth - nTexWidth;
					Temp1.nY = vecEmptySpace[ nBestFitIndex ].nY;
					Temp1.nHeight = vecEmptySpace[ nBestFitIndex ].nHeight;

					Temp2.nX = vecEmptySpace[ nBestFitIndex ].nX;
					Temp2.nWidth = nTexWidth;
					Temp2.nY = vecEmptySpace[ nBestFitIndex ].nY + nTexHeight;
					Temp2.nHeight = vecEmptySpace[ nBestFitIndex ].nHeight - nTexHeight;
				}
				else
				{
					Temp1.nX = vecEmptySpace[ nBestFitIndex ].nX + nTexWidth;
					Temp1.nWidth = vecEmptySpace[ nBestFitIndex ].nWidth - nTexWidth;
					Temp1.nY = vecEmptySpace[ nBestFitIndex ].nY;
					Temp1.nHeight = nTexHeight;

					Temp2.nX = vecEmptySpace[ nBestFitIndex ].nX;
					Temp2.nWidth = vecEmptySpace[ nBestFitIndex ].nWidth;
					Temp2.nY = vecEmptySpace[ nBestFitIndex ].nY + nTexHeight;
					Temp2.nHeight = vecEmptySpace[ nBestFitIndex ].nHeight - nTexHeight;
				}

				CurEmptySpace = vecEmptySpace[ nBestFitIndex ];
				vecEmptySpace.erase( vecEmptySpace.begin() + nBestFitIndex );
				if( ( Temp1.nWidth > 2 ) && ( Temp1.nHeight > 2 ) )
				{
					vecEmptySpace.push_back( Temp1 );
				}
				if( ( Temp2.nWidth > 2 ) && ( Temp2.nHeight > 2 ) )
				{
					vecEmptySpace.push_back( Temp2 );
				}
				std::sort( vecEmptySpace.begin(), vecEmptySpace.end(), EmptySpaceSortFunc );
			}
			else // 빈공간 없다.. 리턴하자..
			{
				return false;
			}
		}

		if( nSameTexIndex == -1 )
		{
			vecTexLayout[ i ] = CurEmptySpace;
		}
		else
		{
			vecTexLayout[ i ] = vecTexLayout[ nSameTexIndex ];
		}
	}

	int nMaxY = 0;

	// 이 아래 체크에 버그 있음.
	// Height를 고려하지 않아서 50x29짜리 2개를 64x64에 넣어야하는데, 64x32텍스처로 잘라버림.
	//for( i = 0; i < ( int )vecEmptySpace.size(); i++ )
	//{
	//	if( vecEmptySpace[ i ].nY > nMaxY )
	//	{
	//		nMaxY = vecEmptySpace[ i ].nY;
	//	}
	//}
	// 검증이 제대로 안되서 바로 커밋하기 좀 그렇다. -> 아무래도 필요한거 같아서 커밋.
	for( i = 0; i < ( int )vecTemplateTexture.size(); i++ )
	{
		if( vecTemplateTexture[ i ].hTexture )
		{
			int nTexHeight = vecTemplateTexture[ i ].hTexture->Height() + 2;
			if( vecTexLayout[ i ].nY + nTexHeight > nMaxY )
			{
				nMaxY = vecTexLayout[ i ].nY + nTexHeight;
			}
		}
	}

	if( nMaxY != 0 )
	{
		while( nHeight / 2 >= nMaxY )
		{
			nHeight /= 2;
			if( nHeight == 0 )
			{
				return true;
			}
		}
	}

	return true;
}

int g_anOffsetX[] = { -1, 1, -1, 1, -1, 1, 0, 0, 0 };
int g_anOffsetY[] = { -1, -1, 1, 1, 0, 0, -1, 1, 0 };
void MakeTexture( EtTextureHandle hArrangeTexture, std::vector< CUIToolTemplate * > &vecToolTemplate, std::vector< STemplateTexture > &vecTemplateTexture, std::vector< SEmptySpaceTexGen > &vecTexLayout, int nWidth, int nHeight, bool bUseTemplateUVCoord )
{
	int i, j;

	for( i = 0; i < ( int )vecTemplateTexture.size(); i++ )
	{
		if( vecTexLayout[ i ].nWidth == 0 )
		{
			SUICoord TempCoord;
			memset( &TempCoord, 0, sizeof( SUICoord ) );
			if( bUseTemplateUVCoord )
				vecToolTemplate[ vecTemplateTexture[ i ].nIndex1 ]->m_Template.m_vecElement[ vecTemplateTexture[ i ].nIndex2 ].TemplateUVCoord = TempCoord;
			else
				vecToolTemplate[ vecTemplateTexture[ i ].nIndex1 ]->m_Template.m_vecElement[ vecTemplateTexture[ i ].nIndex2 ].UVCoord = TempCoord;
			continue;
		}

		LPDIRECT3DSURFACE9 pTargetSurface, pSourceSurface;
		RECT DestRect, TempRect;
		int nTexWidth = vecTemplateTexture[ i ].hTexture->Width() + 2;
		int nTexHeight = vecTemplateTexture[ i ].hTexture->Height() + 2;

		DestRect.left = vecTexLayout[ i ].nX + 1;
		DestRect.right = DestRect.left + ( nTexWidth - 2 );
		DestRect.top = vecTexLayout[ i ].nY + 1;
		DestRect.bottom = DestRect.top + ( nTexHeight - 2 );

		int nIndex1, nIndex2;
		SUICoord UVCoord;

		UVCoord.fX = ( vecTexLayout[ i ].nX + 1 ) / ( float )nWidth;
		UVCoord.fY = ( vecTexLayout[ i ].nY + 1 ) / ( float )nHeight;
		UVCoord.fWidth = ( nTexWidth - 2 ) / ( float )nWidth;
		UVCoord.fHeight = ( nTexHeight - 2 ) / ( float )nHeight;
		nIndex1 = vecTemplateTexture[ i ].nIndex1;
		nIndex2 = vecTemplateTexture[ i ].nIndex2;
		if( bUseTemplateUVCoord )
			vecToolTemplate[ nIndex1 ]->m_Template.m_vecElement[ nIndex2 ].TemplateUVCoord = UVCoord;
		else
			vecToolTemplate[ nIndex1 ]->m_Template.m_vecElement[ nIndex2 ].UVCoord = UVCoord;
		vecTemplateTexture[ i ].UVCoord = UVCoord;

		pTargetSurface = hArrangeTexture->GetSurfaceLevel();
		pSourceSurface = vecTemplateTexture[ i ].hTexture->GetSurfaceLevel();
		for( j = 0; j < 9; j++ )
		{
			TempRect = DestRect;
			OffsetRect( &TempRect, g_anOffsetX[ j ], g_anOffsetY[ j ] );
			D3DXLoadSurfaceFromSurface( pTargetSurface, NULL, &TempRect, pSourceSurface, NULL, NULL, D3DX_FILTER_NONE, 0 );
		}
	}
}

EtTextureHandle GenerateTexture( std::vector< CUIToolTemplate * > &vecToolTemplate, bool bUseTemplateUVCoord )
{
	int i, j, nTotalSpace, nReqWidth, nReqHeight;
	EtTextureHandle hTexture, hGenerateTexture;
	std::vector< STemplateTexture > vecTemplateTexture;

	// 이렇게 하면 LayoutView에서 사용하는 일반 다이얼로그에서도,
	// TemplateView에서 사용하는 템플릿 보여주기용 다이얼로그에서도, 텍스처를 안만들게 된다.
	// 다이얼로그마다 생성된 텍스처가 없으면 저장 자체를 안하기때문에, 이거 한줄이면 끝.
	// (우선은 해외때문에 커밋 불가)
	//if( bUseTemplateUVCoord == false )	// 진솔
	//	return hGenerateTexture;

	nTotalSpace = 0;
	for( i = 0; i < ( int )vecToolTemplate.size(); i++ )
	{
		for( j = 0; j < ( int )vecToolTemplate[ i ]->m_vecTextureName.size(); j++ )
		{
			STemplateTexture TemplateTexture;

			TemplateTexture.hTexture = LoadResource( vecToolTemplate[ i ]->m_vecTextureName[ j ].c_str(), RT_TEXTURE );
			if( TemplateTexture.hTexture )
			{
				nTotalSpace += ( TemplateTexture.hTexture->Width() + 2 ) * ( TemplateTexture.hTexture->Height() + 2 );
			}
			TemplateTexture.nIndex1 = i;
			TemplateTexture.nIndex2 = j;
			vecTemplateTexture.push_back( TemplateTexture );
		}
	}

	nReqWidth = 2;
	nReqHeight = 2;
	while( nReqWidth * nReqHeight < nTotalSpace )
	{
		if( nReqWidth == nReqHeight )
		{
			nReqWidth *= 2;
		}
		else
		{
			nReqHeight *= 2;
		}
	}

	std::sort( vecTemplateTexture.begin(), vecTemplateTexture.end(), TemplateTextureSortFunc );

	std::vector< SEmptySpaceTexGen > vecTexLayout;
	while( 1 )
	{
		if( ArrangeTexture( vecTemplateTexture, vecTexLayout, nReqWidth, nReqHeight ) )
		{
			break;
		}
		if( nReqWidth == nReqHeight )
		{
			nReqWidth *= 2;
		}
		else
		{
			nReqHeight *= 2;
		}
	}

	D3DLOCKED_RECT LockedRect;

	if( bUseTemplateUVCoord && nReqWidth == 2 && nReqHeight == 2 )
	{
		// 템플릿 텍스처를 생성할때 텍스처 생성할필요 없으면 작은 텍스처도 생성하지 않고 그냥 패스한다.
	}
	else if( nReqWidth * nReqHeight == 0 )
	{
		hGenerateTexture = CEtTexture::CreateNormalTexture( 16, 16, FMT_A8R8G8B8, USAGE_DYNAMIC, POOL_DEFAULT );
	}
	else
	{
		hGenerateTexture = CEtTexture::CreateNormalTexture( nReqWidth, nReqHeight, FMT_A8R8G8B8, USAGE_DYNAMIC, POOL_DEFAULT );
		( ( EtTexture * )hGenerateTexture->GetTexturePtr() )->LockRect( 0, &LockedRect, NULL, D3DLOCK_DISCARD );
		memset( LockedRect.pBits, 0, hGenerateTexture->Height() * LockedRect.Pitch );
		( ( EtTexture * )hGenerateTexture->GetTexturePtr() )->UnlockRect( 0 );
		MakeTexture( hGenerateTexture, vecToolTemplate, vecTemplateTexture, vecTexLayout, nReqWidth, nReqHeight, bUseTemplateUVCoord );
	}

	for( i = 0; i < ( int )vecTemplateTexture.size(); i++ )
	{
		SAFE_RELEASE_SPTR( vecTemplateTexture[ i ].hTexture );
	}
	CEtResource::FlushWaitDelete();

	return hGenerateTexture;
}

