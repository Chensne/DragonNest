// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �� ������Ʈ ���� ���� ������
// ��� �ִ� ���� �����Դϴ�.
//

#pragma once


#define WIN32_LEAN_AND_MEAN		// ���� ������ �ʴ� ������ Windows ������� �����մϴ�.

// TODO: ���α׷��� �ʿ��� �߰� ����� ���⿡�� �����մϴ�.
#pragma warning( disable:4996 )

#define _WIN32_WINNT 0x0501

#include <windows.h>
#include <stdio.h>
#include <string.h>

#include <vector>
#include <map>
#include <string>
#include <xstring>
using namespace std;

typedef std::basic_string< TCHAR > tstring;

#pragma warning(disable:4819)
#define DIRECTINPUT_VERSION 0x0800

#if defined(_DEBUG) || defined(_RDEBUG)
#ifndef D3D_DEBUG_INFO
#define D3D_DEBUG_INFO
#endif
#endif
#include <d3d9.h>
#include <d3dx9.h>
#include <dinput.h>

#pragma warning(default:4819)

#include "../DNDefine.h"
#include "AssertX.h"
#include "Singleton.h"
#include "SmartPtr.h"
#include "Stream.h"
#include "SundriesFunc.h"


#include "EtMathWrapperD3D.h"
#include "EtPrimitive.h"
#include "EtCollisionPrimitive.h"
