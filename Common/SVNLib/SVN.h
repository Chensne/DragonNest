#pragma once

#include "Singleton.h"
#include "include\\svn_client.h"
#include "include\\svn_pools.h"

struct log_msg_baton2
{
	const char *message;  /* the message. */
	const char *message_encoding; /* the locale/encoding of the message. */
	const char *base_dir; /* the base directory for an external edit. UTF-8! */
	const char *tmpfile_left; /* the tmpfile left by an external edit. UTF-8! */
	apr_pool_t *pool; /* a pool. */
};

class CSVN : public CSingleton< CSVN >
{
public:
	CSVN(void);
	virtual ~CSVN(void);

protected:
	bool m_bPoolInit;
	svn_client_ctx_t *m_pCTX;
	apr_pool_t *m_pBasePool;
	svn_auth_baton_t *m_pAuthBaton;

public:
	void Initialize( const char *pUserName, const char *pPassword );
	void AuthSVN( apr_pool_t *pool, svn_client_ctx_t* ctx );
	void *logMessage( const char *message, char *baseDirectory = NULL );

	BOOL Checkout( const char *moduleName, const char *destPath, BOOL recurse, BOOL bIgnoreExternals );
	BOOL Lock( const char *pLockItem, BOOL bStealLock, const char *pComment );
	BOOL Unlock( const char *pUnlockItem, BOOL bBreakLock );
	int Commit( const char *pCommitItem, const char *pMessage, BOOL recurse, BOOL keep_locks );
	BOOL MakeDir( const char *pDir, const char *pComment );
};
