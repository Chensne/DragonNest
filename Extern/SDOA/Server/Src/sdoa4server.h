/*
* Copyright (c) 2009,SDO Billing Center
* All rights reserved.
* 
* File name:sdoa4server.h
* Abstract:Definition of the interface function
* Current Version:2.0
* Developer:SNDA
* Completion date:2009-2-23
*
* Changes:2.0 final version of external interface
* Modified Time:2009-3-10

* Updates£ºAdd interface definition for InitializeEx and SendResponse
* Date   £º2009-06-23
*/
 
#ifndef SDOA4SERVER_H
#define SDOA4SERVER_H
 
 
#include "sndabase.h"
 
#define SDOA_ITEM_SEND_REQUEST 	0X01000001
#define SDOA_DEPOSIT_REQUEST 	0X01000003

interface ISDOAMsg : ISDUnknown
 
{
 
	//-------------------------------------------------------------------------------
	//Name:GetValue
	//Description:Get the specified value of the specified fields
	//Parameter:
	//	[in]szKey£º         data keyword
	//Return:                The contents of the corresponding keyword, return NULL if the keyword doed not exist
 
	//-------------------------------------------------------------------------------
	SNDAMETHOD(const char*)	GetValue(const char* szKey) = 0;
 
	//-------------------------------------------------------------------------------
	//Name:SetValue
	//Description:Add the specified value of the specified fields
	//Parameter:
	//	[in]szKey£º         data keyword
	//	[in]szValue£º       data value
	//Return:                0 means success,the others means failure(generally the data length overflow)
	//----------------------------------------------------------------------------------------
	SNDAMETHOD(int)	SetValue(const char* szKey, const char* szValue) = 0;
};
 
SNDAAPI(ISDOAMsg*) sdoaCreateMsg(void);
//-------------------------------------------------------------------------------
	//Name:UserInfoCallback
	//Description:identify information callback deal function,using the sdk to achieve
	//Parameter:
	//	[out]nResult£º          Return information result(please refer to the relevant error documents to find the specific error types)
	//	[out]nUniqueId£º        Related ID
	//	[in/out]pMsg£º          Information analyse and set interface
	//Return:none
	//-------------------------------------------------------------------------------
 
typedef int (SNDACALLBACK *UserInfoCallback)(int nResult, unsigned long nUniqueId, 
 
                                             ISDOAMsg *pMsg);

interface ISDOAUserInfoAuthen : ISDUnknown
 
{
 
	//-------------------------------------------------------------------------------
	//Name:Initialize
	//Description:Initialization
	//Parameter:
	//	[in]szConfPath:    Configuration file,if it is NULL then use default sdoa4server.ini configuration file
	//	[in]pCallbackFunc:callback function address
	//Return:                0 means success;!0 means failure;
	//-------------------------------------------------------------------------------
 
	SNDAMETHOD(int) Initialize(const char *szConfPath, UserInfoCallback pCallbackFunc
                                   ) = 0;
 	
	
	//-------------------------------------------------------------------------------
	//Name:AsyncGetUserInfo
	//Description£ºCertified results of asynchronous authentication (Through Initialize to set callback function)
	//Parameter:
	//	[in]szSessionId:   Token or SeccionID which need to be verified
	//	[in]szEndpointIp:  Client IP
	//	[in]nUniqueId£º     token's unique id
	//Return£º                0 success;!0 failure;
 
	//-------------------------------------------------------------------------------
 
	SNDAMETHOD(int) AsyncGetUserInfo(const char *szSessionId, const char *szEndpointIp,
                                          const unsigned long nUniqueId
                                         ) = 0;
 
	//Name:SyncGetUserInfo
	//Description£ºCertified results of synchronized authentication 
	//Parameter:
	//	[in]szSessionId:   Token or SessionId which should be verified
	//	[in]szEndpointIp:  Client IP
	//	[in]nUniqueId:     token's unique id
	//	[in/out]pMsg:      returned user information		
	//Return£º                0 means success;!0 means failure;
 
	//-------------------------------------------------------------------------------
 
	SNDAMETHOD(int) SyncGetUserInfo(const char *szSessionId, const char *szEndpointIp, 
                                         const unsigned long nUniqueId, ISDOAMsg* pMsg
                                        ) = 0;
 
};
 
SNDAAPI(ISDOAUserInfoAuthen*) sdoaCreateUserInfoAuthen(void);
interface ISDOARSASign : ISDUnknown
{
	//----------------------------------------------------------------------------------------
	//Name£ºInitialPriKey
	//Description£ºThe private key location when signed for the specific order. Initialize RSA private key interface
	//Parameter£º
	//	[in]szPriKeyFile£º  file name of private key(Full path or relative path)
	//Return£º                0 means success;!0 means failure;
	//----------------------------------------------------------------------------------------
	SNDAMETHOD(int) InitialPriKey(const char *szPriKeyFile) = 0;
	
	//----------------------------------------------------------------------------------------
	//Name£ºSign
	//Description£ºsign for the order parameter based on customer private key, get signature string
	//Parameter£º
	//	[in]szData£º        the data to sign
	//	[in]nDataLength£º   the data length to sign 
	//	[out]szSignature£º  signature,length 512Byte.
	//Return£º                0 means success£»!0 means failure£»
	//----------------------------------------------------------------------------------------
	SNDAMETHOD(int) Sign(const char *szData, int nDataLength, char* szSignature) = 0;
	
	//----------------------------------------------------------------------------------------
	//Name£ºInitialPubKey
	//Description£ºthe required public key file location when verify the specific order signature, initialize RSA public key verification interface
	//Parameter£º
	//	[in]szPubKeyFile£º  public file name(ful path or relative path)
	//Returen£º                0 means success£»!0 means failure£»
	//----------------------------------------------------------------------------------------
	SNDAMETHOD(int) InitialPubKey(const char *szPubKeyFile) = 0;
	
	//----------------------------------------------------------------------------------------
	//Name£ºVerifySignature
	//Description£ºverify item sending notification message post by SNDA billing interface based on SNDA public key
	//Parameter£º
	//	[in]szData£º        the data to verify
	//	[in]nDataLength£º   the data length to verify 
	//	[in]szSignature£º   signature to verify
	//Return£º                0 means success£»!0 means failure£»
	//----------------------------------------------------------------------------------------
	SNDAMETHOD(int) VerifySignature(const char *szData,int nDataLength,const char* szSignature) = 0;
};
SNDAAPI(ISDOARSASign*) sdoaCreateRSASign(void);

//---------------------------------------------------------------------------------------
//Name:RecvMsgCallback
	//Description£ºOrder information callback deal function, using the sdk to achieve
	//Parameter:
	//	nMsgType:    message type:There are currently two type of news, item and charging, please refer to itemproto.h for detail definition
	//	pRequest£º    Request message object: please refer to interface of ISDOAMsg and protocol for detail content
	//	pResponse£º  Reponse after callback function processing: please refer to interface of ISDOAMsg and protocol for detail content

	//Return:                  none
 
	//-------------------------------------------------------------------------------
 
typedef int (SNDACALLBACK *RecvMsgCallback)(int nMsgType, ISDOAMsg* pRequest,
                                            ISDOAMsg* pResponse);
//---------------------------------------------------------------------------------------
//Name£ºAsyncMsgCallback
//Description£ºCallback function handles OrderInfo. Follow SDK to implement this function. SendResponse should be used to send response upon receiving message
//Parameter£º
//	nMsgType£º              Message Type£ºWhen sending response, this parameter is mandatory
//	nMsgFrom£º              Message Source£ºWhen sending response, this parameter is mandatory
//	nMsgSeq£º               Message Sequence£ºWhen sending response, this parameter is mandatory
//Return£º                    None
//----------------------------------------------------------------------------------------
typedef int (SNDACALLBACK *AsyncMsgCallback)(int nMsgType, int nMsgFrom, int nMsgSeq, ISDOAMsg* pRequest);
 
interface ISDOAOrderHandler : ISDUnknown
 
{
 
	//-------------------------------------------------------------------------------
	//Name:Initialize
	//Description:initialize QFT client
	//Parameter:
	//	[in]szConfPath:    Configuration file,if it is NULL then use default sdoa4server.ini configuration file
	//	[in]pCallbackFunc: Message handling callback function
	//Return£º                0 means success£»!0 means failure
	//                      -10225001 Configuration file does not exist
	//                      -10225002 Failed to create authorized configuration file
	//                      -10225003 Authorize the SDK initialization failed
	//                      -10225004 Failed to create network
	//                      -10225005 Failed to create local listening STOCKET
	//                      -10225006 Failed to create callback line	
//----------------------------------------------------------------------------------------
	SNDAMETHOD(int) Initialize(const char *szConfPath, RecvMsgCallback pCallbackFunc) = 0;

	//----------------------------------------------------------------------------------------
	//Name£ºInitializeEx
	//Description£ºInitialize client side component. InitializeEx will call async functon to send response later.
	//Parameter£º
	//	[in]szConfPath£º    Configuration file,if it is NULL then use default sdoa4server.ini configuration file¼þ
	//	[in]pCallbackFunc£º callback function to handle message
	//Return£º                0 means success£»!0 means failed
	//						-10225000 Initializing is already done
	//                      -10225001 Configuration file does not exist
	//                      -10225002 Failed to create authorizing config file
	//                      -10225003 Initialize authorizing SDK failed
	//                      -10225004 Failed to create networking dispatch thread
	//                      -10225005 Failed to create local listening socket
	//                      -10225006 Failed to create thread which deals with callback
	//----------------------------------------------------------------------------------------
	SNDAMETHOD(int) InitializeEx(const char *szConfPath, AsyncMsgCallback pCallbackFunc) = 0;
	
	//----------------------------------------------------------------------------------------
	//Name:CreateOrderInfo
	//Description:Create order
	//Parameter:
	//	[in]nType£º         1.billing information,2.item deliver information
	//Return:                order information,0 failure
	//                      !0 is ULONG object cursor
	//-------------------------------------------------------------------------------
 
	SNDAMETHOD(ULONG) CreateOrderInfo(const int nType) = 0;
	
	//----------------------------------------------------------------------------------------
	//Name:DestroyOrderInfo
	//Description:Destruction Order
	//Parameter:
	//	[in]pOrderInfo£º    Billing information
	//Return£º                0 means success,no others
 
	//-------------------------------------------------------------------------------
 
	SNDAMETHOD(int) DestroyOrderInfo(ULONG pOrderInfo) = 0;
	
	//----------------------------------------------------------------------------------------
	//Name:SetOrderInfoValue
	//Description:Add value to the order
	//Parameter:
	//	[in]pOrderInfo:    Order information
	//	[in]szKey[in]£º     Data keyword
	//	[in]szValue£º       Data value
	//Return£º0               success ,no others
 
	//-------------------------------------------------------------------------------
 
	SNDAMETHOD(int) SetOrderInfoValue(ULONG pOrderInfo, const char *szKey, 
                                           const char *szValue
                                          ) = 0;
 
	//----------------------------------------------------------------------------------------
	//Name:BuildOrderUrl
	//Description£ºSign order information,and return the contents of the corresponding http request
	//Parameter:
	//	[in]pOrderInfo:    Billing object
	//	[out]szUrl:        Http request URL,the maximum 2048bytes such as:char szUrl[2048]
	//Return£º                0 means success,no others
	//-------------------------------------------------------------------------------
 
	SNDAMETHOD(int) BuildOrderUrl(ULONG pOrderInfo, char *szUrl) = 0;

	//----------------------------------------------------------------------------------------
	//Name:SignData
	//Description:Packet signature
	//Parameter:
	//	[in]szData:        Data which should be signed
	//	[in]nDataLength:  Data length which should be signed 
	//	[out]szSignature: Return 32 bytes' long data pocket signature,32 bytes' tail add '\0'£¬
          //                               means need 33 bytes such as char szUrl[33]
         //Return£º                0 means success;!0 means failed
	//                      -10225007 failed
 
	//-------------------------------------------------------------------------------
 
	SNDAMETHOD(int) SignData(const char* szData, const int nDataLength, 
                                  char* szSignature
                                 ) = 0;

	//----------------------------------------------------------------------------------------
	//Name:VerifySignature
	//Description:Check the signature packet
	//Parameter:
	//	[in]szData:        unverified source data
	//	[in]nDataLength:   unverified data length
	//	[in]szSignature:   unverified signature
	//Return£º                0 success;!0 failure;
	//                      -10225008	failed
	//-------------------------------------------------------------------------------
 
	SNDAMETHOD(int) VerifySignature(const char* szData, const int nDataLength, const char* szSignature) = 0;

	//----------------------------------------------------------------------------------------
	//Name:GetUniqueId
	//Description:get the sole ID
	//Parameter:
	//	[out]szUniqueId£º   the sole ID output addess,32bytes and at the end you need add'\0',means you need 33 bytes of space, 
       //       example:char szUrl[33]
	//Return£º                0 means success£»!0 means failure;
	//                      -10225009	the parameter is NULL
 
	//-------------------------------------------------------------------------------
 
	SNDAMETHOD(int) GetUniqueId(char *szUniqueId) = 0;
	
	//---------------------------------------------------------------------------------------
	//Name£ºSendResponse
	//Description£ºSend response asnychronous
	//Parameter£º
	//	nMsgType£º              AsyncMsgCallback message type obtained during callback stage
	//	nMsgFrom£º              AsyncMsgCallback Message Source£ºWhen sending response, this parameter is mandatory
	//	nMsgSeq£º               AsyncMsgCallback Message Sequence£ºWhen sending response, this parameter is mandatory
	//Return£º                    0 Success; !0 Fail; 
	//-10225010	Null, OrderID does not exist
	//-10225011	Network disconncted	
        //-10225012	Message source doest not exist. This usually indicate network failure
	//----------------------------------------------------------------------------------------
	SNDAMETHOD(int) SendResponse(int nMsgType, int nMsgFrom, int nMsgSeq, ISDOAMsg* pResponse) = 0;

	//----------------------------------------------------------------------------------------
	//Name:UnInitialize
	//Description:Uninstall QFT client
	//Parameter:
	//	None
	//Return£º                0 means success;no others;
	//-------------------------------------------------------------------------------
 
	SNDAMETHOD(int) UnInitialize() = 0;
};
SNDAAPI(ISDOAOrderHandler*) sdoaCreateOrderHandler();

#endif
