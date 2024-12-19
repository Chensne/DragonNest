#pragma once

#include <list>
#include <queue>
#include <stack>
#include <vector>
#include <map>
#include <set>
#include <hash_map>
#include "NxSyncObject.h"

// 걍 일케 쓰자. -_-a;

template< typename _T >
class NxList : public std::list<_T>, public NxSyncObject { };

template< typename _T >
class NxVector : public std::vector<_T>, public NxSyncObject { };

template< typename _T >
class NxQueue : public std::queue<_T>, public NxSyncObject { };

template< typename _T >
class NxStack : public std::stack<_T>, public NxSyncObject { };

template< typename _T>
class NxSet : public std::set<_T>, public NxSyncObject { };

template< typename _T1, typename _T2 >
class NxMap : public std::map<_T1, _T2>, public NxSyncObject { };

template< typename _T1, typename _T2 >
class NxHashMap : public stdext::hash_map<_T1, _T2>, public NxSyncObject { };


