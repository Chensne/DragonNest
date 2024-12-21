#include "stdafx.h"
#include <stdio.h>
#include "EtFileIO.h"
#include "EtStdFileIO.h"
#include "EtPackFileIO.h"


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif



/// 타입에 맞는 파일입출력 객체를 생성해서 돌려준다 ( 해제는 알아서.. )
CEtFileIO*
CEtFileIO::CreateFileIO(FileIOType opt)
{
	CEtFileIO* pFileIO = NULL;
	
	switch(opt) 
	{
	case ET_STD_FILE:
		{
			pFileIO = new CEtStdFileIO();
		}
		break;
	case ET_PACK_FILE:
		{
			pFileIO = new CEtPackFileIO();
		}
		break;
	}

	return pFileIO;
}
