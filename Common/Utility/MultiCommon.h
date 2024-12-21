#pragma once

#define MAX_SESSION_COUNT 1200

#define STATIC_DECL( type ) \
	static type[MAX_SESSION_COUNT]

#define STATIC_INSTANCE( type ) \
	type[ GetRoom()->GetRoomID() ]
	//type[ GetRoom()->GetSessionID() ]

#define STATIC_INSTANCE_( type ) \
	type[ pRoom->GetRoomID() ]
	//type[ pRoom->GetSessionID() ]

#define STATIC_DECL_INIT( cls, type, val ) \
	type cls::##val[MAX_SESSION_COUNT]
