#ifndef __NMDATATYPES_H_C9C3D6C1_CC9F_4C22_8388_24CB34669356__
#define __NMDATATYPES_H_C9C3D6C1_CC9F_4C22_8388_24CB34669356__

//
//	Windows header files
//
#include <Windows.h>

#ifndef STRSAFE_NO_DEPRECATE
#define STRSAFE_NO_DEPRECATE
#endif
#include <strsafe.h>

#include <atlstr.h>

//
//	STL header files
//
#include <functional>

///////////////////////////////////////////////////////////////////////////////
//
//	Primary data types
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _NMIDCODE_DEFINED
#define _NMIDCODE_DEFINED
	typedef	UINT64	NMIDCode;
#endif

#ifndef _NMGAMECODE_DEFINED
#define _NMGAMECODE_DEFINED
	typedef UINT32	NMGameCode;
#endif

#ifndef _NMGUILDCODE_DEFINED
#define _NMGUILDCODE_DEFINED
	typedef UINT32	NMGuildCode;
#endif

#ifndef _NMGUILDKEY_DEFINED
#define _NMGUILDKEY_DEFINED
	typedef UINT64	NMGuildKey;
#endif

#ifndef _NMGAMESERVERCODE_DEFINED
#define _NMGAMESERVERCODE_DEFINED
	typedef UINT32	NMGameServerCode;
#endif

///////////////////////////////////////////////////////////////////////////////
//
//	Main classes
//
///////////////////////////////////////////////////////////////////////////////

namespace Nexon
{
	namespace Platform
	{
		///////////////////////////////////////////////////////////////////////
		//
		//	Primary messenger data types
		//
		///////////////////////////////////////////////////////////////////////

		inline NMIDCode MakeNMIDCode( INT32 nNexonSN, INT32 nGameCode = 0x00010000 )
		{
			return ( ( ( static_cast< NMIDCode >( nNexonSN ) ) << 32 ) | nGameCode );
		}

		inline INT32 GetNexonSN( NMIDCode uIDCode )
		{
			return ( static_cast< INT32 >( uIDCode >> 32 ) );
		}

		inline INT32 GetServerCode( NMGuildKey const & guildKey )
		{
			return static_cast< INT32 >( guildKey >> 32 );
		}

		inline INT32 GetGuildSN( NMGuildKey const & guildKey )
		{
			return static_cast< INT32 >( guildKey );
		}

		inline NMGuildKey MakeNMGuildKey( INT32 nServerCode, INT32 nGuildSN )
		{
			return ( ( ( static_cast< NMGuildKey >( nServerCode ) ) << 32 ) | nGuildSN );
		}

		template< class T >
		struct IsSameNexonSN : public std::unary_function< T, bool >
		{
			IsSameNexonSN( INT32 nNexonSN ) : m_nNexonSN( nNexonSN )
			{
			}
			IsSameNexonSN( NMIDCode uIDCode ) : m_nNexonSN( GetNexonSN( uIDCode ) )
			{
			}
			result_type operator () ( argument_type const & value ) const
			{
				return ( value.GetNexonSN() == m_nNexonSN );
			}
		private:
			INT32 m_nNexonSN;
		};

		///////////////////////////////////////////////////////////////////////
		//
		//	NMGuildID structure
		//
		///////////////////////////////////////////////////////////////////////

		struct NMGuildID
		{
			INT32	nGameCode;
			INT32	nServerCode;
			INT32	nGuildSN;

			NMGuildID()
				: nGameCode( 0 )
				, nServerCode( 0 )
				, nGuildSN( 0 ) 
			{
			}
			NMGuildID( INT32 nGameCode, NMGuildKey guildKey )
				: nGameCode( nGameCode )
				, nServerCode( GetServerCode( guildKey ) )
				, nGuildSN( GetGuildSN( guildKey ) )
			{
			}
			NMGuildID( INT32 nGameCode, INT32 nServerCode, INT32 nGuildSN )
				: nGameCode( nGameCode )
				, nServerCode( nServerCode )
				, nGuildSN( nGuildSN )
			{
			}
			NMGuildKey GetGuildKey() const
			{
				return MakeNMGuildKey( this->nServerCode, this->nGuildSN );
			}
			bool operator == ( NMGuildID const & rhs ) const
			{
				return ( this->nGameCode == rhs.nGameCode && this->nServerCode == rhs.nServerCode && this->nGuildSN == rhs.nGuildSN );
			}
		};

		struct NMGuildIDLess : public std::binary_function< NMGuildID, NMGuildID, bool >
		{
			bool operator () ( NMGuildID const & lhs, NMGuildID const & rhs ) const
			{
				return ( lhs.nGameCode < rhs.nGameCode || lhs.nServerCode < rhs.nServerCode || lhs.nGuildSN < rhs.nGuildSN );
			}
		};

		template< class T >
		struct IsSameGuildID : public std::unary_function< T, bool >
		{
			IsSameGuildID( NMGuildID const & guildID ) : m_guildID( guildID )
			{
			}
			result_type operator () ( argument_type const & data ) const
			{
				return ( data.GetGuildID() == this->m_guildID );
			}
		private:
			NMGuildID m_guildID;
		};

		///////////////////////////////////////////////////////////////////////
		//
		//	StringZip template class
		//
		///////////////////////////////////////////////////////////////////////

		template< size_t N >
		class StringZip
		{
		public:
			typedef TCHAR		value_type;
			typedef LPTSTR		pointer;
			typedef LPCTSTR		const_pointer;
			typedef size_t		size_type;

		public:
			StringZip()
				: m_pData( m_szEmpty )
			{
			}

			StringZip( StringZip const & rhs )
				: m_pData( m_szEmpty )
			{
				this->CopyString( rhs.m_pData );
			}

			StringZip( const_pointer pszText )
				: m_pData( m_szEmpty )
			{
				this->CopyString( pszText );
			}

			template< size_type N2 >
			StringZip( StringZip< N2 > const & rhs )
				: m_pData( m_szEmpty )
			{
				this->CopyString( rhs );
			}

			StringZip & operator = ( StringZip const & rhs )
			{
				this->CopyString( rhs );
				return *this;
			}

			StringZip & operator = ( const_pointer pszText )
			{
				this->CopyString( pszText );
				return *this;
			}

			template< size_type N2 >
			StringZip & operator = ( StringZip< N2 > const & rhs )
			{
				this->CopyString( rhs );
				return *this;
			}

			~StringZip()
			{
				this->DeleteString();
			}

			bool IsEmpty() const
			{
				return ( this->m_pData == NULL || this->m_pData[ 0 ] == 0 );
			}

			size_type GetSize() const
			{
				if ( this->m_pData )
				{
					size_t cchSize = 0;
					::StringCchLength( this->m_pData, N, &cchSize );
					return ( static_cast< size_type >( cchSize ) );
				}
				return 0;
			}

			UINT32 GetCapacity() const
			{
				return static_cast< UINT32 >( N );
			}

			pointer GetBuffer()
			{
				this->AllocateString();
				return ( this->m_pData );
			}

			operator const_pointer () const
			{
				return ( this->m_pData );
			}

			operator pointer ()
			{
				return ( this->m_pData );
			}

			template< size_type N2 >
			bool operator == ( StringZip< N2 > const & rhs ) const
			{
				return ( this->CompareString( this->m_pData, rhs ) == CSTR_EQUAL );
			}

			bool operator == ( const_pointer pszText ) const
			{
				return ( this->CompareString( this->m_pData, pszText ) == CSTR_EQUAL );
			}

			bool operator == ( pointer pszText ) const
			{
				return ( this->CompareString( this->m_pData, pszText ) == CSTR_EQUAL );
			}

			template< size_type N2 >
			bool operator != ( StringZip< N2 > const & rhs ) const
			{
				return ( this->CompareString( this->m_pData, rhs.m_pData ) != CSTR_EQUAL );
			}

			bool operator != ( const_pointer pszText ) const
			{
				return ( this->CompareString( this->m_pData, pszText ) != CSTR_EQUAL );
			}

			bool operator != ( pointer pszText ) const
			{
				return ( this->CompareString( this->m_pData, pszText ) != CSTR_EQUAL );
			}

			template< size_type N2 >
			bool operator < ( StringZip< N2 > const & rhs ) const
			{
				return ( this->CompareString( this->m_pData, rhs.m_pData ) == CSTR_LESS_THAN );
			}

			bool operator < ( const_pointer pszText ) const
			{
				return ( this->CompareString( this->m_pData, pszText ) == CSTR_LESS_THAN );
			}

			bool operator < ( pointer pszText ) const
			{
				return ( this->CompareString( this->m_pData, pszText ) == CSTR_LESS_THAN );
			}

			template< size_type N2 >
			bool operator > ( StringZip< N2 > const & rhs ) const
			{
				return ( this->CompareString( this->m_pData, rhs.m_pData ) == CSTR_GREATER_THAN );
			}

			bool operator > ( const_pointer pszText ) const
			{
				return ( this->CompareString( this->m_pData, pszText ) == CSTR_GREATER_THAN );
			}

			bool operator > ( pointer pszText ) const
			{
				return ( this->CompareString( this->m_pData, pszText ) == CSTR_GREATER_THAN );
			}

			template< size_type N2 >
			bool operator <= ( StringZip< N2 > const & rhs ) const
			{
				return !( *this > rhs );
			}

			bool operator <= ( const_pointer pszText ) const
			{
				return !( *this > pszText );
			}

			bool operator <= ( pointer pszText ) const
			{
				return !( *this > pszText );
			}

			template< size_type N2 >
			bool operator >= ( StringZip< N2 > const & rhs ) const
			{
				return !( *this < rhs );
			}

			bool operator >= ( const_pointer pszText ) const
			{
				return !( *this < pszText );
			}

			bool operator >= ( pointer pszText ) const
			{
				return !( *this < pszText );
			}

		protected:

			bool AllocateString()
			{
				if ( this->m_pData == NULL || this->m_pData == this->m_szEmpty )
				{
					try
					{
						this->m_pData = new value_type[ N ];
						this->m_pData[ 0 ] = 0;
					}
					catch (...)
					{
						this->m_pData = this->m_szEmpty;
					}
				}
				return ( this->m_pData != NULL && this->m_pData != this->m_szEmpty );
			}

			void DeleteString()
			{
				if ( this->m_pData && this->m_pData != this->m_szEmpty )
				{
					delete this->m_pData;
					this->m_pData = this->m_szEmpty;
				}
			}

			void CopyString( const_pointer pszText )
			{
				if ( pszText )
				{
					if ( this->AllocateString() )
					{
						::StringCchCopy( this->m_pData, N, pszText );
					}
				}
				else
				{
					this->DeleteString();
				}
			}

			int CompareString( LPCTSTR pszLeft, LPCTSTR pszRight ) const
			{
				if ( pszLeft != NULL && pszRight != NULL )
				{
					return ::CompareString( LOCALE_USER_DEFAULT, 0, pszLeft, -1, pszRight, -1 );
				}
				else if ( pszLeft == NULL && pszRight != NULL && pszRight[ 0 ] != 0 )
				{
					return CSTR_LESS_THAN;
				}
				else if ( pszLeft != NULL && pszLeft[ 0 ] != 0 && pszRight == NULL )
				{
					return CSTR_GREATER_THAN;
				}
				return CSTR_EQUAL;
			}

		private:
			pointer	m_pData;

		private:
			static value_type m_szEmpty[];
		};

		template< size_t N >
		typename StringZip< N >::value_type StringZip< N >::m_szEmpty[] = _T("");

		template< size_t N > bool operator == ( LPCTSTR pszText, StringZip< N > const & rhs )
		{
			return ( rhs == pszText );
		}

		template< size_t N > bool operator == ( LPTSTR pszText, StringZip< N > const & rhs )
		{
			return ( rhs == pszText );
		}

		template< size_t N > bool operator != ( LPCTSTR pszText, StringZip< N > const & rhs )
		{
			return ( rhs != pszText );
		}

		template< size_t N > bool operator != ( LPTSTR pszText, StringZip< N > const & rhs )
		{
			return ( rhs != pszText );
		}

		template< size_t N > bool operator < ( LPCTSTR pszText, StringZip< N > const & rhs )
		{
			return ( rhs > pszText );
		}

		template< size_t N > bool operator < ( LPTSTR pszText, StringZip< N > const & rhs )
		{
			return ( rhs > pszText );
		}

		template< size_t N > bool operator > ( LPCTSTR pszText, StringZip< N > const & rhs )
		{
			return ( rhs < pszText );
		}

		template< size_t N > bool operator > ( LPTSTR pszText, StringZip< N > const & rhs )
		{
			return ( rhs < pszText );
		}

		template< size_t N > bool operator <= ( LPCTSTR pszText, StringZip< N > const & rhs )
		{
			return ( rhs >= pszText );
		}

		template< size_t N > bool operator <= ( LPTSTR pszText, StringZip< N > const & rhs )
		{
			return ( rhs >= pszText );
		}

		template< size_t N > bool operator >= ( LPCTSTR pszText, StringZip< N > const & rhs )
		{
			return ( rhs <= pszText );
		}

		template< size_t N > bool operator >= ( LPTSTR pszText, StringZip< N > const & rhs )
		{
			return ( rhs <= pszText );
		}
	};
};

#endif	//	#ifndef __NMDATATYPES_H_C9C3D6C1_CC9F_4C22_8388_24CB34669356__