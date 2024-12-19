#include "StdAfx.h"
#include "EtUIDialogInfo.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

bool SUIDialogInfo::Load_01( CStream &stream )
{
	stream >> AllignHori;
	stream >> AllignVert;
	stream >> DlgCoord;
	stream >> dwDlgColor;
	stream.ReadBuffer( szDlgTextureName, sizeof(szDlgTextureName) );
	stream.ReadBuffer( szUITexturename, sizeof(szUITexturename) );
	stream.SeekCur<int>();
	stream.SeekCur<int>();
	stream >> nFrameLeft;
	stream >> nFrameTop;
	stream >> nFrameRight;
	stream >> nFrameBottom;

	return true;
}

bool SUIDialogInfo::Load_02( CStream &stream )
{
	stream >> AllignHori;
	stream >> AllignVert;
	stream >> DlgCoord;
	stream >> dwDlgColor;
	stream.ReadBuffer( szDlgTextureName, sizeof(szDlgTextureName) );
	stream.ReadBuffer( szUITexturename, sizeof(szUITexturename) );
	stream.SeekCur<int>();
	stream.SeekCur<int>();
	stream >> nFrameLeft;
	stream >> nFrameTop;
	stream >> nFrameRight;
	stream >> nFrameBottom;
	stream.Seek( sizeof(char)*_MAX_PATH, SEEK_CUR );
	stream.Seek( sizeof(char)*_MAX_PATH, SEEK_CUR );

	return true;
}

bool SUIDialogInfo::Load_03( CStream &stream )
{
	stream >> AllignHori;
	stream >> AllignVert;
	stream >> DlgCoord;
	stream >> dwDlgColor;
	stream.ReadBuffer( szDlgTextureName, sizeof(szDlgTextureName) );
	stream.ReadBuffer( szUITexturename, sizeof(szUITexturename) );
	stream.SeekCur<int>();
	stream.SeekCur<int>();
	stream >> nFrameLeft;
	stream >> nFrameTop;
	stream >> nFrameRight;
	stream >> nFrameBottom;
	stream.Seek( sizeof(char)*_MAX_PATH, SEEK_CUR );
	stream.Seek( sizeof(char)*_MAX_PATH, SEEK_CUR );
	stream >> bModalDialog;

	return true;
}

bool SUIDialogInfo::Load_04( CStream &stream )
{
	stream >> AllignHori;
	stream >> AllignVert;
	stream >> DlgCoord;
	stream >> dwDlgColor;
	stream.ReadBuffer( szDlgTextureName, sizeof(szDlgTextureName) );
	stream.ReadBuffer( szUITexturename, sizeof(szUITexturename) );
	stream.SeekCur<int>();
	stream.SeekCur<int>();
	stream >> nFrameLeft;
	stream >> nFrameTop;
	stream >> nFrameRight;
	stream >> nFrameBottom;
	stream >> bModalDialog;
	stream >> bSound;

	return true;
}

bool SUIDialogInfo::Load_05( CStream &stream )
{
	stream >> AllignHori;
	stream >> AllignVert;
	stream >> DlgCoord;
	stream >> dwDlgColor;
	stream.ReadBuffer( szDlgTextureName, sizeof(szDlgTextureName) );
	stream.ReadBuffer( szUITexturename, sizeof(szUITexturename) );
	stream >> nFrameLeft;
	stream >> nFrameTop;
	stream >> nFrameRight;
	stream >> nFrameBottom;
	stream >> bModalDialog;
	stream >> bSound;

	return true;
}

bool SUIDialogInfo::Load( CStream &stream )
{
	DWORD dwVersion;
	stream >> dwVersion;

	switch( dwVersion )
	{
	case SUIDialogInfo_01::VERSION:	Load_01(stream); break;
	case SUIDialogInfo_02::VERSION: Load_02(stream); break;
	case SUIDialogInfo_03::VERSION: Load_03(stream); break;
	case SUIDialogInfo_04::VERSION: Load_04(stream); break;
	case SUIDialogInfo_05::VERSION: Load_05(stream); break;
	case VERSION:
		{
			stream >> AllignHori;
			stream >> AllignVert;
			stream >> DlgCoord;
			stream >> dwDlgColor;
			stream.ReadBuffer( szDlgTextureName, sizeof(szDlgTextureName) );
			stream.ReadBuffer( szUITexturename, sizeof(szUITexturename) );
			stream >> nFrameLeft;
			stream >> nFrameTop;
			stream >> nFrameRight;
			stream >> nFrameBottom;
			stream >> bModalDialog;
			stream >> bSound;
			stream >> bLockScalingByResolution;
		}
		break;

	default:
		CDebugSet::ToLogFile( "SUIDialogInfo::Load, Invalid Dialog Info Version!" );
		return false;
	}

	return true;
}

bool SUIDialogInfo::Save( CStream &stream )
{
	stream << VERSION;

	stream << AllignHori;
	stream << AllignVert;
	stream << DlgCoord;
	stream << dwDlgColor;
	stream.WriteBuffer( szDlgTextureName, sizeof(szDlgTextureName) );
	stream.WriteBuffer( szUITexturename, sizeof(szUITexturename) );
	stream << nFrameLeft;
	stream << nFrameTop;
	stream << nFrameRight;
	stream << nFrameBottom;
	stream << bModalDialog;
	stream << bSound;
	stream << bLockScalingByResolution;

	return true;
}