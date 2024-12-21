#pragma once
#include <string>
using namespace std;

class CVisitURL
{
public:
	CVisitURL(void);
	~CVisitURL(void);

	// 绑定 解绑 修改绑定的子应用的接口 返回值是一个错误代码。 返回-1说明读取URL失败 -2说明解析返回值失败
	/*
	绑定
	---------------------
	0	成功
	1	用户不存在
	2 	已绑定密宝
	3 	参数错误
	4 	绑定失败

	解绑
	---------------------
	0	成功
	1	用户不存在
	2 	未绑定密宝
	3 	参数错误
	4 	解绑失败

	修改绑定的子应用
	---------------------
	0	成功
	1	用户不存在
	2 	未绑定密宝
	3 	参数错误
	4 	修改失败
	*/
	static int BindEkey(const char* szURL, const char* szPTID, const char* szEKeyID, const char* szDPwd, const char* szPwd, const char* szChallenge, const char* szType);
	static int UnBindEkey(const char* szURL, const char* szPTID, const char* szEKeyID, const char* szDPwd, const char* szPwd, const char* szEKeyServiceNum, const char* szChallenge);
	static int ChangeBindApp(const char* szURL, const char* szPTID, const char* szGameType, const char* szAppType, const char* szDPwd, const char* szPwd, const char* szChallenge);

	static int ChangePwd(const char* szURL, const char* szPTID, const char* szPWD, const char* szIP);

protected:
	static int GetUrlOnce(LPCTSTR url, string &szResult);
};
