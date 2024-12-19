#ifndef __SDOPENAPI_H__
#define __SDOPENAPI_H__

namespace SDOpenApi
{
	/**
	* @defgroup groupogsdk ��Ϸ����ƽ̨��Ϸ��SDK
	* @ingroup  SDOpenApi
	* @{
	*/
#ifdef WIN32
#define SDAPI  __stdcall
#endif
	/**
	* @brief ��Ϸ����ƽ̨��Ϸ��SDK�����Խӿ�
	*/
	class ISDProperty
	{
	public:
		/**
		* @brief �������Key-Value����
		* @param pProp : ��������
		* @param value : ����ֵ
		* @return ����0Ϊ��ӳɹ������ظ����������ʧ��
		* @remark ���ԭ������ͬ�����Դ��ڣ�ֱ�Ӹ���
		*/
		virtual int SDAPI AddIntValue(const char * pProp, int value) =0;

		/**
		* @brief ��Ӹ�����Key-Value����
		* @param pProp : ��������
		* @param value : ����ֵ
		* @return ����0Ϊ��ӳɹ������ظ����������ʧ��
		* @remark ���ԭ������ͬ�����Դ��ڣ�ֱ�Ӹ���
		*/
		virtual int SDAPI AddDoubleValue(const char * pProp, double value) =0;

		/**
		* @brief ����ַ�����Key-Value����
		* @param pProp : ��������
		* @param value : ����ֵ
		* @return ����0Ϊ��ӳɹ������ظ����������ʧ��
		* @remark ���ԭ������ͬ�����Դ��ڣ�ֱ�Ӹ���
		*/
		virtual int SDAPI AddStringValue(const char * pProp, const char * pValue) =0;

		/**
		* @brief ���������Key-Value����
		* @param pProp : ��������
		* @param value : ����
		* @return ����0Ϊ��ӳɹ������ظ����������ʧ��
		* @remark ���ԭ������ͬ�����Դ��ڣ�ֱ�Ӹ���
		*/
		virtual int SDAPI AddPropValue(const char * pProp, ISDProperty * pValue) =0;

		/**
		* @brief ��ȡ����Key-Value����
		* @param pProp : ��������
		* @param defVal : ȱʡ����ֵ
		* @return ���ظ����Ե�ֵ
		*/
		virtual int SDAPI GetIntValue(const char * pProp, int defVal =0) =0;

		/**
		* @brief ��ȡ������Key-Value����
		* @param pProp : ��������
		* @param defVal : ȱʡ����ֵ
		* @return ���ظ����Ե�ֵ
		*/
		virtual double SDAPI GetDoubleValue(const char * pProp, double =0.0) =0;

		/**
		* @brief ��ȡ�ַ�����Key-Value����
		* @param pProp : ��������
		* @param defVal : ȱʡ����ֵ
		* @return ���ظ����Ե�ֵ
		*/
		virtual const char * SDAPI GetStringValue(const char * pProp, const char * defVal =0) =0;

		/**
		* @brief ��ȡ������Key-Value����
		* @param pProp : ��������
		* @param defVal : ȱʡ����ֵ
		* @return ���ظ����Ե�ֵ
		*/
		virtual ISDProperty * SDAPI GetPropValue(const char * pProp) =0;


		/**
		* @brief �ͷŴ�Property
		*/
		//virtual void SDAPI Release() =0;
	};

	/**
	* @brief ��ȡһ��Property��ʵ�����ڴ���sdk�����ͷ�ʱ�������SDReleaseProperty
	* @return ���ص�Property��ʵ��
	*/
	__declspec(dllexport) ISDProperty * SDAPI  SDCreateProperty();

	/**
	* @brief �ͷŴ�SDK�����Property�ڴ�
	* @param pProp : ��Ҫ�ͷŵ�Property
	*/
	__declspec(dllexport) void SDAPI  SDReleaseProperty(ISDProperty * pProp);


	class ISDOpenApiSdk;

	/**
	* @brief ��Ϣ����ص���������
	* @param pService : OgService
	* @param pAppId : Ӧ��ID
	* @param pMatrixId : ��ϷID��������Ϸ����������Ϣ
	* @param pSeqId : �������кţ�ÿ������Ψһ����Ӧʱ��Ҫ����
	* @param pMethod : ������
	* @param pVersion : Ӧ�ýӿڰ汾��
	* @param pBody : Ӧ�������е�����
	*
	*/
	typedef void (*PFSDMessageProc)(
		ISDOpenApiSdk * pService,
		const char * pAppId,
		const char * pSeqId,
		const char * pMethod,
		const char * pVersion,
		ISDProperty * pBody);


	/**
	* @brief OG Service����Ϸ����ƽ̨��Ϸ�˵�SDK����
	* 
	*/
	class ISDOpenApiSdk 
	{
	public:
		/**
		* @brief ��ʼ��Open API SDK����
		* @param pfMsgProc : ע�����Ϣ�ص�����
		* @return ����0Ϊ��ʼ���ɹ������ظ�����ʼ��ʧ��
		*/
		virtual int SDAPI Init(
			PFSDMessageProc pfMsgProc, 
			const char * pConfigFile) =0;

		/**
		* @brief ������ϷSDK����
		* @return  ����0Ϊ����ɹ������ظ�������ʧ��
		*/
		virtual int SDAPI Uninit() =0;

		/**
		* @brief ������ϷSDK����ÿ����һ�Σ�����ִ��һ������������
		* @return ִ�����������
		*/
		virtual int SDAPI Run() =0;


		/**
		* @deprecated �ѷ����Ľӿڣ��벻Ҫ��ʹ�ã����ӿڽ����汾����
		* @brief ����һ�����ݰ�����Ӧ��ͨ��
		* @param pAppId : Ӧ��ID
		* @param pSeqId : �������кţ�ÿ������Ψһ����Ӧʱ��Ҫ����
		* @param pMethod : ������
		* @param pVersion : Ӧ�ýӿڰ汾��
		* @param pBody : ��Ӧ���
		* @return ����0Ϊ���ͳɹ������ظ���Ϊ����ʧ��:
		*/
		virtual int SDAPI SendPacket(
			const char * pAppId,			
			const char * pSeqId,
			const char * pMethod,
			const char * pVersion,
			ISDProperty * pBody) =0;

		/**
		* @brief ����һ�����ݰ�����Ӧ��ͨ��
		* @param pBody : ��Ӧ���
		* @return ����0Ϊ���ͳɹ������ظ���Ϊ����ʧ��:
		*/
		//virtual int SDAPI SendPacket(ISDProperty * pBody) =0;


		/**
		* @brief �ͷŴ���ϷSDK����
		*/
		virtual void SDAPI Release() =0;
	};

	/**
	* @brief ��ȡһ����ϷSDK����
	* @return ���ص���ϷSDK�����������ʧ�ܣ�����NULL
	*/
	__declspec(dllexport) ISDOpenApiSdk * SDAPI SDGetOpenApiSdk();

	

	
}


#endif //
