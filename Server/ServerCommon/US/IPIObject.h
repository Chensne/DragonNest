#pragma once

#include "ImmigratorUserEntity.h"
#include "ImmigratorSDKErrorCodes.h"


class IPIObject
{
public:
	IPIObject(void):m_errorCode(Immigrator_HTTP_NONE){};
	virtual ~IPIObject(void){};

	virtual  void InitConfiguration(const char* hostAddress
		, const unsigned int port
		, const char* requestPage
		, const int nTimeoutinMs
		, const char* userAgent
		, const char* serviceCode
		, const int nLoggingLevel) = 0 ;

	virtual ImmigratorErrors RequestRegistration(
		const char* userId
		, const char* userPwd
		, const char* remoteIp
		, unsigned int* pnUserNo) = 0 ;

	virtual ImmigratorErrors RequestLogin(
		const unsigned int userNo
		, const char* userPwd
		, const char* remoteIp
		, ImmigratorUserEntity* pUserEntity) = 0 ;

	virtual ImmigratorErrors RequestLogoff(const unsigned int userNo
		, const char* remoteIp) = 0 ;

	// must be less than 2000 characters
	virtual ImmigratorErrors RequestBanAccount(const unsigned int userNo
		, const unsigned int reasonCode
		, const char* remoteIp
		, const char* pStartTime
		, const char* pEndTime
		, const char* pszComment) = 0;

	virtual ImmigratorErrors RequestUnbanAccount(const unsigned int userNo
		, const unsigned int reasonCode
		, const char* opServerIP
		, const char* effectiveStartDate
		, const char* effectiveEndDate
		, const char* opComment) = 0 ;

 	virtual ImmigratorErrors RequestCancelRegister( const unsigned int userNo 
																, const char* userPwd
																, const unsigned int reasonCode 
																,const char* remoteIp) = 0 ;
	 virtual const char* GetErrorMessage() = 0;
	 ImmigratorErrors GetErrorCode() const { return m_errorCode ; }

	 virtual const char* GetEventInformation() = 0 ;

	 virtual ImmigratorErrors ValidateUserAccount( const char* userId, const char* userPwd, const char* remoteIp ) = 0 ;

protected:
	 ImmigratorErrors m_errorCode ;
	 virtual void SetErrorCode( const ImmigratorErrors errors_ ) = 0;
};

class IWLAuthorizeObject
{
public:
	IWLAuthorizeObject(void):m_errorCode(Immigrator_HTTP_NONE){};
	virtual ~IWLAuthorizeObject(void){};

	virtual  void InitConfiguration(const char* hostAddress
		, const unsigned int port
		, const char* requestPage
		, const int nTimeoutinMs
		, const char* userAgent
		, const char* serviceCode
		, const int nLoggingLevel) = 0 ;

	virtual ImmigratorErrors RequestWTRegistration(
		const char* userId
		, const char* remoteIp
		, unsigned int* pnUserNo) = 0 ;

	virtual ImmigratorErrors RequestWTLogin(
		const unsigned int userNo
		, const char* remoteIp
		, ImmigratorUserEntity* pUserEntity) = 0 ;

	virtual ImmigratorErrors RequestWTLogoff(const unsigned int userNo
		, const char* remoteIp) = 0 ;

 	virtual ImmigratorErrors RequestWTCancelRegister( const unsigned int userNo 
																, const unsigned int reasonCode 
																,const char* remoteIp) = 0 ;
	 virtual const char* GetErrorMessage() = 0;
	 ImmigratorErrors GetErrorCode() const { return m_errorCode ; }

	 virtual const char* GetEventInformation() = 0 ;

	 virtual ImmigratorErrors ValidateUserAccount( const char* userId, const char* userPwd, const char* remoteIp ) = 0 ;

protected:
	 ImmigratorErrors m_errorCode ;
	 virtual void SetErrorCode( const ImmigratorErrors errors_ ) = 0;
};

