#include "StdAfx.h"
#include "DnAttachFile.h"

#define DNTOKEN "@.D.r.a.g.o.n.n.e.s.t.^.G.a.m.e."

#pragma warning(disable: 4996)

CDnAttachFile::CDnAttachFile(void)
{

	ZeroMemory(m_Token , sizeof(m_Token));
	char Str[128] = {0,};
	strcpy_s( Str , sizeof(Str) , DNTOKEN );
	
	for( int i = 0; i < (int)strlen(Str) ; i+= 2 )
	{
		m_Token[i/2] = Str[i];
	}

	m_FP = NULL ; 
	m_CurDetachSize = 0 ;
	m_ExeModuleSize = 0 ;
}

CDnAttachFile::~CDnAttachFile(void)
{
}

void CDnAttachFile::Initialize()
{
}

bool CDnAttachFile::Create( const char* ModuleFileName, const char* NewFileName )
{
	FILE* fpSrc = fopen( ModuleFileName, "rb" );
	if( ! fpSrc ) return FALSE;

	FILE* fpDest = fopen( NewFileName, "wb" );
	if( ! fpDest )
	{
		fclose( fpSrc );
		return FALSE;
	}

	int c;
	while( ( c = fgetc( fpSrc ) ) != EOF )
	{
		fputc( c, fpDest );
	}

	fclose( fpSrc );
	int cnt = lstrlenA( m_Token ); 

	fwrite( m_Token, lstrlenA( m_Token ), 1, fpDest );	

	m_FP = fpDest;

	return TRUE;
}

void CDnAttachFile::WriteVersion(int PrevVersion , int LastVersion)
{
	SAttachFileHeader Header; 
	
	sprintf_s(Header.FileName , sizeof(Header.FileName) , "%d" , PrevVersion );
	Header.FileSize = LastVersion ; 
	
	fwrite( &Header , sizeof(SAttachFileHeader) , 1, m_FP );

}

bool	CDnAttachFile::AttachFile( char* DestDir, char* FileNameOnly )
{
	if( ! m_FP ) return FALSE ;

	USES_CONVERSION;

	//A2W( DestDir)
	CStringA path;
	path.Format( "%s%s" , DestDir ,  FileNameOnly );

	HANDLE hFile = CreateFileA(	path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );    

	if( hFile == INVALID_HANDLE_VALUE ) return FALSE ;

	SAttachFileHeader header;
	strcpy_s( header.FileName, sizeof(header.FileName) , FileNameOnly );
	header.FileSize = GetFileSize( hFile, NULL );
	if( header.FileSize == 0xFFFFFFFF ) return FALSE;
		
	fwrite( &header, sizeof( header ), 1, m_FP );
//	fwrite( &header.FileSize, sizeof( header.FileSize ), 1, m_FP );


	BYTE buf[BUFSIZ];

	DWORD readBytes;

	for( ;; )
	{
		ZeroMemory(buf , sizeof(buf)) ;
		ReadFile( hFile, buf, BUFSIZ, &readBytes, NULL );
		if( readBytes == 0 ) 
		{
			break;
		}
		fwrite( buf, readBytes, 1, m_FP );
	}

	CloseHandle( hFile );
	return TRUE; 
}


void CDnAttachFile::Close()
{
	fclose( m_FP );
	m_FP =NULL ;
}


bool CDnAttachFile::Open(const char * FileName)
{
	m_FP = fopen( FileName, "rb" );
	if( !m_FP ) return FALSE;

	int c;
	char buf[32];
	int i;

	while( ( c = fgetc( m_FP ) ) != EOF )
	{
		++m_ExeModuleSize;

		if( c == '@' )
		{
			ZeroMemory( buf, sizeof( buf ) );
			buf[0] = '@';
			for( i = 1; i < 16; ++i )
			{
				if( ( c = fgetc( m_FP ) ) == EOF )
				{
					fclose( m_FP );
					return FALSE;
				}
				++m_ExeModuleSize;
				buf[i] = c;
			}

			if( lstrcmpA( buf, m_Token ) == 0 )
			{
				m_ExeModuleSize -= 16;
				return TRUE;	
			}
		}
	}

	fclose( m_FP );
	return FALSE;
}


bool CDnAttachFile::Detach(const char* DestDir)
{
	SAttachFileHeader header;
	if( ReadHeader( &header ) )
	{
		Detach( DestDir, &header );
		return TRUE;
	}

	return FALSE; 
}


bool CDnAttachFile::Detach( const char* DestDir, const SAttachFileHeader* pHeader )
{
	if( ! m_FP ) return FALSE;

	char outFileName[MAX_PATH];
	wsprintfA( outFileName, "%s\\%s", DestDir, pHeader->FileName );
	FILE* fpOut = fopen( outFileName, "wb" );
	if( ! fpOut ) return FALSE;

	m_CurDetachSize = 0;

	int block = pHeader->FileSize / BUFSIZ;
	int rest = pHeader->FileSize % BUFSIZ;

	//512 사이즈로 끝어서 읽어옴... 

	BYTE buf[BUFSIZ];
	for( int i = 0; i < block; ++i )
	{
		fread( buf, BUFSIZ, 1, m_FP );
		fwrite( buf, BUFSIZ, 1, fpOut );
		m_CurDetachSize += BUFSIZ;
	}

	if( rest > 0 )
	{
		fread( buf, rest, 1, m_FP );
		fwrite( buf, rest, 1, fpOut );

		m_CurDetachSize += rest;
	}

	fclose( fpOut );

	return TRUE; 
}




bool CDnAttachFile::ReadHeader(SAttachFileHeader * pHeader)
{
	if( !m_FP ) return FALSE; 

	if( fread(pHeader , sizeof(SAttachFileHeader)  ,1 , m_FP ) < 1 ) return FALSE;
	
	return TRUE; 
}