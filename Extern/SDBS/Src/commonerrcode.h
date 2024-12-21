#ifndef _COMMON_ERRCODE_H_
#define _COMMON_ERRCODE_H_

enum
{
	BSIP_OK			  = 0,
    SOCKDISCONNECT    = -401001,  //socket连接断开
    SOCKIP_OR_PORTERR = -401002,  //服务器的ip port配置错误
    SOCKCONNECTFAIL   = -401003,  //sock连接失败
	SOCKLOCALFAILED	  = -401004,	//本地初始化SOCKET失败
	SNDBUF_OVERFLOW	  = -401005,  //发送缓存满
	RCVBUF_OVERFLOW	  = -401006,  //接收缓存满
	TMOBUF_OVERFLOW	  = -401007,	//超时缓存满
	LSOCKDISCONNECT	  = -401008,  //通讯异常		
    NOINITCLIENT      = -401011,  //没有初始化的客户端

    QUEUECACHEFULL    = -402001,  //队列缓存数满
    QUEUELOCKERROR    = -402002,  //队列锁定错误
    QUEUEOPERATEERROR = -402003,  //队列Push Pop传入参数错(空指针)
    QUEUEEMPTY        = -402004,  //队列空，Pop无数据
    QUEUEDATAERR      = -402005,  //缓存数据的内容错，如长度大于小于可缓存的范围

    USERDATALENERROR  = -403001,  //用户数据Push长度和业务结构的长度不一直
    COMMUDATAERROR    = -403002,  //接收的通讯数据包加密错

    POINTERNULL       = -404001,  //传入了空指针(需要指针的地方是不能接收空指针的)

    ENCRYPTERROR      = -405001,  //加密错
    DECRYPTERROR      = -405002,  //解密错

    PARAMPATHERROR    = -406001,  //缓存或者环形文件路径错误
	CONFLOADFAILED	  = -406002,  //配置文件加载失败
	CONFGETFAILED	  = -406003,	//从配置服务器获取配置失败
	RELOADFAILED	  = -406004,  //加载Re失败
    ITEMLOGINITFAILD   = -406005,
    ITEMGATHERSTARTFAILD= -406006,  //启动道具采集客户端失败
	
	BSIP_BE_START_FAILD			=-407001,//引擎启动失败
	BSIP_BE_LOGIN_FAILD			=-407002,//用户登入失败
	BSIP_BE_LOGOUT_FAILD		=-407003,//用户注销失败
	BSIP_BE_CHANGEMAP_FAILD		=-407004,//用户跳地图失败
	BSIP_BE_USER_ALREADY_EXIST	=-407005,//用户已经存在引擎中
	BSIP_BE_USER_NOT_EXIST		=-407006,//用户不存在
	BSIP_BE_MEMORY_ERROR		=-407007,//引擎分配内存错误

    USERIDTYPEERROR   = -408001,

    PUSHBUFF          = 900001   //放入缓存正确，此返回值表示发送成功，非错误代码
};

#define BSIP_BE_OK		0

#endif
