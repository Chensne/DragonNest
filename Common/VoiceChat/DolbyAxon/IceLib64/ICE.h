#ifndef _ICE_H_
#define _ICE_H_

#ifdef __cplusplus
extern "C" {
#endif

#define ICE_VERSION "DolbyAxon1.2.1S"

#define REQUEST_GAME_ID 0xFFFF

#define ICE_SERVER_API_VERSION 0x0012

typedef struct _ICE_IP
{
	unsigned char a;
	unsigned char b;
	unsigned char c;
	unsigned char d;
} ICE_IP;                         // Holds an IP a.b.c.d

typedef struct _ICE ICE;

typedef struct _ICE_player_status
{
	unsigned char talking;        // 0 if not talking, indicative volume 1-255 if player is talking
	unsigned int lastcontact;     // Number of seconds since the player last contacted the server
	unsigned char connected;      // 1 if the player is currently in contact with the server
	unsigned char premium;        // 1 if the client is premium
	unsigned int voice_morph;     // Voice morphing ID used by the client (if any), 0 otherwise
} ICE_player_status;

typedef enum _ICE_ERROR
{
	//Common Error Code
	ICE_ERROR_NONE,
	ICE_ERROR_ICE_NULL,
	ICE_ERROR_NOT_INITED,
	ICE_ERROR_ALREADYINITED,
	ICE_ERROR_PARAM_NULL,
	ICE_ERROR_SOCKET_CREATE,
	ICE_ERROR_SOCKET_BIND,
	ICE_ERROR_AVATAR_NOT_FOUND,
	ICE_ERROR_CHANNEL_NOT_FOUND,
	ICE_ERROR_CHANNEL_EXISTS,
	ICE_ERROR_AVATAR_NOT_FOUND_IN_CHAN,
	ICE_ERROR_AVATAR_EXISTS,
	ICE_ERROR_TOOMUCH_DATA,
	ICE_ERROR_INVALID_MATRIX,
	ICE_ERROR_NONSPATIAL_CHANNEL,
	ICE_ERROR_SPATIAL_CHANNEL,
	ICE_ERROR_MODEL_INSTANCE_NOT_FOUND, 
	ICE_ERROR_NO_INSTANCE_ID_AVAILABLE,
	ICE_ERROR_MODEL_NOT_FOUND, 
	ICE_ERROR_MODEL_USED,
	ICE_ERROR_MODEL_EXISTS,
	ICE_ERROR_BATCHING,
	ICE_ERROR_ALREADY_BATCHING,
	ICE_ERROR_NOT_BATCHING,
	ICE_ERROR_READONLY,
	ICE_ERROR_INVALIDSIZE,
	ICE_ERROR_INVALIDPROPERTY,
	ICE_ERROR_INVALIDPROPERTYVALUE,
	ICE_ERROR_INVALID_PROFILE,
	ICE_ERROR_OUTOFBOUND_ROTATION,
	ICE_ERROR_INVALID_IP,
	ICE_ERROR_UNRECOGNISED_DATA,
	ICE_ERROR_INCOMPATIBLE_API_VERSION,
	ICE_ERROR_CALLBACK_NULL,
	ICE_ERROR_CONFIG_GSNAME_INVALID,
	ICE_ERROR_CONFIG_SERVER_INVALID,

	//SINGLE only error code
	ICE_ERROR_ID_OUTOFBOUND_MLIB,
	ICE_ERROR_MLIB_ENABLED,
	ICE_ERROR_GAMEID_NOTREADY,
	ICE_ERROR_GAMEID_INVALID,
	ICE_ERROR_ENCAPSULATION_NOT_SET,
	ICE_ERROR_SEND_CAPSULE_FAILED,
	ICE_ERROR_NEGATIVE_EMPHASIS,
	ICE_ERROR_LOAD_CONTROL_LIB_FAILED,
	ICE_ERROR_LOAD_AUDIO_LIB_FAILED,
	ICE_ERROR_CONFIG_MODES_INCOMPATIBLE,

	//MULTI only error code
	ICE_ERROR_BAD_POSITION,
	ICE_ERROR_NO_SPACE,

	//Unknown
	ICE_ERROR_UNKNOWN

} ICE_ERROR;


typedef enum _ICE_CHAN_PROPERTY
{
	ICE_CHAN_HEARING_RANGE,       //hearing range of the channel, read only, spatial only, int
	ICE_CHAN_EFFECT,              //channel effect, non spatial only, 0 for none, 1 for noise, 2 DO NOT USE, 4 for spatialised walkie talkie
	ICE_CHAN_NUM_AVATARS,         //number of players in this channel, read only, int
	ICE_CHAN_SPATIAL,             //0 if nonspatial, 1 if spatial, read only, int
} ICE_CHAN_PROPERTY;

// Axon server side library modes
// Please refer to the provided documentation
const unsigned int ICE_MODE_REMOTESERVERS	= 0;		//Audio and Control servers are external processes (default mode)
const unsigned int ICE_MODE_LOCALSERVERS	= 1 << 0;	//Run Audio and Control servers locally as threads (requires ICE_MODE_ENCAPSULATION)
const unsigned int ICE_MODE_ENCAPSULATION	= 1 << 1;	//Turn on encapsulation mode (data between Axon client and Audio Server is passed via the game)
const unsigned int ICE_MODE_GCS				= 1 << 2;	//Use a global channel server, which allows non-spatial channels to span multiple audio servers (incompatible with ICE_MODE_LOCALSERVERS)
const unsigned int ICE_MODE_LOGGING			= 1 << 3;	//Enable logging
const unsigned int ICE_MODE_MULTIGAME		= 1 << 4;	//Allow multiple libraries to use the same control/audio servers pair (incompatible with ICE_MODE_LOCALSERVERS)

// Basic server structure
typedef struct _ICE_SERVER
{
	const char* address;				// valid IP or DNS name
	unsigned short port;				// valid port
} ICE_SERVER;

// Audio server parameters
typedef struct _ICE_AS_PARAMS
{
	unsigned int cores;					// number of audio processing threads
	unsigned int default_codec;			// default codec
	unsigned int force;					// 1 to force the default codec, 0 otherwise
	unsigned int loglevel;				// loglevel /* To be deprecated */
	const char* logfile;				// logfile /* To be deprecated */
} ICE_AS_PARAMS;

// Control server parameters
typedef struct _ICE_CS_PARAMS
{
	const char* map_prefix;				// map prefix/path
	const char* map_preload;			// maps to be preloaded
	unsigned int loglevel;				// loglevel /* To be deprecated */
	const char* logfile;				// logfile /* To be deprecated */
} ICE_CS_PARAMS;

// Local servers parameters
typedef struct _ICE_LOCALSERVER_PARAMS
{
	ICE_AS_PARAMS audio;				// Audio server parameters
	ICE_CS_PARAMS control;				// Control server parameters
} ICE_LOCALSERVER_PARAMS;

// Encapsulation parameters
typedef struct _ICE_ENCAPSULATION_PARAMS
{
	void (*callback)(const char* msg, unsigned int len, unsigned long long player_id, void* rock);
										// encapsulation callback; if NULL, encapsulation is not used
	void* rock;							// identifier passed to the callback function
} ICE_ENCAPSULATION_PARAMS;

// Logging parameters
typedef struct _ICE_LOGGING_PARAMS
{
	unsigned int loglevel;				// loglevel
	const char* logfile;				// logfile
	void(*callback)(unsigned int msg_level, const char* msg, void* rock);
										// logging callback; ignored if NULL
	void* rock;							// identifier passed to the callback function
} ICE_LOGGING_PARAMS;

typedef struct _ICE_CONFIG
{
	//Library mode
	// This field determines the mode the ICE library will operate in.
	// It should be constructed by OR'ing a select set of
	// ICE_MODE_XXXX values.
	unsigned int mode;

	//Game server name
	// This field must not be NULL and must not be an empty string
	const char* gsname;					

	//Control server location
	// This structure is used if ICE_MODE_REMOTESERVERS is set
	ICE_SERVER cs;

	//Global channel server location
	// This structure is used if ICE_MODE_GCS is set
	ICE_SERVER gcs;

	//Multigame option
	// This parameter is used if ICE_MODE_MULTIGAME is set
	unsigned short game_id;

	//Logging options
	// These values are used if ICE_MODE_LOGGING is set
	ICE_LOGGING_PARAMS logging;

	//Encapsulation options
	// These values are used if ICE_MODE_ENCAPSULATION is set
	ICE_ENCAPSULATION_PARAMS encapsulation;

	//Server threads options
	// These values are only used if ICE_MODE_LOCALSERVERS is set
	ICE_LOCALSERVER_PARAMS localserver;

} ICE_CONFIG;

typedef struct _ICE_SERVER_STATS
{
	//control server
	unsigned int control_server_connected;  // 1 if libICE is properly connected to its control server, 0 otherwise
	unsigned int control_server_uptime;     // seconds the control server has been running
	float control_server_load;              // 1 if normal, >1 if control server is overloaded

	//audio server
	unsigned int audio_server_connected;    // 1 if the audio server is properly connected to the control server, 0 otherwise
	unsigned int audio_server_uptime;       // seconds the audio server has been running
	float audio_server_load;                // 0 if no load, 1 if normal, and >1 if simplification occurs
	unsigned int clients_connected;         //number of players connected to the game server
	unsigned int clients_connected2;        //number of players connected to both game server and audio server
	ICE_IP as_ip;                           //audio server's IP
	unsigned short as_aport;                //audio server's audio port
	unsigned short as_cport;                //audio server's control port

	//Audio server network stats
	unsigned int packets_out;               //number of audio packets sent since startup
	unsigned int packets_out_sec;           //number of audio packets sent per second
	float kbytes_out;                       //audio data sent since start up, in kilobytes
	unsigned int bytes_out_sec;             //audio bytes sent per second
	unsigned int packets_in;                //number of audio packets received since startup
	unsigned int packets_in_sec;            //number of audio packets received per second
	float kbytes_in;                        //audio data received since start up, in kilobytes
	unsigned int bytes_in_sec;              //audio bytes received per second
} ICE_SERVERS_STATS;

// HEARING PROFILE structure 
typedef struct _ICE_HEARING_PROFILE_POINT
{
	float distance;               // 0 <= distance <= 1
	float gain;                   // 0 <= gain <= 1
} ICE_HEARING_PROFILE_POINT;


// Description:
//   Creates the ICE object used for all subsequent API calls that require an
//   ICE pointer. Keep only one ICE object per world and control server.
// Parameters:
//   None.
// Return value:
//   Pointer to ICE object which should be used for future ICE function calls, 
//   NULL if create failed.
ICE* ICE_create();


// Description:
//   Disconnects all players and cleans up any memory allocations.
// Parameters:
//   d: Pointer to the ICE object to be destroyed.
// Return value:
//   No return value.
void ICE_destroy(ICE* d);


// Description:
//   Initializes the ICE object; does not create any channels.
// Parameters:
//   d: Valid ICE pointer returned by ICE_create.
//   config: valid poiter to a config structure
//   interface_version: pass the the value of the macro ICE_SERVER_API_VERSION
// Return value:
//   ICE_ERROR_NONE if successful; non-zero ICE_ERROR if it fails.
ICE_ERROR ICE_init(ICE* d, const ICE_CONFIG* config, unsigned int interface_version);

// Description:
//   Pauses the ICE object's autotick.
// Parameters:
//   d: Valid ICE pointer returned by ICE_create.
//   pause: Non-zero to pause. Zero to start again,
// Return value:
//   ICE_ERROR_NONE if successful; non-zero ICE_ERROR if it fails.
ICE_ERROR ICE_pause(ICE* d, int pause);


// Description:
//   Poll the ICE library to get a game_id provided by the control server
// Parameters:
//   d: Valid ICE pointer initialized by the ICE_init_multigame functions.
//   game_id: a pointer to a 16bit variable to be filled up with the game ID
// Return value:
//   ICE_ERROR_NONE if successful; non-zero ICE_ERROR if it fails.
ICE_ERROR ICE_gameid_poll(ICE* d, unsigned short* game_id);


// Description:
//   Connects a player into libICE. Should be called when a player connects to
//   the server or enters the virtual world. Does not join the player into any
//   channels.
// Parameters:
//   d: Valid ICE pointer initialized by one of the ICE_init functions.
//   name: NULL terminated string specifying the player's name (this name may
//   be used to resolve avatar to client mappings). Max length=254.
//   player_id: The unique ID for the player, it may be reused after the player
//   has been disconnected. Note that player_id must be smaller than 2^32 - 1
//   (0xFFFFFFFF is not valid) in normal mode and smaller than 2^16 - 1 in
//   multi-game mode (see ICE_init_multigame for more infomation about
//   multi-game ID limitations).
//   public_ip: The public IP address of the player. Ignored if using
//   encapsulation.
// Return value:
//   ICE_ERROR_NONE if successful; non-zero ICE_ERROR if it fails.
ICE_ERROR ICE_connect_adv(ICE* d, const char* name, const unsigned long long player_id, const ICE_IP public_ip);

// Description:
//	 Sets whether a given player is a premium client or not. Premium clients 
//	 will be sent surround sound scenes. Non premium clients get a basic 
//	 non-surround sound scene.
// Parameters:
//	 d: Valid ICE pointer initialized by one of the ICE_init functions.
//   player_id: The unique id for the player, ids may be reused by other
//   players after the player has been disconnected. Note that player_id
//   must be smaller than 2^32 - 1 (player_id = 0xFFFFFFFF is not valid).
//	 premium: whether or not the server will promote this user to premium
// NOTE:
//	 If this flag is set, a player will be premium. However if a player has
//	 submitted a premium authentication code to the audio server, they will
//	 be premium with no regard to any calls to this function.
ICE_ERROR ICE_set_premium(ICE* d, const unsigned long long player_id, const int premium);

// Description:
//	 Sets whether all players are premium clients or not. Premium clients 
//	 will be sent surround sound scenes. Non premium clients get a basic 
//	 non-surround sound scene.
// Parameters:
//	 d: Valid ICE pointer initialized by one of the ICE_init functions.
//	 premium: whether or not the server will promote all users to premium
// NOTE:
//	 If this flag is set, all players will be premium. However if a player has
//	 submitted a premium authentication code to the audio server, they will
//	 be premium with no regard to any calls to this function.
ICE_ERROR ICE_set_premium_all(ICE* d, const int premium);

// Description:
//   Disconnects the player from libICE; Should be called when the avatar leaves
//   the virtual world.
// Parameters:
//   d: Valid ICE pointer initialized by one of the ICE_init functions.
//   player_id: The unique id for the player to be disconnected.
// Return value:
//   ICE_ERROR_NONE if successful; non-zero ICE_ERROR if it fails.
ICE_ERROR ICE_disconnect(ICE* d, const unsigned long long player_id);


// Description:
//   Sets the player's name. Can be called again to change the player's name.
// Parameters:
//   d: Valid ICE pointer initialized by one of the ICE_init functions.
//   player_id: The unique id for the player.
//   name: The player's new name. Max length 254.
// Return value:
//   ICE_ERROR_NONE if successful; non-zero ICE_ERROR if it fails.
ICE_ERROR ICE_set_name(ICE* d, const unsigned long long player_id, const char* name);


// Description:
//   Updates the player's position in the virtual world. The coordinate system
//   is such that if +x is to your right then +y is in front. A player with 
//   0-degree rotation would be facing along the positive x axis and a player 
//   with 90-degree rotation would be facing along the positive y axis. This
//   function only affects spatial channels that have not had their player
//   positions updated by ICE_position_channel.
// Parameters:
//   d: Valid ICE pointer initialized by one of the ICE_init functions.
//   player_id: The unique id for the player.
//   x: The x location of a player.
//   y: The y location of a player.
//   z: The z location of a player.
//   yaw: The rotation of a player between 0 and 360 around the z axis of the world
// Return value:
//   ICE_ERROR_NONE if successful; non-zero ICE_ERROR if it fails.
// NOTE:
//   This rotation is only used if ICEClient_SetRotation either isn't set or is
//   set to -1 on the client side using ICE client library. 
ICE_ERROR ICE_position(ICE* d, const unsigned long long player_id, const int x, const int y, const int z, const unsigned short yaw);


// Description:
//   Updates the player's position in the virtual world. The coordinate system
//   is such that if +x is to your right then +y is in front. A player with 
//   0-degree rotation would be facing along the positive x axis and a player 
//   with 90-degree rotation would be facing along the positive y axis. This
//   function only affects spatial channels that have not had their player
//   positions updated by ICE_position_channel.
// Parameters:
//   d: Valid ICE pointer initialized by one of the ICE_init functions.
//   player_id: The unique id for the player.
//   x: The x location of a player.
//   y: The y location of a player.
//   z: The z location of a player.
//   yaw: The rotation of a player between 0 and 360 around the z axis of the world (ie - left/right)
//   pitch: The rotation of a player between -90 and 90 around his own x axis (ie - up/down)
//   roll: The rotation of a player between -180 and +180 around his own viewing axis (ie - rolling left and right)
// Return value:
//   ICE_ERROR_NONE if successful; non-zero ICE_ERROR if it fails.
// NOTE:
//   Do not use client side rotation if you use this function to set non-zero pitch or roll values.
ICE_ERROR ICE_position_npr(ICE* d, const unsigned long long player_id, const int x, const int y, const int z, const unsigned short yaw, const short pitch, const short roll);


// Description:
//   Updates the player's position in the virtual world for a particular
//   spatial channel. The coordinate system is such that if +x is to your
//   right then +y is in front. Zero-degree rotation is facing along the
//   positive x axis, 90-degree rotation is facing along the positive y axis.
//   After ICE_position_channel is used on the given channel, its player
//   positions will not be affected by subsequent calls to ICE_position.
// Parameters:
//   d: Valid ICE pointer initialized by one of the ICE_init functions.
//   player_id: The unique id for the player.
//   channel_id: The id of the channel to change.
//   x: The x coordinate of the avatar.
//   y: The y coordinate of the avatar.
//   z: The z coordinate of the avatar.
//   yaw: The rotation of a player between 0 and 360 around the z axis of the world
// Return value:
//   ICE_ERROR_NONE if successful; non-zero ICE_ERROR if it fails.
ICE_ERROR ICE_position_channel(ICE* d, const unsigned long long player_id, const unsigned int channel_id, const int x, const int y, const int z, const unsigned short yaw);


// Description:
//   Updates the player's position in the virtual world for a particular
//   spatial channel. The coordinate system is such that if +x is to your
//   right then +y is in front. Zero-degree rotation is facing along the
//   positive x axis, 90-degree rotation is facing along the positive y axis.
//   After ICE_position_channel is used on the given channel, its player
//   positions will not be affected by subsequent calls to ICE_position.
// Parameters:
//   d: Valid ICE pointer initialized by one of the ICE_init functions.
//   player_id: The unique id for the player.
//   channel_id: The id of the channel to change.
//   x: The x coordinate of the avatar.
//   y: The y coordinate of the avatar.
//   z: The z coordinate of the avatar.
//   yaw: The rotation of a player between 0 and 360 around the z axis of the world (ie - left/right)
//   pitch: The rotation of a player between -90 and 90 around his own x axis (ie - up/down)
//   roll: The rotation of a player between -180 and +180 around his own viewing axis (ie - rolling left and right)
// Return value:
//   ICE_ERROR_NONE if successful; non-zero ICE_ERROR if it fails.
// NOTE:
//   Do not use client side rotation if you use this function to set non-zero pitch or roll values.
ICE_ERROR ICE_position_channel_npr(ICE* d, const unsigned long long player_id, const unsigned int channel_id, const int x, const int y, const int z, const unsigned short yaw, const short pitch, const short roll);


// Description:
//   Returns information about the player's status in the underlying system.
// Parameters:
//   d: Valid ICE pointer initialized by one of the ICE_init functions.
//   player_id: The unique id for the player.
//   ps: A pointer to ICE_player_status structure that will be used to store
//   status information.    
// Return value:
//   ICE_ERROR_NONE if successful; non-zero ICE_ERROR if it fails.
ICE_ERROR ICE_get_player_status(ICE* d, const unsigned long long player_id, ICE_player_status* ps);


// Description:
//   Mutes and unmutes an individual player for another player.
// Parameters:
//   d: Valid ICE pointer initialized by one of the ICE_init functions.
//   player_id: The unique id of the player who wishes to mute the other player.
//   muted_id: The unique id for the player to be muted.
//   mute: If mute=1 then the player is muted; if mute=0 the player is unmuted.
// Return value:
//   ICE_ERROR_NONE if successful; non-zero ICE_ERROR if it fails.
ICE_ERROR ICE_mute_one_to_one(ICE* d, const unsigned long long player_id, const unsigned long long muted_id, const unsigned char mute);


// Description:
//   Mutes and unmutes a list of players for a single player.
// Parameters:
//   d: Valid ICE pointer initialized by one of the ICE_init functions.
//   player_id: The unique id of the player who wishes to mute the other
//   players.
//   muted_id: The unique ids of the players to be muted, given as an array of
//   length len.
//   mute: An array of len values, each corresponding to one player id given in
//   the muted_id array. If the value is 1, the corresponding player is muted. 
//   If the value is 0, the corresponding player is unmuted. 
//   len: The length of the mutedid and mute arrays
// Return value:
//   ICE_ERROR_NONE if successful; non-zero ICE_ERROR if it fails.
// Note:
//   If one or more player id given in the muted_id array is invalid, the
//   function will fail and no player will be muted/unmuted
ICE_ERROR ICE_mute_one_to_many(ICE* d, const unsigned long long player_id, const unsigned long long* muted_id, const unsigned char* mute, const unsigned int len);


// Description:
//   Mutes and unmutes a single player for a list of players.
// Parameters:
//   d: Valid ICE pointer initialized by one of the ICE_init functions.
//   muter_id: The unique ids of the players that will have player_id muted,
//   given an array of length len.
//   mute: An array of len values, each correspodning to one player id given in
//   the muter_id array. If the value is 1, the player will be muted, if the
//   value is 0, the player will be unmuted.
//   len: The length of the muter_id and mute arrays.
//   player_id: The unique id for the player that will be muted or unmuted.
// Return value:
//   ICE_ERROR_NONE if successful; non-zero ICE_ERROR if it fails.
// Note:
//   If one or more player id given in the muter_id array is invalid, the
//   function will fail and no player will be muted/unmuted
ICE_ERROR ICE_mute_many_to_one(ICE* d, const unsigned long long* muter_id, const unsigned char* mute, const unsigned int len, const unsigned long long player_id);


// Description:
//   Emphasises an individual player for another player.
// Parameters:
//   d: Valid ICE pointer initialized by one of the ICE_init functions.
//   player_id: The unique id of the player who wishes to emphasise the other player.
//   emphasised_id: The unique id for the player to be emphasised.
//   emphasis: If emphasis = 1 then the player is at normal volume; if emphasis = 0,
//   the player will be muted. If 0 < emphasis < 1, the players volume will
//   be reduced. If emphasis > 1, then the player's volume will be increased
//   Values above 255 will have no additional effect - the player will be at full
//   volume whenever he can be heard.
// Return value:
//   ICE_ERROR_NONE if successful; non-zero ICE_ERROR if it fails.
ICE_ERROR ICE_emphasise_one_to_one(ICE* d, const unsigned long long player_id, const unsigned long long emphasised_id, const float emphasis);


// Description:
//   applies emphasis to a list of players for a single player.
// Parameters:
//   d: Valid ICE pointer initialized by one of the ICE_init functions.
//   player_id: The unique id of the player who wishes to emphasise the other
//   players.
//   emphasised_id: The unique ids of the players to be emphasised, given as an array of
//   length len.
//   emphasisval: An array of emphasis values corresponding to the ids in emphasis_id
//   If emphasis = 1 then the player is at normal volume; if emphasis = 0,
//   the player will be muted. If 0<emphasis<1, the players volume will
//   be reduced. If emphasis > 1, then the player's volume will be increased
//   Values above 255 will have no additional effect - the player will be at full
//   volume whenever he can be heard.
//   len: The length of the emphasised_id and emphasisval arrays
// Return value:
//   ICE_ERROR_NONE if successful; non-zero ICE_ERROR if it fails.
// Note: 
//   If one or more player id given in the emphasised_id array is invalid, the function
//   will fail and no player will be emphasised.
//   If one or more emphasis level given in the emphasisval array is invalid (such as a
//   negative value), the function will fail and no player will be emphasised.
ICE_ERROR ICE_emphasise_one_to_many(ICE* d, const unsigned long long player_id, const unsigned long long* emphasised_id, const float* emphasisval, const unsigned int len);


// Description:
//   applies emphasis to a single player for a list of players.
// Parameters:
//   d: Valid ICE pointer initialized by one of the ICE_init functions.
//   emphasiser_id: The unique ids of the players that will have player_id emphasised,
//   given in an array of length len.
//   emphasisval: An array of emphasis values corresponding to the ids in emphasiser_id
//   If emphasis = 1 then the player is at normal volume; if emphasis = 0,
//   the player will be muted. If 0<emphasis<1, the players volume will
//   be reduced. If emphasis > 1, then the player's volume will be increased
//   Values above 255 will have no additional effect - the player will be at full
//   volume whenever he can be heard
//   len: The length of the emphasiser_id and emphasisval arrays.
//   player_id: The unique id of the player who will be emphasized by players listed in
//   emphasiser_id.
// Return value:
//   ICE_ERROR_NONE if successful; non-zero ICE_ERROR if it fails.
// Note: 
//   If one or more player id given in the emphasiser_id array is invalid, the function
//   will fail and no player will be emphasised.
//   If one or more emphasis level given in the emphasisval array is invalid (such as a
//   negative value), the function will fail and no player will be emphasised.
ICE_ERROR ICE_emphasise_many_to_one(ICE* d, const unsigned long long* emphasiser_id, const float* emphasisval, const unsigned int len, const unsigned long long player_id);

// Description:
//   Emphasises an individual player for another player in a specific channel.
// Parameters:
//   d: Valid ICE pointer initialized by one of the ICE_init functions.
//   player_id: The unique id of the player who wishes to emphasise the other player.
//   emphasised_id: The unique id for the player to be emphasised.
//   emphasis: If emphasis = 1 then the player is at normal volume; if emphasis = 0,
//   the player will be muted. If 0 < emphasis < 1, the players volume will
//   be reduced. If emphasis > 1, then the player's volume will be increased
//   Values above 255 will have no additional effect - the player will be at full
//   volume whenever he can be heard.
//   channel_id: The if of the channel on which to apply the gain on
// Return value:
//   ICE_ERROR_NONE if successful; non-zero ICE_ERROR if it fails.
ICE_ERROR ICE_emphasise_one_to_one_channel(ICE* d, const unsigned long long player_id, const unsigned long long emphasised_id, const float emphasis, const unsigned int channel_id);


// Description:
//   applies emphasis to a list of players for a single player in a specific channel.
// Parameters:
//   d: Valid ICE pointer initialized by one of the ICE_init functions.
//   player_id: The unique id of the player who wishes to emphasise the other
//   players.
//   emphasised_id: The unique ids of the players to be emphasised, given as an array of
//   length len.
//   emphasisval: An array of emphasis values corresponding to the ids in emphasis_id
//   If emphasis = 1 then the player is at normal volume; if emphasis = 0,
//   the player will be muted. If 0<emphasis<1, the players volume will
//   be reduced. If emphasis > 1, then the player's volume will be increased
//   Values above 255 will have no additional effect - the player will be at full
//   volume whenever he can be heard.
//   len: The length of the emphasised_id and emphasisval arrays
//   channel_id: The if of the channel on which to apply the gain on
// Return value:
//   ICE_ERROR_NONE if successful; non-zero ICE_ERROR if it fails.
// Note: 
//   If one or more player id given in the emphasised_id array is invalid, the function
//   will fail and no player will be emphasised.
//   If one or more emphasis level given in the emphasisval array is invalid (such as a
//   negative value), the function will fail and no player will be emphasised.
ICE_ERROR ICE_emphasise_one_to_many_channel(ICE* d, const unsigned long long player_id, const unsigned long long* emphasised_id, const float* emphasisval, const unsigned int len, const unsigned int channel_id);


// Description:
//   applies emphasis to a single player for a list of players in a specific channel.
// Parameters:
//   d: Valid ICE pointer initialized by one of the ICE_init functions.
//   emphasiser_id: The unique ids of the players that will have player_id emphasised,
//   given in an array of length len.
//   emphasisval: An array of emphasis values corresponding to the ids in emphasiser_id
//   If emphasis = 1 then the player is at normal volume; if emphasis = 0,
//   the player will be muted. If 0<emphasis<1, the players volume will
//   be reduced. If emphasis > 1, then the player's volume will be increased
//   Values above 255 will have no additional effect - the player will be at full
//   volume whenever he can be heard
//   len: The length of the emphasiser_id and emphasisval arrays.
//   player_id: The unique id of the player who will be emphasized by players listed in
//   emphasiser_id.
//   channel_id: The if of the channel on which to apply the gain on
// Return value:
//   ICE_ERROR_NONE if successful; non-zero ICE_ERROR if it fails.
// Note: 
//   If one or more player id given in the emphasiser_id array is invalid, the function
//   will fail and no player will be emphasised.
//   If one or more emphasis level given in the emphasisval array is invalid (such as a
//   negative value), the function will fail and no player will be emphasised.
ICE_ERROR ICE_emphasise_many_to_one_channel(ICE* d, const unsigned long long* emphasiser_id, const float* emphasisval, const unsigned int len, const unsigned long long player_id, const unsigned int channel_id);


// Description:
//   Returns the version string of the runtime ICE library. This is the version
//   of the ICE library currently loaded by the game. The version of the ICE
//   library used at compile time can be retrieved as ICE_VERSION, defined the
//   ICE.h header file.
// Parameters:
//   None.
// Return value:
//   Runtime version string.
const char* ICE_version();


// Description:
//   Converts a dotted IP string to an ICE_IP structure.
// Parameters:
//   ip: Dotted IP string, such as, "127.0.0.1".
// Return value:
//   ICE_IP structure. Bad input results in an undefined return value.
ICE_IP ICE_str2ip(const char* ip);


// Description:
//   Gets the last error code set by an ICE function.
// Parameters:
//   d: Valid ICE pointer initialized by one of the ICE_init functions.
// Return value:
//   The last error set by an ICE function.
ICE_ERROR ICE_get_last_error(const ICE* d);


// Description:
//   Converts an ICE error code into a human readable string.
// Parameters:
//   e: The error code.
// Return value:
//   The human readable string.
const char* ICE_error_str(const ICE_ERROR e);


// Description:
//   Adds a player with talking privileges to a channel.
// Parameters:
//   d: Valid ICE pointer initialized by one of the ICE_init functions.
//   player_id: The unique id for the player being added.
//   channel_id: The id of the channel to join.
//   channel_mapping: The id that the audio client of the player would use to
//   refer to this channel. This id is sent along with audio to the audio server
//   when the player is talking on a particular channel.
//   player_priority: The priority that this player has in the channel (ignored
//   for spatial channels).
// Return value:
//   ICE_ERROR_NONE if successful; non-zero ICE_ERROR if it fails.
ICE_ERROR ICE_channel_talk_join(ICE* d, const unsigned long long player_id, const unsigned int channel_id, const unsigned char channel_mapping, const unsigned short player_priority);


// Description:
//   Revokes the talking privileges of a player from a channel.
// Parameters:
//   d: Valid ICE pointer initialized by one of the ICE_init functions.
//   player_id: The unique id of the player being removed from the channel.
//   channel_id: The id of the channel from which the player is being removed.
// Return value:
//   ICE_ERROR_NONE if successful; non-zero ICE_ERROR if it fails.
ICE_ERROR ICE_channel_talk_leave(ICE* d, const unsigned long long player_id, const unsigned int channel_id);


// Description:
//   Revokes the talking privileges of a player from all channels.
// Parameters:
//   d: Valid ICE pointer initialized by one of the ICE_init functions.
//   player_id: The unique id for the player being removed.
// Return value:
//   ICE_ERROR_NONE if successful; non-zero ICE_ERROR if it fails.
ICE_ERROR ICE_channel_talk_leave_all(ICE* d, const unsigned long long player_id);


// Description:
//   Adds a player to a channel with listening privileges.
// Parameters:
//   d: Valid ICE pointer initialized by one of the ICE_init functions.
//   player_id: The unique id for the avatar being added.
//   channel_id: The id of the channel to join.
//   channel_priority: The priority that this player gives the channel (ignored
//   for spatial channels).
// Return value:
//   ICE_ERROR_NONE if successful; non-zero ICE_ERROR if it fails.
ICE_ERROR ICE_channel_listen_join(ICE* d, const unsigned long long player_id, const unsigned int channel_id, const unsigned short channel_priority);


// Description:
//   Revokes the listening privileges of a player from a channel.
// Parameters:
//   d: Valid ICE pointer initialized by one of the ICE_init functions.
//   player_id: The unique id for the player being removed.
//   channel_id: The id of the channel from which the player is being removed.
// Return value:
//   ICE_ERROR_NONE if successful; non-zero ICE_ERROR if it fails.
ICE_ERROR ICE_channel_listen_leave(ICE* d, const unsigned long long player_id, const unsigned int channel_id);


// Description:
//   Revokes a player's listening privileges from all channels.
// Parameters:
//   d: Valid ICE pointer initialized by one of the ICE_init functions.
//   player_id: The unique id for the player being removed.
// Return value:
//   ICE_ERROR_NONE if successful; non-zero ICE_ERROR if it fails.
ICE_ERROR ICE_channel_listen_leave_all(ICE* d, const unsigned long long player_id);


// Description:
//   Returns an available channel identifier.
// Parameters:
//   d: Valid ICE pointer initialized by one of the ICE_init functions.
// Return value:
//   Returns an available channel identifier.
unsigned int ICE_get_free_chanid(ICE* d);


// Description:
//   Creates a spatial channel.
// Parameters:
//   d: Valid ICE pointer initialized by one of the ICE_init functions.
//   channel_id: a unique ID for the new channel, this must be unique among
//   all channel (spatial, nonspatial). It may be reused after the channel has
//   been destroyed. Note that channel_id must be smaller than 2^32 - 1
//  (0xFFFFFFFF is not valid) in normal mode and smaller than 2^16 - 1 in
//   multi-game mode (see ICE_init_multigame for more infomation about
//   multi-game ID limitations).
//   channels (spatial and nonspatial) and is used to reference this channel.
//   hearing_range: Assigns the hearing range for this channel; any avatar
//   further away than this will not be heard.
//   profile: An array of ICE_HEARING_PROFILE_POINT that determines the audio 
//   dropoff as players are separated. If NULL, default profile is used (len
//   is ignored).
//   len: Size of the profile array.
// Return value:
//   ICE_ERROR_NONE if successful; non-zero ICE_ERROR if it fails.
ICE_ERROR ICE_create_channel_spatial(ICE* d, const unsigned int channel_id, const unsigned int hearing_range, const ICE_HEARING_PROFILE_POINT* profile, const unsigned int len);


// Description:
//   Creates a non-spatial channel.
// Parameters:
//   d: Valid ICE pointer initialized by one of the ICE_init functions.
//   channel_id: a unique ID for the new channel, this must be unique among
//   all channel (spatial, nonspatial). It may be reused after the channel has
//   been destroyed. Note that channel_id must be smaller than 2^32 - 1
//  (0xFFFFFFFF is not valid) in normal mode and smaller than 2^16 - 1 in
//   multi-game mode (see ICE_init_multigame for more infomation about
//   multi-game ID limitations).
// Return value:
//   ICE_ERROR_NONE if successful; non-zero ICE_ERROR if it fails.
// NOTE: 
//   By default no client processing is done on the channel.
ICE_ERROR ICE_create_channel_nonspatial(ICE* d, const unsigned int channel_id);


// Description:
//   Sets a property of a channel.
// Parameters:
//   d: Valid ICE pointer initialized by one of the ICE_init functions.
//   channel_id: The id of the channel for which you want to modify a property.
//   prop: The identifier of the property you wish to set.
//   val: Pointer to the value to assign to the channel property.
//   len: The length of what val points at in bytes.
// Return value:
//   ICE_ERROR_NONE if successful; non-zero ICE_ERROR if it fails.
// NOTE: 
//   For more information on properties look at the declaration of
//   ICE_CHAN_PROPERTY.
ICE_ERROR ICE_set_channel_property(ICE* d, const unsigned int channel_id, const ICE_CHAN_PROPERTY prop, const void* val, const unsigned int len);


// Description:
//   Gets a property of a channel.
// Parameters:
//   d: Valid ICE pointer initialized by one of the ICE_init functions.
//   channel_id: The id of the channel for which you want to retrieve a property.
//   prop: The identifier of the property you wish to get.
//   val: Pointer to where the returned value will be stored.
//   len: Pointer to an integer that is set to the size (in bytes) of what val
//   points at. If the function returns without error, this value is then set
//   to the size of the channel property retrieved.
// Return value:
//   ICE_ERROR_NONE if successful; non-zero ICE_ERROR if it fails.
// NOTE: 
//   For more information on properties look at the declaration of
//   ICE_CHAN_PROPERTY.
ICE_ERROR ICE_get_channel_property(ICE* d, const unsigned int channel_id, const ICE_CHAN_PROPERTY prop, void* val, unsigned int* len);


// Description:
//   Destroys a previously created channel and removes all avatars from it.
// Parameters:
//   d: Valid ICE pointer initialized by one of the ICE_init functions.
//   channel_id: The channel id of a previously created channel that you wish
//   to destroy.
// Return value:
//   ICE_ERROR_NONE if successful; non-zero ICE_ERROR if it fails.
ICE_ERROR ICE_destroy_channel(ICE* d, const unsigned int channel_id);


// Description:
//   Gets statistics about the running audio and control servers.
// Parameters:
//   d: Valid ICE pointer initialized by one of the ICE_init functions.
//   s: The ICE_server_stats structure to fill out.
// Return value:
//   ICE_ERROR_NONE if successful; non-zero ICE_ERROR if it fails.
// NOTE: 
//   The ICE_STATS structure is filled with the last known data, but
//   the data is reset after 4 seconds of no contact with the control
//   server.
ICE_ERROR ICE_get_servers_stats(ICE* d, ICE_SERVERS_STATS* s);


// Description:
//   Specifies the map file to use for wall calculations for a given spatial
//   channel in the current virtual world.
// Parameters:
//   d: Valid ICE pointer initialized by one of the ICE_init functions.
//   channel_id: The id of the channel for which the map is set.
//   map_name: The name of the map to use.
// Return value:
//   ICE_ERROR_NONE if successful; non-zero ICE_ERROR if it fails.
// NOTE:
//   1. Map files to be loaded by control servers must already be converted
//   into suitable binary format.
//   2. Objects contained in a map file define the boundaries of that map.
//   However, if a model is added outside these boundaries, they will be 
//   recomputed to contain the new model.
ICE_ERROR ICE_map_channel(ICE* d, const unsigned int channel_id, const char* map_name);


// Description:
//   Adds a new model to a channel's map. The new model is not present in the
//   map until it's instantiated.
// Parameters:
//   d: Valid ICE pointer initialized by one of the ICE_init functions.
//   model_name: The name of the new model; must be NULL terminated and limited
//   to 1,000 bytes.
//   triangles: An array of integers containing the coordinates of triangle
//   points that make up the model. Each triangle is represented by nine integer
//   values. A example is:
//   (t1x1,t1y1,t1z1,t1x2,t1y2,t1z2,t1x3,t1y3,t1z3, ...)
//   triangle_count: The number of triangles represented in the triangles array.
//   The array will be exactly 9*triangle count. There must be no more than
//   16,384 triangles.
//   attenuations: The attenuation of each triangle in order, the length of this
//   array will be triangle_count.
//   channel_id: The id of the channel in which the model is being added.
// Return value:
//   ICE_ERROR_NONE if successful; non-zero ICE_ERROR if it fails.
ICE_ERROR ICE_add_model(ICE* d, const char* model_name, const int* triangles, const unsigned int triangle_count, const float* attenuations, const unsigned int channel_id);


// Description:
//   Adds an instance of a model to a channel's map.
// Parameters:
//   d: Valid ICE pointer initialized by one of the ICE_init functions.
//   model_name: The name of the model to use; must be NULL terminated and
//   limited to 1,000 bytes.
//   instance_id: The id to assign to the instance for future reference; if the
//   id is already taken a new id is provided.
//   transform: The transform to apply to the model to obtain an instance with 
//   the correct location and rotation. If this is NULL, then the instance 
//   will not be moved or scaled and will be in the same place as the original
//   model.
//   atten: A multiplier to be applied to the model's attenuation. Set atten=0
//   to prevent an instance from blocking sound; set atten=1 to use the model's 
//   default attenuation, or >1 to increase attenuation.
//   channel_id: The id of the channel in which the model instance is being
//   added.
// Return value:
//   ICE_ERROR_NONE if successful; non-zero ICE_ERROR if it fails.
// NOTE: 
//   Objects contained in a map file define the boundaries of that map, but if
//   a model is added outside these boundaries, they will be recomputed to 
//   contain the new model.
ICE_ERROR ICE_add_model_instance(ICE* d, const char* model_name, unsigned int *instance_id, const float transform[3][4], const float atten, const unsigned int channel_id);


// Description:
//   Removes an instance of a model from a channel's map. This should be used
//   instead of transform with atten = 0 if you're not going to need the
//   instance again any time soon, because even disabled instances produce a
//   small performance and memory hit.
// Parameters:
//   d: Valid ICE pointer initialized by one of the ICE_init functions.
//   instance_id: The id of the instance to remove.
//   channel_id: The id of the channel in which the model is being removed.
// Return value:
//   ICE_ERROR_NONE if successful; non-zero ICE_ERROR if it fails.
ICE_ERROR ICE_remove_model_instance(ICE* d, const unsigned int instance_id, const unsigned int channel_id);


// Description:
//   Transforms or enables/disables an existing model instance in a channel's
//   map.
// Parameters:
//   d: Valid ICE pointer initialized by one of the ICE_init functions.
//   instance_id: The id of the model instance to transform.
//   transform: The transform to apply to the model to obtain an instance with
//   the correct location and rotation. If this is NULL, the instance will not
//   be moved or scaled and will have the same position and size as the original
//   model.
//   atten: A multiplier to be applied to the model's attenuation. Set atten = 0
//   to prevent an instance from blocking sound; set atten = 1 to use the
//   model's default attenuation, or to >1 to increase attenuation.
//   channel_id: The id of the channel in which the model instance is being
//   transformed.
// Return value:
//   ICE_ERROR_NONE if successful; non-zero ICE_ERROR if it fails.
// NOTE: 
//   1.The model instance resulting from calling this function is obtained by
//   transforming the original model (template), but not from transforming an
//   instance. For example, if we create a model at the origin, then create an
//   instance at offset b, and finally apply a transform with offset c, the
//   resulting model will be at offset c, not offset b+c.
//   2. The function exists as an optimization. Using
//   ICE_transform_model_instance to move an instance is more efficient than
//   removing the instance and creating a new one. Additionally disabling or
//   enabling an existing instance without moving it can be achieved by 
//   supplying NULL as the transformation matrix and simply changing the atten
//   value.
ICE_ERROR ICE_transform_model_instance(ICE* d, const unsigned int instance_id, const float transform[3][4], const float atten, const unsigned int channel_id);


// Description:
//   This removes a model from a given channel. Any instances of the model must
//   be removed before the model can be removed. A library can only remove 
//   models that it created.
// Parameters:
//   d: Valid ICE pointer initialized by one of the ICE_init functions.
//   name: The name of the model being removed.
//   channel_id: The channel this model was added to.
// Return value:
//   ICE_ERROR_NONE if successful; non-zero ICE_ERROR if it fails.
ICE_ERROR ICE_remove_model(ICE* d, const char* name, const unsigned int channel_id);


// Description:
//   This removes a static model instance from a channel's map. If multiple
//   channels share a map, this change will only affect the given channel.
//   A static instance is an instance that was present in the map file loaded
//   by the control server. 
// Parameters:
//   d: Valid ICE pointer initialized by one of the ICE_init functions.
//   id: The id of the static instance to be removed. The use of this function
//   requires static instance ids to be included in your map file.
//   channel_id: The id of the channel in which the the static model instance is
//   being removed.
// Return value:
//   ICE_ERROR_NONE if successful; non-zero ICE_ERROR if it fails.
// NOTE: 
//   This function could be used to implement destructible objects such as walls
//   that were present in the original map file. 
ICE_ERROR ICE_remove_static_model_instance(ICE* d, const unsigned int id, const unsigned int channel_id);


// Description:
//   This copies the map used by the source channel to the destination channel
//   and replaces that channel's old map. The entire map is copied, including:
//   1. All static model instances that haven't been removed.
//   2. All dynamic models and instances that have been added by the current
//   library.
// Parameters:
//   d: Valid ICE pointer initialized by one of the ICE_init functions.
//   source_channel_id: The id of the channel that the map is to be copied from.
//   dest_channel_id: The id of the channel that the map is to be copied to.
// Return value:
//   ICE_ERROR_NONE if successful; non-zero ICE_ERROR if it fails.
ICE_ERROR ICE_copy_map(ICE* d, const unsigned int source_channel_id, const unsigned int dest_channel_id);


// Description:
//   This function copies a model that was created by the current library, from
//   one channel to another.
// Parameters:
//   d: Valid ICE pointer initialized by one of the ICE_init functions.
//   model_name: The name of the model to be copied; must be NULL terminated and
//   limited to 1000 bytes.
//   source_channel_id: The id of the channel that the model is to be copied
//   from.
//   dest_channel_id: The id of the channel that the model is to be copied to.
// Return value:
//   ICE_ERROR_NONE if successful; non-zero ICE_ERROR if it fails.
ICE_ERROR ICE_copy_model(ICE* d, const char* model_name, const unsigned int source_channel_id, const unsigned int dest_channel_id);


// Description:
//   This applies a given yaw (rotation around the z axis) to a transformation
//   matrix. Tranforming an object by the resulting matrix has the same effect
//   as transforming the object by the original matrix, and then rotating it in
//   place by the given angle.
// Parameters:
//   matrix: The matrix to be modified.
//   angle: The angle in degrees by which to change the yaw; positive rotation
//   direction is from the positive y axis towards the positive x axis.
// Return value:
//   No return value.
void ICE_yaw(float matrix[3][4], const float angle);


// Description:
//   This applies a given pitch (rotation around the x axis) to a transformation
//   matrix.Transforming an object by the resulting matrix has the same effect
//   as transforming the object by the original matrix, and then rotating it in
//   place by the given angle.
// Parameters:
//   matrix: The matrix to be modified.
//   angle: The angle in degrees by which to change the pitch; positive rotation
//   direction is from positive z axis towards the positive y axis.
// Return value:
//   No return value.
void ICE_pitch(float matrix[3][4], const float angle);


// Description:
//   This applies a given roll (rotation around the y axis) to a transformation
//   matrix. Tranforming an object by the resulting matrix has the same effect
//   as transforming the object by the original matrix, and then rotating it in
//   place by the given angle.
// Parameters:
//   matrix: The matrix to be modified.
//   angle: The angle in degrees to change the roll by; positive rotation
//   direction is from the positive z axis towards positive x axis.
// Return value:
//   No return value.
void ICE_roll(float matrix[3][4], const float angle);


// Description:
//   This applies the given translation to the given matrix so the matrix will
//   translate the object from its original position to the given position.
// Parameters:
//   matrix: The matrix to modify.
//   x: The new x coordinate of the object.
//   y: The new y coordinate of the object.
//   z: The new z coordinate of the object.
// Return value:
//   No return value.
void ICE_translate(float matrix[3][4], const float x, const float y, const float z);


// Description:
//   This applies the given translation to the given matrix so the matrix will
//   scale the object from its original size while remaining at the same
//   position and rotation.
// Parameters:
//   matrix: The matrix to modify.
//   scale: The new scale desired for the object (if scale=1.0f, the object is
//   at the original scale).
// Return value:
//   No return value.
// NOTE:
//  It is better to create large models and scale them down when creating
//  instances than to create small models to be scaled up. This is because
//  scaling up can result in sound occlusion inaccuracies.
void ICE_scale(float matrix[3][4], const float scale);


// Description:
//   This resets the matrix(to an identity matrix). You must allocate the array
//   before the call.
// Parameters:
//   matrix: An already-allocated array of floats that will be initialized.
// Return value:
//   No return value.
void ICE_reset_matrix(float matrix[3][4]);


// Batchable functions
// 
// Some functions are batchable, and calling ICE_begin_batch() before calling 
// batchable function will defer their effect until ICE_end_batch() is called,
// at which point the cumulated effect of the functions called will take place.
// One example where batching functions calls can be useful is when removing a
// player from all but a few channels. Without batching, this could be done by
// calling ICE_channel_listen_leave_all() followed by ICE_channel_listen_join()
// for the channels that the player is to remain in. However this causes the
// player to be disconnected and then reconnected. If we use the same API calls
// within a batch, the desired result is achieved without disconnection and 
// reconnection.  
// NOTE: Do not call non batchable functions while batching. The results are
// undefined.
//
// Batchable functions are:
//  ICE_channel_talk_join
//  ICE_channel_talk_leave
//  ICE_channel_talk_leave_all
//  ICE_channel_listen_join
//  ICE_channel_listen_leave
//  ICE_channel_listen_leave_all


// Description:
//   Specify the beginning of a batch.
// Parameters:
//   d: Valid ICE pointer initialized by one of the ICE_init functions.
// Return value:
//   ICE_ERROR_NONE if successful; non-zero ICE_ERROR if it fails.
ICE_ERROR ICE_begin_batch(ICE* d);


// Description:
//   Specify the end of a batch. 
// Parameters:
//   d: Valid ICE pointer initialized by one of the ICE_init functions.
// Return value:
//   ICE_ERROR_NONE if successful; ICE_ERROR_NOT_BATCHING as a non blocking 
//   error if ICE_begin_batch() has not been called.
ICE_ERROR ICE_end_batch(ICE* d);


// Description:
//   Requests a complaint ticket and sends the ticket to a complaint server.
// Parameters:
//   d: Valid ICE pointer initialized by one of the ICE_init functions.
//   complainer_id: The id of player who creates this complaint ticket.
//   has_complainee: If non-zero, the ticket is about a specific player whose
//   id given by complainee_id.
//   complainee_id: The id of the player being complained about if
//   has_complainee is non-zero; unused otherwise.
//   category: String indicating complaint category, for example "racist" or
//   "offensive language"; It must be NULL terminated and cannot be NULL.
//   subject: Subject of ticket; It must be NULL terminated and cannot be NULL. 
//   msg: Text message to include in ticket; It must be NULL terminated and
//   cannot be NULL. 
// Return value:
//   ICE_ERROR_NONE if successful; non-zero ICE_ERROR if it fails.
// NOTE:
//   Even if this function returns successfully, a complaint ticket is only
//   created if the audio server was started with complaint functionality
//   enabled (-complaint_ip command line option).
ICE_ERROR ICE_complaint_request(ICE *d, const unsigned long long complainer_id, const unsigned int has_complainee, const unsigned long long complainee_id, const char* category, const char *subject, const char* msg);


// Description
//   Pass encapsulated data coming from a Dolby Axon client to libICE
//   for delivery to the audio server.
// Parameters:
//   d: Valid ICE pointer initialised by one of the ICE_init functions
//   msg: pointer to the data
//   len: size of the data
//   player_id: the id of the player this data comes from
// Return value:
//   ICE_ERROR_NONE if successful; non-zero ICE_ERROR if it fails.
ICE_ERROR ICE_receive_capsule(ICE* d, const char* msg, unsigned int len, unsigned long long player_id);


#ifdef __cplusplus
}
#endif
#endif

