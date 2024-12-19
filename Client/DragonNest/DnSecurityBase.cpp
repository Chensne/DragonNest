#include "stdafx.h"
#include "DnSecurityBase.h"
#include "DnInterface.h"
#include <MMSystem.h>
#include "DnSecure.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnSecurityBase::CDnSecurityBase()
{
	MouseLClickPress = false; 
	m_fTime = 0.0f; 
	m_pBtnDelete = NULL;
#ifdef PRE_MOD_SECURITYNUMPAD_RENEWAL
	m_pBtnShuffle = NULL;
#endif 
}	



CDnSecurityBase::~CDnSecurityBase(void)
{


}

bool CDnSecurityBase::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{

	return TRUE;

}

bool CDnSecurityBase::CheckMaxCountPw(SecondPassStruct &Pass)
{
	if( Pass.GetSize() != SecondAuth::Common::PWMaxLength )
	{
		WCHAR str[1024]={0,}; // 6444
		swprintf_s(str, 1024 , GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 6444 ) , (int)SecondAuth::Common::PWMaxLength );//HAN_MSG
		GetInterface().MessageBox( str ) ;
		return TRUE;
	}

	return FALSE; 
}

bool CDnSecurityBase::CheckCompareWord(SecondPassStruct &Pass1, SecondPassStruct &Pass2)
{
	if( Pass1 == Pass2 ) return true;
	return false;
}

int CDnSecurityBase::SuffleNumber1( std::vector<std::wstring> &vecNumberStr, std::vector<CEtUIButton*> &vecBtnNumPad, int &nCountNum, int *nResultSeed )
{
	nCountNum++;

	std::vector<std::wstring> szVecTemp, szVecTemp2;
	szVecTemp2 = vecNumberStr;
	CMtRandom Random;

	std::vector<std::string> tokens;
	TokenizeA(std::string(SecondPassword::GetVersion()), tokens, ".");

	int nKey = rand();

	if( atoi(tokens[1].c_str()) / 10 == nCountNum )  {
		nKey = atoi(tokens[0].c_str()) + atoi(tokens[1].c_str());
		if( nKey % 2 ) nKey = -nKey;
	}
	else {
		nKey = atoi(tokens[0].c_str()) + atoi(tokens[1].c_str()) + atoi(tokens[2].c_str());
		nKey = nKey << 3;
		nKey = nKey >> 6;
	}

	int nSeed = timeGetTime();
	if( atoi(tokens[1].c_str()) / 10 == nCountNum )
		Random.srand( nSeed + nKey - ( nCountNum / 912374 ) );
	else Random.srand( nSeed + nKey - ( nCountNum / 3 ) );
	if( nResultSeed ) {
		if( atoi(tokens[1].c_str()) / 10 == nCountNum ) *nResultSeed = nSeed;
		else *nResultSeed = nSeed + nCountNum - nKey;
	}
	while( !szVecTemp2.empty() ) {
		int nOffset = Random.rand( (int)szVecTemp2.size() );
		szVecTemp.push_back( szVecTemp2[nOffset] );
		szVecTemp2.erase( szVecTemp2.begin() + nOffset );
	}

	for( int i = 0 ; i < (int)szVecTemp.size() ; ++i )
	{
		vecBtnNumPad[i]->SetText(szVecTemp[i].c_str());
	}

	if( nCountNum % 6 == 0 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return SuffleNumber1( vecNumberStr, vecBtnNumPad, nCountNum, nResultSeed );
	}
	else if( nCountNum % 6 == 1 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return SuffleNumber2( vecNumberStr, vecBtnNumPad, nCountNum, nResultSeed );
	}
	else if( nCountNum % 6 == 2 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return SuffleNumber3( vecNumberStr, vecBtnNumPad, nCountNum, nResultSeed );
	}
	else if( nCountNum % 6 == 3 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return SuffleNumber4( vecNumberStr, vecBtnNumPad, nCountNum, nResultSeed );
	}
	else if( nCountNum % 6 == 4 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return SuffleNumber5( vecNumberStr, vecBtnNumPad, nCountNum, nResultSeed );
	}
	else if( nCountNum % 6 == 5 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return SuffleNumber6( vecNumberStr, vecBtnNumPad, nCountNum, nResultSeed );
	}
	return 1;
}

int CDnSecurityBase::SuffleNumber2( std::vector<std::wstring> &vecNumberStr, std::vector<CEtUIButton*> &vecBtnNumPad, int &nCountNum, int *nResultSeed )
{
	nCountNum++;

	std::vector<std::wstring> szVecTemp, szVecTemp2;
	szVecTemp2 = vecNumberStr;
	CMtRandom Random;

	std::vector<std::string> tokens;
	TokenizeA(std::string(SecondPassword::GetVersion()), tokens, ".");

	int nKey = rand();

	if( atoi(tokens[1].c_str()) / 10 == nCountNum )  {
		nKey = atoi(tokens[0].c_str()) + atoi(tokens[1].c_str());
		if( nKey % 2 ) nKey = -nKey;
	}
	else {
		nKey = atoi(tokens[0].c_str()) + atoi(tokens[1].c_str()) + atoi(tokens[2].c_str());
		nKey = nKey << 7;
		nKey = nKey >> 4;
	}

	int nSeed = timeGetTime();
	if( atoi(tokens[1].c_str()) / 10 == nCountNum )
		Random.srand( nSeed + nKey - ( nCountNum / 783343 ) );
	else Random.srand( nSeed + nKey - ( nCountNum / 6) );
	if( nResultSeed ) {
		if( atoi(tokens[1].c_str()) / 10 == nCountNum ) *nResultSeed = nSeed;
		else *nResultSeed = nSeed + nCountNum - nKey;
	}
	while( !szVecTemp2.empty() ) {
		int nOffset = Random.rand( (int)szVecTemp2.size() );
		szVecTemp.push_back( szVecTemp2[nOffset] );
		szVecTemp2.erase( szVecTemp2.begin() + nOffset );
	}

	for( int i = 0 ; i < (int)szVecTemp.size() ; ++i )
	{
		vecBtnNumPad[i]->SetText(szVecTemp[i].c_str());
	}

	if( nCountNum % 6 == 0 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return SuffleNumber1( vecNumberStr, vecBtnNumPad, nCountNum, nResultSeed );
	}
	else if( nCountNum % 6 == 1 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return SuffleNumber2( vecNumberStr, vecBtnNumPad, nCountNum, nResultSeed );
	}
	else if( nCountNum % 6 == 2 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return SuffleNumber3( vecNumberStr, vecBtnNumPad, nCountNum, nResultSeed );
	}
	else if( nCountNum % 6 == 3 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return SuffleNumber4( vecNumberStr, vecBtnNumPad, nCountNum, nResultSeed );
	}
	else if( nCountNum % 6 == 4 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return SuffleNumber5( vecNumberStr, vecBtnNumPad, nCountNum, nResultSeed );
	}
	else if( nCountNum % 6 == 5 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return SuffleNumber6( vecNumberStr, vecBtnNumPad, nCountNum, nResultSeed );
	}
	return 2;
}

int CDnSecurityBase::SuffleNumber3( std::vector<std::wstring> &vecNumberStr, std::vector<CEtUIButton*> &vecBtnNumPad, int &nCountNum, int *nResultSeed )
{
	nCountNum++;

	std::vector<std::wstring> szVecTemp, szVecTemp2;
	szVecTemp2 = vecNumberStr;

	CMtRandom Random;

	std::vector<std::string> tokens;
	TokenizeA(std::string(SecondPassword::GetVersion()), tokens, ".");

	int nKey = rand();

	if( atoi(tokens[1].c_str()) / 10 == nCountNum )  {
		nKey = atoi(tokens[0].c_str()) + atoi(tokens[1].c_str());
		if( nKey % 2 ) nKey = -nKey;
	}
	else {
		nKey = atoi(tokens[0].c_str()) + atoi(tokens[1].c_str()) + atoi(tokens[2].c_str());
		nKey = nKey << 1;
		nKey = nKey >> 2;
	}

	int nSeed = timeGetTime();
	if( atoi(tokens[1].c_str()) / 10 == nCountNum )
		Random.srand( nSeed + nKey - ( nCountNum / 532380 ) );
	else Random.srand( nSeed + nKey - ( nCountNum / 5 ) );
	if( nResultSeed ) {
		if( atoi(tokens[1].c_str()) / 10 == nCountNum ) *nResultSeed = nSeed;
		else *nResultSeed = nSeed + nCountNum - nKey;
	}
	while( !szVecTemp2.empty() ) {
		int nOffset = Random.rand( (int)szVecTemp2.size() );
		szVecTemp.push_back( szVecTemp2[nOffset] );
		szVecTemp2.erase( szVecTemp2.begin() + nOffset );
	}

	for( int i = 0 ; i < (int)szVecTemp.size() ; ++i )
	{
		vecBtnNumPad[i]->SetText(szVecTemp[i].c_str());
	}

	if( nCountNum % 6 == 0 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return SuffleNumber1( vecNumberStr, vecBtnNumPad, nCountNum, nResultSeed );
	}
	else if( nCountNum % 6 == 1 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return SuffleNumber2( vecNumberStr, vecBtnNumPad, nCountNum, nResultSeed );
	}
	else if( nCountNum % 6 == 2 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return SuffleNumber3( vecNumberStr, vecBtnNumPad, nCountNum, nResultSeed );
	}
	else if( nCountNum % 6 == 3 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return SuffleNumber4( vecNumberStr, vecBtnNumPad, nCountNum, nResultSeed );
	}
	else if( nCountNum % 6 == 4 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return SuffleNumber5( vecNumberStr, vecBtnNumPad, nCountNum, nResultSeed );
	}
	else if( nCountNum % 6 == 5 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return SuffleNumber6( vecNumberStr, vecBtnNumPad, nCountNum, nResultSeed );
	}
	return 3;
}

int CDnSecurityBase::SuffleNumber4( std::vector<std::wstring> &vecNumberStr, std::vector<CEtUIButton*> &vecBtnNumPad, int &nCountNum, int *nResultSeed )
{
	nCountNum++;

	std::vector<std::wstring> szVecTemp, szVecTemp2;
	szVecTemp2 = vecNumberStr;

	CMtRandom Random;

	std::vector<std::string> tokens;
	TokenizeA(std::string(SecondPassword::GetVersion()), tokens, ".");

	int nKey = rand();

	if( atoi(tokens[1].c_str()) / 10 == nCountNum )  {
		nKey = atoi(tokens[0].c_str()) + atoi(tokens[1].c_str());
		if( nKey % 2 ) nKey = -nKey;
	}
	else {
		nKey = atoi(tokens[0].c_str()) + atoi(tokens[1].c_str()) + atoi(tokens[2].c_str());
		nKey = nKey << 2;
		nKey = nKey >> 8;
	}

	int nSeed = timeGetTime();
	if( atoi(tokens[1].c_str()) / 10 == nCountNum )
		Random.srand( nSeed + nKey - ( nCountNum / 120050 ) );
	else Random.srand( nSeed + nKey - (nCountNum / 4) );
	if( nResultSeed ) {
		if( atoi(tokens[1].c_str()) / 10 == nCountNum ) *nResultSeed = nSeed;
		else *nResultSeed = nSeed + nCountNum - nKey;
	}
	while( !szVecTemp2.empty() ) {
		int nOffset = Random.rand( (int)szVecTemp2.size() );
		szVecTemp.push_back( szVecTemp2[nOffset] );
		szVecTemp2.erase( szVecTemp2.begin() + nOffset );
	}

	for( int i = 0 ; i < (int)szVecTemp.size() ; ++i )
	{
		vecBtnNumPad[i]->SetText(szVecTemp[i].c_str());
	}

	if( nCountNum % 6 == 0 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return SuffleNumber1( vecNumberStr, vecBtnNumPad, nCountNum, nResultSeed );
	}
	else if( nCountNum % 6 == 1 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return SuffleNumber2( vecNumberStr, vecBtnNumPad, nCountNum, nResultSeed );
	}
	else if( nCountNum % 6 == 2 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return SuffleNumber3( vecNumberStr, vecBtnNumPad, nCountNum, nResultSeed );
	}
	else if( nCountNum % 6 == 3 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return SuffleNumber4( vecNumberStr, vecBtnNumPad, nCountNum, nResultSeed );
	}
	else if( nCountNum % 6 == 4 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return SuffleNumber5( vecNumberStr, vecBtnNumPad, nCountNum, nResultSeed );
	}
	else if( nCountNum % 6 == 5 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return SuffleNumber6( vecNumberStr, vecBtnNumPad, nCountNum, nResultSeed );
	}
	return 4;
}

int CDnSecurityBase::SuffleNumber5( std::vector<std::wstring> &vecNumberStr, std::vector<CEtUIButton*> &vecBtnNumPad, int &nCountNum, int *nResultSeed )
{
	nCountNum++;

	std::vector<std::wstring> szVecTemp, szVecTemp2;
	szVecTemp2 = vecNumberStr;

	CMtRandom Random;

	std::vector<std::string> tokens;
	TokenizeA(std::string(SecondPassword::GetVersion()), tokens, ".");

	int nKey = rand();

	if( atoi(tokens[1].c_str()) / 10 == nCountNum )  {
		nKey = atoi(tokens[0].c_str()) + atoi(tokens[1].c_str());
		if( nKey % 2 ) nKey = -nKey;
	}
	else {
		nKey = atoi(tokens[0].c_str()) + atoi(tokens[1].c_str()) + atoi(tokens[2].c_str());
		nKey = nKey << 5;
		nKey = nKey >> 4;
	}

	int nSeed = timeGetTime();
	if( atoi(tokens[1].c_str()) / 10 == nCountNum )
		Random.srand( nSeed + nKey - ( nCountNum / 10000000 ) );
	else Random.srand( nSeed + nKey - ( nCountNum / 3 ) );
	if( nResultSeed ) {
		if( atoi(tokens[1].c_str()) / 10 == nCountNum ) *nResultSeed = nSeed;
		else *nResultSeed = nSeed + nCountNum - nKey;
	}
	while( !szVecTemp2.empty() ) {
		int nOffset = Random.rand( (int)szVecTemp2.size() );
		szVecTemp.push_back( szVecTemp2[nOffset] );
		szVecTemp2.erase( szVecTemp2.begin() + nOffset );
	}

	for( int i = 0 ; i < (int)szVecTemp.size() ; ++i )
	{
		vecBtnNumPad[i]->SetText(szVecTemp[i].c_str());
	}

	if( nCountNum % 6 == 0 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return SuffleNumber1( vecNumberStr, vecBtnNumPad, nCountNum, nResultSeed );
	}
	else if( nCountNum % 6 == 1 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return SuffleNumber2( vecNumberStr, vecBtnNumPad, nCountNum, nResultSeed );
	}
	else if( nCountNum % 6 == 2 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return SuffleNumber3( vecNumberStr, vecBtnNumPad, nCountNum, nResultSeed );
	}
	else if( nCountNum % 6 == 3 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return SuffleNumber4( vecNumberStr, vecBtnNumPad, nCountNum, nResultSeed );
	}
	else if( nCountNum % 6 == 4 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return SuffleNumber5( vecNumberStr, vecBtnNumPad, nCountNum, nResultSeed );
	}
	else if( nCountNum % 6 == 5 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return SuffleNumber6( vecNumberStr, vecBtnNumPad, nCountNum, nResultSeed );
	}
	return 5;
}

int CDnSecurityBase::SuffleNumber6( std::vector<std::wstring> &vecNumberStr, std::vector<CEtUIButton*> &vecBtnNumPad, int &nCountNum, int *nResultSeed )
{
	nCountNum++;

	std::vector<std::wstring> szVecTemp, szVecTemp2;
	szVecTemp2 = vecNumberStr;

	CMtRandom Random;

	std::vector<std::string> tokens;
	TokenizeA(std::string(SecondPassword::GetVersion()), tokens, ".");

	int nKey = rand();

	if( atoi(tokens[1].c_str()) / 10 == nCountNum )  {
		nKey = atoi(tokens[0].c_str()) + atoi(tokens[1].c_str());
		if( nKey % 2 ) nKey = -nKey;
	}
	else {
		nKey = atoi(tokens[0].c_str()) + atoi(tokens[1].c_str()) + atoi(tokens[2].c_str());
		nKey = nKey << 3;
		nKey = nKey >> 7;
	}

	int nSeed = timeGetTime();
	if( atoi(tokens[1].c_str()) / 10 == nCountNum )
		Random.srand( nSeed + nKey - ( nCountNum / 100000 ) );
	else Random.srand( nSeed + nKey - ( nCountNum / 2 ) );
	if( nResultSeed ) {
		if( atoi(tokens[1].c_str()) / 10 == nCountNum ) *nResultSeed = nSeed;
		else *nResultSeed = nSeed + nCountNum - nKey;
	}
	while( !szVecTemp2.empty() ) {
		int nOffset = Random.rand( (int)szVecTemp2.size() );
		szVecTemp.push_back( szVecTemp2[nOffset] );
		szVecTemp2.erase( szVecTemp2.begin() + nOffset );
	}

	for( int i = 0 ; i < (int)szVecTemp.size() ; ++i )
	{
		vecBtnNumPad[i]->SetText(szVecTemp[i].c_str());
	}

	if( nCountNum % 6 == 0 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return SuffleNumber1( vecNumberStr, vecBtnNumPad, nCountNum, nResultSeed );
	}
	else if( nCountNum % 6 == 1 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return SuffleNumber2( vecNumberStr, vecBtnNumPad, nCountNum, nResultSeed );
	}
	else if( nCountNum % 6 == 2 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return SuffleNumber3( vecNumberStr, vecBtnNumPad, nCountNum, nResultSeed );
	}
	else if( nCountNum % 6 == 3 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return SuffleNumber4( vecNumberStr, vecBtnNumPad, nCountNum, nResultSeed );
	}
	else if( nCountNum % 6 == 4 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return SuffleNumber5( vecNumberStr, vecBtnNumPad, nCountNum, nResultSeed );
	}
	else if( nCountNum % 6 == 5 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return SuffleNumber6( vecNumberStr, vecBtnNumPad, nCountNum, nResultSeed );
	}
	return 6;
}

int CDnSecurityBase::RevertNumber1( const int nSeed, const int nValue[], int &nCountNum, std::string &szResultValue )
{
	nCountNum++;

	std::vector<std::string> szVecTemp, szVecTemp2;

	CMtRandom Random;

	std::vector<std::string> tokens;
	TokenizeA(std::string(SecondPassword::GetVersion()), tokens, ".");

	int nKey = rand();

	if( atoi(tokens[1].c_str()) / 10 == nCountNum )  {
		nKey = atoi(tokens[0].c_str()) + atoi(tokens[1].c_str());
		if( nKey % 2 ) nKey = -nKey;
	}
	else {
		nKey = atoi(tokens[0].c_str()) + atoi(tokens[1].c_str()) + atoi(tokens[2].c_str());
		nKey = nKey << 3;
		nKey = nKey >> 7;
	}

	int nSeed2 = timeGetTime();
	if( atoi(tokens[1].c_str()) / 10 == nCountNum )
		Random.srand( nSeed + nKey - ( nCountNum / 742345 ) );
	else Random.srand( nSeed2 + nKey - ( nCountNum / 2 ) );

	for( int i=0; i<10; i++ ) {
		char szTT[2];
		sprintf_s( szTT, "%c", (atoi(tokens[1].c_str())+i)%255  );
		szVecTemp2.push_back( std::string(szTT) );
	}

	while( !szVecTemp2.empty() ) {
		int nOffset = Random.rand( (int)szVecTemp2.size() );
		szVecTemp.push_back( szVecTemp2[nOffset] );
		szVecTemp2.erase( szVecTemp2.begin() + nOffset );
	}

	if( atoi(tokens[1].c_str()) / 10 == nCountNum ) {
		for( int i=0 ; i<4; ++i )
		{
			if( nValue[i] < 0 ) continue;
			szResultValue += szVecTemp[nValue[i]].c_str();
		}
	}



	if( nCountNum % 6 == 0 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return RevertNumber1( nSeed, nValue, nCountNum, szResultValue );
	}
	else if( nCountNum % 6 == 1 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return RevertNumber2( nSeed, nValue, nCountNum, szResultValue );
	}
	else if( nCountNum % 6 == 2 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return RevertNumber3( nSeed, nValue, nCountNum, szResultValue );
	}
	else if( nCountNum % 6 == 3 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return RevertNumber4( nSeed, nValue, nCountNum, szResultValue );
	}
	else if( nCountNum % 6 == 4 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return RevertNumber5( nSeed, nValue, nCountNum, szResultValue );
	}
	else if( nCountNum % 6 == 5 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return RevertNumber6( nSeed, nValue, nCountNum, szResultValue );
	}
	return 1;
}

int CDnSecurityBase::RevertNumber2( const int nSeed, const int nValue[], int &nCountNum, std::string &szResultValue )
{
	nCountNum++;

	std::vector<std::string> szVecTemp, szVecTemp2;


	CMtRandom Random;

	std::vector<std::string> tokens;
	TokenizeA(std::string(SecondPassword::GetVersion()), tokens, ".");

	int nKey = rand();

	if( atoi(tokens[1].c_str()) / 10 == nCountNum )  {
		nKey = atoi(tokens[0].c_str()) + atoi(tokens[1].c_str());
		if( nKey % 2 ) nKey = -nKey;
	}
	else {
		nKey = atoi(tokens[0].c_str()) + atoi(tokens[1].c_str()) + atoi(tokens[2].c_str());
		nKey = nKey << 1;
		nKey = nKey >> 5;
	}

	int nSeed2 = timeGetTime();
	if( atoi(tokens[1].c_str()) / 10 == nCountNum )
		Random.srand( nSeed + nKey - ( nCountNum / 241256 ) );
	else Random.srand( nSeed2 + nKey - ( nCountNum / 3 ) );

	for( int i=0; i<10; i++ ) {
		char szTT[2];
		sprintf_s( szTT, "%c", (atoi(tokens[1].c_str())+i)%255  );
		szVecTemp2.push_back( std::string(szTT) );
	}

	while( !szVecTemp2.empty() ) {
		int nOffset = Random.rand( (int)szVecTemp2.size() );
		szVecTemp.push_back( szVecTemp2[nOffset] );
		szVecTemp2.erase( szVecTemp2.begin() + nOffset );
	}

	if( atoi(tokens[1].c_str()) / 10 == nCountNum ) {
		for( int i=0 ; i<4; ++i )
		{
			if( nValue[i] < 0 ) continue;
			szResultValue += szVecTemp[nValue[i]].c_str();
		}
	}

	if( nCountNum % 6 == 0 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return RevertNumber1( nSeed, nValue, nCountNum, szResultValue );
	}
	else if( nCountNum % 6 == 1 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return RevertNumber2( nSeed, nValue, nCountNum, szResultValue );
	}
	else if( nCountNum % 6 == 2 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return RevertNumber3( nSeed, nValue, nCountNum, szResultValue );
	}
	else if( nCountNum % 6 == 3 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return RevertNumber4( nSeed, nValue, nCountNum, szResultValue );
	}
	else if( nCountNum % 6 == 4 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return RevertNumber5( nSeed, nValue, nCountNum, szResultValue );
	}
	else if( nCountNum % 6 == 5 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return RevertNumber6( nSeed, nValue, nCountNum, szResultValue );
	}
	return 2;
}

int CDnSecurityBase::RevertNumber3( const int nSeed, const int nValue[], int &nCountNum, std::string &szResultValue )
{
	nCountNum++;

	std::vector<std::string> szVecTemp, szVecTemp2;

	CMtRandom Random;

	std::vector<std::string> tokens;
	TokenizeA(std::string(SecondPassword::GetVersion()), tokens, ".");

	int nKey = rand();

	if( atoi(tokens[1].c_str()) / 10 == nCountNum )  {
		nKey = atoi(tokens[0].c_str()) + atoi(tokens[1].c_str());
		if( nKey % 2 ) nKey = -nKey;
	}
	else {
		nKey = atoi(tokens[0].c_str()) + atoi(tokens[1].c_str()) + atoi(tokens[2].c_str());
		nKey = nKey << 1;
		nKey = nKey >> 3;
	}

	int nSeed2 = timeGetTime();
	if( atoi(tokens[1].c_str()) / 10 == nCountNum )
		Random.srand( nSeed + nKey - ( nCountNum / 441234 ) );
	else Random.srand( nSeed2 + nKey - ( nCountNum / 4 ) );

	for( int i=0; i<10; i++ ) {
		char szTT[2];
		sprintf_s( szTT, "%c", (atoi(tokens[1].c_str())+i)%255  );
		szVecTemp2.push_back( std::string(szTT) );
	}

	while( !szVecTemp2.empty() ) {
		int nOffset = Random.rand( (int)szVecTemp2.size() );
		szVecTemp.push_back( szVecTemp2[nOffset] );
		szVecTemp2.erase( szVecTemp2.begin() + nOffset );
	}

	if( atoi(tokens[1].c_str()) / 10 == nCountNum ) {
		for( int i=0 ; i<4; ++i )
		{
			if( nValue[i] < 0 ) continue;
			szResultValue += szVecTemp[nValue[i]].c_str();
		}
	}

	if( nCountNum % 6 == 0 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return RevertNumber1( nSeed, nValue, nCountNum, szResultValue );
	}
	else if( nCountNum % 6 == 1 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return RevertNumber2( nSeed, nValue, nCountNum, szResultValue );
	}
	else if( nCountNum % 6 == 2 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return RevertNumber3( nSeed, nValue, nCountNum, szResultValue );
	}
	else if( nCountNum % 6 == 3 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return RevertNumber4( nSeed, nValue, nCountNum, szResultValue );
	}
	else if( nCountNum % 6 == 4 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return RevertNumber5( nSeed, nValue, nCountNum, szResultValue );
	}
	else if( nCountNum % 6 == 5 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return RevertNumber6( nSeed, nValue, nCountNum, szResultValue );
	}
	return 3;
}

int CDnSecurityBase::RevertNumber4( const int nSeed, const int nValue[], int &nCountNum, std::string &szResultValue )
{
	nCountNum++;

	std::vector<std::string> szVecTemp, szVecTemp2;
	CMtRandom Random;

	std::vector<std::string> tokens;
	TokenizeA(std::string(SecondPassword::GetVersion()), tokens, ".");

	int nKey = rand();

	if( atoi(tokens[1].c_str()) / 10 == nCountNum )  {
		nKey = atoi(tokens[0].c_str()) + atoi(tokens[1].c_str());
		if( nKey % 2 ) nKey = -nKey;
	}
	else {
		nKey = atoi(tokens[0].c_str()) + atoi(tokens[1].c_str()) + atoi(tokens[2].c_str());
		nKey = nKey << 7;
		nKey = nKey >> 2;
	}

	int nSeed2 = timeGetTime();
	if( atoi(tokens[1].c_str()) / 10 == nCountNum )
		Random.srand( nSeed + nKey - ( nCountNum / 995421 ) );
	else Random.srand( nSeed2 + nKey - ( nCountNum / 4 ) );

	for( int i=0; i<10; i++ ) {
		char szTT[2];
		sprintf_s( szTT, "%c", (atoi(tokens[1].c_str())+i)%255  );
		szVecTemp2.push_back( std::string(szTT) );
	}

	while( !szVecTemp2.empty() ) {
		int nOffset = Random.rand( (int)szVecTemp2.size() );
		szVecTemp.push_back( szVecTemp2[nOffset] );
		szVecTemp2.erase( szVecTemp2.begin() + nOffset );
	}

	if( atoi(tokens[1].c_str()) / 10 == nCountNum ) {
		for( int i=0 ; i<4; ++i )
		{
			if( nValue[i] < 0 ) continue;
			szResultValue += szVecTemp[nValue[i]].c_str();
		}
	}

	if( nCountNum % 6 == 0 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return RevertNumber1( nSeed, nValue, nCountNum, szResultValue );
	}
	else if( nCountNum % 6 == 1 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return RevertNumber2( nSeed, nValue, nCountNum, szResultValue );
	}
	else if( nCountNum % 6 == 2 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return RevertNumber3( nSeed, nValue, nCountNum, szResultValue );
	}
	else if( nCountNum % 6 == 3 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return RevertNumber4( nSeed, nValue, nCountNum, szResultValue );
	}
	else if( nCountNum % 6 == 4 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return RevertNumber5( nSeed, nValue, nCountNum, szResultValue );
	}
	else if( nCountNum % 6 == 5 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return RevertNumber6( nSeed, nValue, nCountNum, szResultValue );
	}
	return 4;
}

int CDnSecurityBase::RevertNumber5( const int nSeed, const int nValue[], int &nCountNum, std::string &szResultValue )
{
	nCountNum++;

	std::vector<std::string> szVecTemp, szVecTemp2;

	CMtRandom Random;

	std::vector<std::string> tokens;
	TokenizeA(std::string(SecondPassword::GetVersion()), tokens, ".");

	int nKey = rand();

	if( atoi(tokens[1].c_str()) / 10 == nCountNum )  {
		nKey = atoi(tokens[0].c_str()) + atoi(tokens[1].c_str());
		if( nKey % 2 ) nKey = -nKey;
	}
	else {
		nKey = atoi(tokens[0].c_str()) + atoi(tokens[1].c_str()) + atoi(tokens[2].c_str());
		nKey = nKey << 4;
		nKey = nKey >> 4;
	}

	int nSeed2 = timeGetTime();
	if( atoi(tokens[1].c_str()) / 10 == nCountNum )
		Random.srand( nSeed + nKey - ( nCountNum / 715324 ) );
	else Random.srand( nSeed2 + nKey - ( nCountNum / 5 ) );

	for( int i=0; i<10; i++ ) {
		char szTT[2];
		sprintf_s( szTT, "%c", (atoi(tokens[1].c_str())+i)%255  );
		szVecTemp2.push_back( std::string(szTT) );
	}

	while( !szVecTemp2.empty() ) {
		int nOffset = Random.rand( (int)szVecTemp2.size() );
		szVecTemp.push_back( szVecTemp2[nOffset] );
		szVecTemp2.erase( szVecTemp2.begin() + nOffset );
	}

	if( atoi(tokens[1].c_str()) / 10 == nCountNum ) {
		for( int i=0 ; i<4; ++i )
		{
			if( nValue[i] < 0 ) continue;
			szResultValue += szVecTemp[nValue[i]].c_str();
		}
	}

	if( nCountNum % 6 == 0 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return RevertNumber1( nSeed, nValue, nCountNum, szResultValue );
	}
	else if( nCountNum % 6 == 1 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return RevertNumber2( nSeed, nValue, nCountNum, szResultValue );
	}
	else if( nCountNum % 6 == 2 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return RevertNumber3( nSeed, nValue, nCountNum, szResultValue );
	}
	else if( nCountNum % 6 == 3 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return RevertNumber4( nSeed, nValue, nCountNum, szResultValue );
	}
	else if( nCountNum % 6 == 4 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return RevertNumber5( nSeed, nValue, nCountNum, szResultValue );
	}
	else if( nCountNum % 6 == 5 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return RevertNumber6( nSeed, nValue, nCountNum, szResultValue );
	}
	return 5;
}

int CDnSecurityBase::RevertNumber6( const int nSeed, const int nValue[], int &nCountNum, std::string &szResultValue )
{
	nCountNum++;

	std::vector<std::string> szVecTemp, szVecTemp2;

	CMtRandom Random;

	std::vector<std::string> tokens;
	TokenizeA(std::string(SecondPassword::GetVersion()), tokens, ".");

	int nKey = rand();

	if( atoi(tokens[1].c_str()) / 10 == nCountNum )  {
		nKey = atoi(tokens[0].c_str()) + atoi(tokens[1].c_str());
		if( nKey % 2 ) nKey = -nKey;
	}
	else {
		nKey = atoi(tokens[0].c_str()) + atoi(tokens[1].c_str()) + atoi(tokens[2].c_str());
		nKey = nKey << 5;
		nKey = nKey >> 4;
	}

	int nSeed2 = timeGetTime();
	if( atoi(tokens[1].c_str()) / 10 == nCountNum )
		Random.srand( nSeed + nKey - ( nCountNum / 123449 ) );
	else Random.srand( nSeed2 + nKey - ( nCountNum / 6 ) );

	for( int i=0; i<10; i++ ) {
		char szTT[2];
		sprintf_s( szTT, "%c", (atoi(tokens[1].c_str())+i)%255  );
		szVecTemp2.push_back( std::string(szTT) );
	}

	while( !szVecTemp2.empty() ) {
		int nOffset = Random.rand( (int)szVecTemp2.size() );
		szVecTemp.push_back( szVecTemp2[nOffset] );
		szVecTemp2.erase( szVecTemp2.begin() + nOffset );
	}

	if( atoi(tokens[1].c_str()) / 10 == nCountNum ) {
		for( int i=0 ; i<4; ++i )
		{
			if( nValue[i] < 0 ) continue;
			szResultValue += szVecTemp[nValue[i]].c_str();
		}
	}

	if( nCountNum % 6 == 0 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return RevertNumber1( nSeed, nValue, nCountNum, szResultValue );
	}
	else if( nCountNum % 6 == 1 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return RevertNumber2( nSeed, nValue, nCountNum, szResultValue );
	}
	else if( nCountNum % 6 == 2 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return RevertNumber3( nSeed, nValue, nCountNum, szResultValue );
	}
	else if( nCountNum % 6 == 3 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return RevertNumber4( nSeed, nValue, nCountNum, szResultValue );
	}
	else if( nCountNum % 6 == 4 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return RevertNumber5( nSeed, nValue, nCountNum, szResultValue );
	}
	else if( nCountNum % 6 == 5 ) {
		if( atoi(tokens[1].c_str()) / 10 != nCountNum ) return RevertNumber6( nSeed, nValue, nCountNum, szResultValue );
	}
	return 6;
}



