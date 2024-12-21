#include "StdAfx.h"
#include "SVN.h"
#include "svn_config.h"

CSVN::CSVN(void)
{
	m_pCTX = NULL;
	m_bPoolInit = false;
	m_pBasePool = NULL;
	m_pAuthBaton = NULL;
}

CSVN::~CSVN(void)
{
	if( m_pBasePool )
	{
		svn_pool_destroy( m_pBasePool );
	}
	if( m_bPoolInit )
	{
		apr_pool_terminate();
	}
}

void CSVN::Initialize( const char *pUserName, const char *pPassword )
{
	apr_pool_initialize();
	m_bPoolInit = true;
	m_pBasePool = svn_pool_create( NULL );
	svn_client_create_context( &m_pCTX, m_pBasePool );
	AuthSVN( m_pBasePool, m_pCTX );

	svn_auth_set_parameter( m_pAuthBaton, SVN_AUTH_PARAM_DEFAULT_USERNAME, pUserName ); 
	svn_auth_set_parameter( m_pAuthBaton, SVN_AUTH_PARAM_DEFAULT_PASSWORD, pPassword ); 
}

void CSVN::AuthSVN( apr_pool_t *pool, svn_client_ctx_t* ctx )
{
	// set up authentication

	svn_auth_provider_object_t *provider;

	/* The whole list of registered providers */
	apr_array_header_t *providers = apr_array_make (pool, 11, sizeof (svn_auth_provider_object_t *));

	/* The main disk-caching auth providers, for both
	'username/password' creds and 'username' creds.  */
	svn_auth_get_windows_simple_provider (&provider, pool);
	APR_ARRAY_PUSH (providers, svn_auth_provider_object_t *) = provider;
	svn_auth_get_simple_provider (&provider, pool);
	APR_ARRAY_PUSH (providers, svn_auth_provider_object_t *) = provider;
	svn_auth_get_username_provider (&provider, pool);
	APR_ARRAY_PUSH (providers, svn_auth_provider_object_t *) = provider;

	/* The server-cert, client-cert, and client-cert-password providers. */
	svn_auth_get_ssl_server_trust_file_provider (&provider, pool);
	APR_ARRAY_PUSH (providers, svn_auth_provider_object_t *) = provider;
	svn_auth_get_ssl_client_cert_file_provider (&provider, pool);
	APR_ARRAY_PUSH (providers, svn_auth_provider_object_t *) = provider;
	svn_auth_get_ssl_client_cert_pw_file_provider (&provider, pool);
	APR_ARRAY_PUSH (providers, svn_auth_provider_object_t *) = provider;

	/* Build an authentication baton to give to libsvn_client. */
	svn_auth_open ( &m_pAuthBaton, providers, pool );
	ctx->auth_baton = m_pAuthBaton;
}

void *CSVN::logMessage ( const char *message, char *baseDirectory )
{
	log_msg_baton2 *baton = (log_msg_baton2 *) apr_palloc (m_pBasePool, sizeof (*baton));
	baton->message = apr_pstrdup(m_pBasePool, message);
	baton->base_dir = baseDirectory ? baseDirectory : "";

	baton->message_encoding = NULL;
	baton->tmpfile_left = NULL;
	baton->pool = m_pBasePool;

	return baton;
}

BOOL CSVN::Checkout( const char *moduleName, const char *destPath, BOOL recurse, BOOL bIgnoreExternals )
{
	apr_pool_t *pPool = svn_pool_create( m_pBasePool );
	svn_opt_revision_t Revision;
	Revision.kind = svn_opt_revision_head;
	svn_error_t *Err = svn_client_checkout2 ( NULL, moduleName, destPath, &Revision, &Revision, recurse, bIgnoreExternals, m_pCTX, pPool );
	svn_pool_destroy( pPool );
	if(Err != NULL)
	{
		return FALSE;
	}

	return TRUE;	
}

BOOL CSVN::Lock( const char *pLockItem, BOOL bStealLock, const char *pComment )
{
	apr_array_header_t *targets = apr_array_make( m_pBasePool, 1, sizeof( const char * ) );
	const char * target = apr_pstrdup ( m_pBasePool, pLockItem );
	( *( ( const char ** )apr_array_push(targets ) ) ) = target;
	svn_error_t *Err = svn_client_lock( targets, pComment, bStealLock, m_pCTX, m_pBasePool );
	return ( Err == NULL );
}

BOOL CSVN::Unlock( const char *pUnlockItem, BOOL bBreakLock )
{
	apr_array_header_t *targets = apr_array_make( m_pBasePool, 1, sizeof( const char * ) );
	const char * target = apr_pstrdup ( m_pBasePool, pUnlockItem );
	( *( ( const char ** )apr_array_push(targets ) ) ) = target;
	svn_error_t *Err = svn_client_unlock( targets, bBreakLock, m_pCTX, m_pBasePool );
	return (Err == NULL);
}

int CSVN::Commit( const char *pCommitItem, const char *pMessage, BOOL recurse, BOOL keep_locks )
{
	svn_commit_info_t *commit_info = svn_create_commit_info( m_pBasePool );

	m_pCTX->log_msg_baton2 = logMessage( pMessage );
	apr_array_header_t *targets = apr_array_make( m_pBasePool, 1, sizeof( const char * ) );
	const char * target = apr_pstrdup ( m_pBasePool, pCommitItem );
	( *( ( const char ** )apr_array_push(targets ) ) ) = target;
	svn_error_t *Err = svn_client_commit3( &commit_info, targets, recurse, keep_locks, m_pCTX, m_pBasePool );
	m_pCTX->log_msg_baton2 = logMessage("");
	if(Err != NULL)
	{
		return 0;
	}
	if( commit_info && SVN_IS_VALID_REVNUM( commit_info->revision ) )
	{
		return commit_info->revision;
	}

	return 0;
}

BOOL CSVN::MakeDir( const char *pDir, const char *pComment )
{
	svn_commit_info_t *commit_info = svn_create_commit_info( m_pBasePool );
	m_pCTX->log_msg_baton2 = logMessage( pComment );
	apr_array_header_t *targets = apr_array_make( m_pBasePool, 1, sizeof( const char * ) );
	const char * target = apr_pstrdup ( m_pBasePool, pDir );
	( *( ( const char ** )apr_array_push( targets ) ) ) = target;
	svn_error_t *Err = svn_client_mkdir2 ( &commit_info, targets, m_pCTX, m_pBasePool );
	if(Err != NULL)
	{
		return FALSE;
	}
	return TRUE;
}
