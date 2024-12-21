#ifndef __SDOGSDK_H__
#define __SDOGSDK_H__

namespace OpenGame
{
	/**
	* @defgroup groupogsdk 游戏开放平台游戏端SDK
	* @ingroup  OpenGame
	* @{
	*/

#ifdef WIN32
#define SDAPI  __stdcall
#endif

	/**
	* @brief 游戏开放平台的通讯协议格式：JSON
	*/
	const static char * OPENGAME_FORMAT_JSON = "JSON";

	/**
	* @brief 游戏开放平台的通讯协议格式：XML
	*/
	const static char * OPENGAME_FORMAT_XML  = "XML";

	/**
	* @brief 游戏开放平台游戏端SDK的属性接口
	*/
	class ISDProperty
	{
		public:
			virtual ~ISDProperty() {};

			/**
			* @brief 添加整型Key-Value属性
			* @param pProp : 属性名称
			* @param value : 属性值
			* @return 返回0为添加成功，返回负数代表添加失败
			* @remark 如果原来有相同的属性存在，直接覆盖
			*/
			virtual int SDAPI AddIntValue(const char * pProp, int value) =0;

			/**
			* @brief 添加浮点型Key-Value属性
			* @param pProp : 属性名称
			* @param value : 属性值
			* @return 返回0为添加成功，返回负数代表添加失败
			* @remark 如果原来有相同的属性存在，直接覆盖
			*/
			virtual int SDAPI AddDoubleValue(const char * pProp, double value) =0;

			/**
			* @brief 添加字符串型Key-Value属性
			* @param pProp : 属性名称
			* @param value : 属性值
			* @return 返回0为添加成功，返回负数代表添加失败
			* @remark 如果原来有相同的属性存在，直接覆盖
			*/
			virtual int SDAPI AddStringValue(const char * pProp, const char * pValue) =0;

			/**
			* @brief 添加属性型Key-Value属性
			* @param pProp : 属性名称
			* @param value : 属性
			* @return 返回0为添加成功，返回负数代表添加失败
			* @remark 如果原来有相同的属性存在，直接覆盖
			*/
			virtual int SDAPI AddPropValue(const char * pProp, ISDProperty * pValue) =0;

			/**
			* @brief 获取整型Key-Value属性
			* @param pProp : 属性名称
			* @param defVal : 缺省属性值
			* @return 返回该属性的值
			*/
			virtual int SDAPI GetIntValue(const char * pProp, int defVal =0) =0;

			/**
			* @brief 获取浮点型Key-Value属性
			* @param pProp : 属性名称
			* @param defVal : 缺省属性值
			* @return 返回该属性的值
			*/
			virtual double SDAPI GetDoubleValue(const char * pProp, double =0.0) =0;
			
			/**
			* @brief 获取字符串型Key-Value属性
			* @param pProp : 属性名称
			* @param defVal : 缺省属性值
			* @return 返回该属性的值
			*/
			virtual const char * SDAPI GetStringValue(const char * pProp, const char * defVal =0) =0;

			/**
			* @brief 获取属性型Key-Value属性
			* @param pProp : 属性名称
			* @param defVal : 缺省属性值
			* @return 返回该属性的值
			*/
			virtual ISDProperty * SDAPI GetPropValue(const char * pProp) =0;
	};

	/**
	* @brief 获取一个Property的实例，内存由sdk管理，释放时必须调用SDReleaseProperty
	* @return 返回的Property的实例
	*/
	ISDProperty * SDAPI SDCreateProperty();

	/**
	* @brief 释放从SDK申请的Property内存
	* @param pProp : 需要释放的Property
	*/
	void SDAPI SDReleaseProperty(ISDProperty * pProp);


	class ISDOGService;

	/**
	* @brief 消息处理回调函数声明
	* @param pService : OgService
	* @param pMethod : 方法名
	* @param pBody : 应用请求中的属性
	*
	*/
	typedef void (*PFSDMessageProc)(
			ISDOGService * pService,
			const char * pMethod,
			ISDProperty * pBody);


	/**
	* @brief OG Service，游戏开放平台游戏端的SDK服务
	* 
	*/
	class ISDOGService
	{
		public:
			virtual ~ISDOGService() {};

			/**
			* @brief 初始化游戏SDK服务，缺省情况下，支持重连，重连间隔时间为1秒
			* @param pfMsgProc : 注册的消息回调函数
			* @param pHost : GameChannel服务器地址,格式为IP,IP或者IP:Port,IP:Port:Port
			* @param port : GameChannel服务器端口
			* @param pMatrixId : 游戏ID，包含游戏，区，组信息
			* @param pDeviceId : 游戏服务器设备号
			* @param bLog : true为记录日志，false为不记录日志
			* @return 返回0为初始化成功，返回负数初始化失败
			*/
			virtual int SDAPI Init(
					PFSDMessageProc pfMsgProc,
					const char * pHost,
					int          port,
					const char * pMatrixId,
					const char * pDeviceId,
					bool bLog = false) =0;

			/**
			* @brief 清理游戏SDK服务
			* @return  返回0为清理成功，返回负数清理失败
			*/
			virtual int SDAPI Uninit() =0;

			/**
			* @brief 运行游戏SDK服务，每运行一次，都将执行一定数量的请求
			* @return 执行请求的数量
			*/
			virtual int SDAPI Run() =0;

			/**
			* @deprecated 已废弃的接口，请不要再使用，本接口仅供版本兼容
			* @brief 发送一个数据包到对应的通道
			* @param pAppId : 应用ID
			* @param pMatrixId : 游戏ID，包含游戏，区，组信息
			* @param pSeqId : 请求序列号，每个请求唯一，响应时需要带回
			* @param pMethod : 方法名
			* @param pVersion : 应用接口版本号
			* @param pFormat : 协议格式，JSON或XML
			* @param pBody : 响应结果
			* @return 返回0为发送成功，返回负数为发送失败:
			*/
			virtual int SDAPI SendPacket(
					const char * pAppId,
					const char * pMatrixId,
					const char * pSeqId,
					const char * pMethod,
					const char * pVersion,
					const char * pFormat,
					ISDProperty * pBody) =0;

			/**
			* @brief 发送一个数据包到对应的通道
			* @param pBody : 响应结果
			* @return 返回0为发送成功，返回负数为发送失败:
			*/
			virtual int SDAPI SendPacket(ISDProperty * pBody) =0;

			/**
			* @brief 释放此游戏SDK服务
			*/
			virtual void SDAPI Release() =0;

			/**
			* @brief 是否使用重连
			* @param bReconnect : true为允许重连，false为不允许重连
			* @return 返回true为修改成功，返回false为修改失败
			*/
			virtual bool SDAPI UseReconnect(bool bReconnect) =0;

			/**
			* @brief 设置重连的时间
			* @param ReconnectInterval : 单位毫秒
			* @return 返回true为修改成功，返回false为修改失败
			*/
			virtual bool SDAPI SetReconnectInterval(long ReconnectInterval) =0;
	};

	/**
	* @brief 获取一个游戏SDK服务
	* @return 返回的游戏SDK服务，如果返回失败，返回NULL
	*/
	ISDOGService * SDAPI SDGetOGService();
}


#endif //