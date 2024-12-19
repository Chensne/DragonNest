#pragma once

template < class T >
class TObjectSignalImp
{
public:
	TObjectSignalImp() {

	}

	virtual ~TObjectSignalImp() {
		for( DWORD i=0; i<m_VecList.size(); i++ ) {
			SAFE_DELETE( m_VecList[i].pObject );
		}
		SAFE_DELETE_VEC( m_VecList );
	}

	bool IsExistSignalHandle( int nActionIndex, int nSignalIndex ) {
		for( DWORD i=0; i<m_VecList.size(); i++ ) {
			if( m_VecList[i].nActionIndex == nActionIndex && m_VecList[i].nSignalIndex == nSignalIndex ) return true;
		}
		return false;
	}

	T *GetSignalHandle( int nActionIndex, int nSignalIndex ) {
		for( DWORD i=0; i<m_VecList.size(); i++ ) {
			if( m_VecList[i].nActionIndex == nActionIndex && m_VecList[i].nSignalIndex == nSignalIndex ) return m_VecList[i].hHandle;
		}

		return NULL;
	}

	void InsertSignalHandle( int nActionIndex, int nSignalIndex, T *pObject ) {
		ObjectSignalImpStruct Struct;
		Struct.nActionIndex = nActionIndex;
		Struct.nSignalIndex = nSignalIndex;
		Struct.pObject = pObject;

		m_VecList.push_back( Struct );
	}
	void RemoveSignalHandle( int nActionIndex, int nSignalIndex, bool bDelete = true ) {
		for( DWORD i=0; i<m_VecList.size(); i++ ) {
			if( m_VecList[i].nActionIndex == nActionIndex && m_VecList[i].nSignalIndex == nSignalIndex ) {
				if( bDelete ) SAFE_DELETE( m_VecList[i].pObject );
				m_VecList.erase( m_VecList.begin() + i );
				return;
			}
		}
	}

	DWORD GetCount() { return (DWORD)m_VecList.size(); }
	T *GetObject( DWORD dwIndex ) { return m_VecList[dwIndex].pObject; }

protected:
	struct ObjectSignalImpStruct {
		T *pObject;
		int nActionIndex;
		int nSignalIndex;
	};

	std::vector<ObjectSignalImpStruct> m_VecList;
};