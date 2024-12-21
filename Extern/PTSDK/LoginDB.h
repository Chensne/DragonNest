#pragma once
#include "sdkconfig.h"
#include <string>
using namespace std;

class PTSDK_STUFF CLoginDB
{
public:
///enum IdType{ptid = 0, gmid = 1, sdid = 2};
	enum { cantquery=0, nodata, gotdata };
	struct S_UserInfo {

			int    areaNum;
			string ptid;
			string gmid;
			string originSdid;
			string userSdid;
			string password; 
			string ekeyflag;
			string reservePwd;
			int	   status;

			string ekeytype;
			string ekeyid;
			string subapp;

			string ecardno;
			int    ecardApp;
			int    ecardType;
			string emailAccount;
	};

	virtual void SaveUserInfo( S_UserInfo & userInfo) = 0;	
	virtual int  GetUserInfo(int idtype, const char *id,S_UserInfo & userInfo) = 0;

/*==================================================================
`gamearea`		Int			游戏区号							/
`userid`		Varchar	50	PT帐号								加密
`gameid`		Varchar	50	游戏帐号							/
`originsdid`	Bigint		基本数字账号						/
`usersdid`		Varchar	50	个性化数字账号						加密
`passwd`		Varchar	50	登陆密码							加密
`ekeyflag`		Int			[密宝状态类型]						/
							0:该帐号未开通任何密宝应用
							1:该帐号登陆绑定了密宝
							2:该帐号仓库绑定了密宝
							3:该帐号登陆与仓库都绑定了密宝		
`EkeyPass`		Varchar	50	密宝保留密码（用户服务卡的后六位）	加密
`status`		Int			允许登陆状态						/
							0表示可以登陆，其它都表示拒绝登陆	
`EkeyType`		Tinyint		[密宝类型]							/
							0：没有绑定密宝（密宝已解绑）
							1：随机数机制的D6或者E8密宝
							2：不允许使用（全码D6/E8）
							3：挑战码机制的A8密宝	
`EkeyID`		Varchar	50	密宝号（即密宝背面SN）				加密
`SubApp`		Varchar	50	子应用字符串						/
`ECardNo`		Varchar	50	安全卡卡号							加密
`ECardApp`		Tinyint		安全卡绑定应用类型：				/
							0:该帐号未开通任何安全卡应用
							1:该帐号登陆绑定了安全卡
							2:该帐号仓库绑定了安全卡
							3:该帐号登陆与仓库都绑定了安全卡	
`ECardType`		Tinyint		安全卡卡的类型：					/
							1：手机安全卡（5*8）
							2：实物卡（8*15）	
`EMailAccount`	Varchar	50	邮箱账号							加密
====================================================================*/

	static string FormateSubApp(bool v_gold, bool v_cdlogin, bool v_cdstore, bool v_limit, bool v_protect, bool v_refer, bool v_mobile, bool v_TGY);
};
