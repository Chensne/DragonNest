#pragma once
/// \file ImmigratorSDKErrorCodes.h
/// \breif ImmigratorSDKErrorCodes Enumeration 


/// ImmigratorErrors
/// \brief Cash Broker HTTP & Request Error Codes
enum ImmigratorErrors
{
	/// Immigrator_HTTP_NONE
	/// \breif Initial State, Can not be used for error code
	Immigrator_HTTP_NONE = 0 
	/// Immigrator_OK
	/// \breif Success - No Error
	, Immigrator_OK  = 5000 
	/// Immigrator_OK, please have additional event information
	/// \breif Success - No Error
	, Immigrator_OK_with_Custom_Event_Information = 5001 
	/// ImmigratorServer_UnhandledException
	/// \brief unhandled exception happened
	, ImmigratorServer_Unhandled_Exception = 5501
	/// ImmigratorServer_Database_System_Error
	/// \brief unhandled exception happened
	, ImmigratorServer_Database_System_Error = 5502
	/// ImmigratorServer_Unknow_Error
	/// \brief The request can not be completed because unknown error happened
	, ImmigratorServer_Unknow_Error
	/// ImmigratorServer_System_Error
	/// \brief The request can not be completed because system error happened
	, ImmigratorServer_System_Error
	/// Immigrator_HTTP_Open_Failed
	/// \brief The request can not be completed because the system can not use TCP/IP resource
	, Immigrator_HTTP_Open_Failed = 6001// can't use TCP/ IP resource 
	/// Immigrator_HTTP_Connection_Failed
	/// \brief The request can not be completed because the target system is down or actively refusing connections 
	, Immigrator_HTTP_Connection_Failed // can't connect the designated server
	/// Immigrator_HTTP_Request_Faild
	/// \brief The request can not be completed because sending request failed
	, Immigrator_HTTP_Request_Faild // HTTP Request failed
	/// Immigrator_HTTP_Receive_Faild
	/// \brief The request can not be completed because receving response failed
	, Immigrator_HTTP_Receive_Faild // HTTP Request failed
	/// Immigrator_HTTP_Timeout
	/// \brief The request can not be completed because the action could not complete in specific time
	, Immigrator_HTTP_Timeout
	/// Immigrator_HTTP_Error
	/// \brief The request can not be completed because Unknown Http error happened
	, Immigrator_HTTP_Error
	/// Immigrator_Response_Parse_Failed
	/// \brief The request can not be completed because response parse failed
	, Immigrator_Response_Parse_Failed

	/// Immigrator Action failed
	/// \brief Should be need to see the detail error code  & description
	, Immigrator_Action_Failed

	/// Immigrator Buffer Too small
	/// 
	, Immigrator_Buffer_Too_Small
};