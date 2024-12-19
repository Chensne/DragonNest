#include "Stdafx.h"
#include "Max.h"

int _GetCheckBox( HWND hWnd, int CheckID )
{
	HWND HBox = GetDlgItem( hWnd, CheckID );
	int TempResult = ( ( SendMessage( HBox, BM_GETCHECK, 0 , 0 ) == BST_CHECKED ) ? 1 : 0 ) ;

	return TempResult;
}

int _SetCheckBox( HWND hWnd,int CheckID, int Switch ) // Switch: 0 off, 1 marked, 2 greyed...
{
	HWND HBox = GetDlgItem(hWnd, CheckID);	
	return( SendMessage( HBox, BM_SETCHECK, Switch, 0 ) );		
}

int GetTextureChannelCount( Mtl *pMtl )
{
	int i, nMaxChannel=1;
	StdUVGen *uvGen;

	for( i = 0;i < pMtl->NumSubTexmaps(); i++ )
	{
		Texmap *pSubTex = pMtl->GetSubTexmap( i );
		if( ( pSubTex ) && ( pSubTex->ClassID() == Class_ID( BMTEX_CLASS_ID, 0 ) ) )
		{
			if( pMtl->ClassID() == Class_ID( DMTL_CLASS_ID, 0 ) )
			{
				if( !( ( StdMat * )pMtl )->MapEnabled( i ) )
				{
					continue;
				}
			}
			uvGen = ( ( BitmapTex * )pSubTex )->GetUVGen();
			if( uvGen )
			{
				if( uvGen->GetMapChannel() > nMaxChannel )
				{
					nMaxChannel = uvGen->GetMapChannel();
				}
			}
		}
	}

	return nMaxChannel;
}
