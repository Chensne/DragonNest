

// 서비스 매니저 / 모니터 버전관리 헤더


#pragma once


const char g_SMBuildVersionMajor		= 0;		// 주 버전 번호
const char g_SMBuildVersionMinor		= 2;		// 부 버전 번호
const short g_SMBuildVersionRevision	= 2;		// 세부 빌드 번호

const short g_SMBuildTimeYear			= 2012;		// 빌드 시간 (년)
const char g_SMBuildTimeMonth			= 1;		// 빌드 시간 (월)
const char g_SMBuildTimeDay				= 3;		// 빌드 시간 (일)
const char g_SMBuildTimeHour			= 17;		// 빌드 시간 (시)
const char g_SMBuildTimeMinute			= 10;		// 빌드 시간 (분)


typedef	int		TP_SMVERSION;			// GET_SM_VERSION() 의 버전 결과값을 관리하는 타입

#define GET_SM_VERSION()				MAKELONG(g_SMBuildVersionRevision, MAKEWORD(g_SMBuildVersionMinor, g_SMBuildVersionMajor))

