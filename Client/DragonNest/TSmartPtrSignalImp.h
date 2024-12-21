#pragma once

template < class T, typename T2 >
class TSmartPtrSignalImp
{
public:
	TSmartPtrSignalImp() {

	}

	virtual ~TSmartPtrSignalImp() {
		/*
		for( DWORD i=0; i<m_VecList.size(); i++ ) {
			SAFE_RELEASE_SPTR( m_VecList[i].hHandle );
		}
		*/
		SAFE_DELETE_VEC( m_VecList );
	}

	struct SmartPtrSignalImpStruct : public T2 {
		T hHandle;
		int nActionIndex;
		int nSignalIndex;
	};

	bool IsExistSignalHandle( int nActionIndex, int nSignalIndex ) {
		for( DWORD i=0; i<m_VecList.size(); i++ ) {
			if( m_VecList[i].nActionIndex == nActionIndex && m_VecList[i].nSignalIndex == nSignalIndex ) return true;
		}
		return false;
	}

	T GetSignalHandle( int nActionIndex, int nSignalIndex, bool bReverse = false ) {
		if( bReverse ) {
			for( int i=(int)m_VecList.size()-1; i>=0; i-- ) {
				if( m_VecList[i].nActionIndex == nActionIndex && m_VecList[i].nSignalIndex == nSignalIndex ) return m_VecList[i].hHandle;
			}
		}
		else {
			for( DWORD i=0; i<m_VecList.size(); i++ ) {
				if( m_VecList[i].nActionIndex == nActionIndex && m_VecList[i].nSignalIndex == nSignalIndex ) return m_VecList[i].hHandle;
			}
		}

		T hIdentity;
		return hIdentity;
	}

	T2 *InsertSignalHandle( int nActionIndex, int nSignalIndex, T hHandle ) {
		SmartPtrSignalImpStruct Struct;
		Struct.nActionIndex = nActionIndex;
		Struct.nSignalIndex = nSignalIndex;
		Struct.hHandle = hHandle;

		m_VecList.push_back( Struct );
		return &m_VecList[m_VecList.size() - 1];
	}
	void RemoveSignalHandle( int nActionIndex, int nSignalIndex, bool bDelete = true ) {
		for( DWORD i=0; i<m_VecList.size(); i++ ) {
			if( m_VecList[i].nActionIndex == nActionIndex && m_VecList[i].nSignalIndex == nSignalIndex ) {
				if( bDelete ) SAFE_RELEASE_SPTR( m_VecList[i].hHandle );
				m_VecList.erase( m_VecList.begin() + i );
				return;
			}
		}
	}
	DWORD GetCount() { return (DWORD)m_VecList.size(); }
	T GetObject( DWORD dwIndex ) { return m_VecList[dwIndex].hHandle; }
	T2 *GetStruct( DWORD dwIndex ) { return &m_VecList[dwIndex]; }
	void RemoveObject( DWORD dwIndex, bool bDelete = true ) {
		if( dwIndex < 0 || dwIndex >= m_VecList.size() ) return;
		if( bDelete ) SAFE_RELEASE_SPTR( m_VecList[dwIndex].hHandle );
		m_VecList.erase( m_VecList.begin() + dwIndex );
	}

	DWORD GetCountFromActionIndex( int nActionIndex ) {
		DWORD dwCount = 0;
		for( DWORD i=0; i<m_VecList.size(); i++ ) {
			if( m_VecList[i].nActionIndex == nActionIndex ) dwCount++;
		}
		return dwCount;
	};
	void RemoveAllSignalHandle( bool bDelete = true ) {
		if( bDelete ) {
			for( DWORD i=0; i<m_VecList.size(); i++ ) {
				SAFE_RELEASE_SPTR( m_VecList[i].hHandle );
			}
		}
		SAFE_DELETE_VEC( m_VecList );
	}
	T2 *GetStruct( int nActionIndex, int nSignalIndex ) {
		for( DWORD i=0; i<m_VecList.size(); i++ ) {
			if( m_VecList[i].nActionIndex == nActionIndex && m_VecList[i].nSignalIndex == nSignalIndex ) {
				return &m_VecList[i];
			}
		}
		return NULL;
	}

protected:

	std::vector<SmartPtrSignalImpStruct> m_VecList;
};

namespace TSmartPtrSignalStruct 
{
	struct Default {
	};
};