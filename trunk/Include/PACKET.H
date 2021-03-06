/****************************************************************************/
/*    FILE:  PACKET.H                                                       */
/****************************************************************************/
#ifndef _PACKET_H_
#define _PACKET_H_

#include "GENERAL.H"
#include "GROUP.H"
#include "VIEWFILE.H"

#define MAX_SIZE_PASSWORD 12

/* ------------------------------------------------------------------------ */
/* The following list is the types of blocks that are sent back and forth */
/* between client and server (mainly to the client). */

#define CLIENT_MEMORY_BLOCK_UPDATE_BLOCK       0
#define CLIENT_MEMORY_BLOCK_CHARACTER_LISTING  1
#define MEMORY_BLOCK_CHARACTER_DATA            2
#define MEMORY_BLOCK_ACCOUNT_INFO              3
#define MEMORY_BLOCK_TYPE_UNKNOWN              4

/* ------------------------------------------------------------------------ */

/* Data blocks to send: */
typedef struct {
    T_byte8 ownersName[60] ;
    T_byte8 street[80] ;
    T_byte8 city[40] ;
    T_byte8 state[20] ;
    T_byte8 zipcode[14] ;
    T_byte8 phoneHome[20] ;
    T_byte8 phoneWork[20] ;
    T_byte8 billingMethod[20] ;
    T_byte8 billingData[60] ;
    T_word32 accountID ;
} T_dataBlockAccountInfo ;

/* ------------------------------------------------------------------------ */

#define SHORT_PACKET_LENGTH 10
#define LONG_PACKET_LENGTH 72

#define PACKET_PREFIX     0xCC

#define MAX_MESSAGE_LEN      40

typedef struct {
    T_byte8    prefix            PACK;
    T_byte8    packetLength      PACK;
    T_word32   id                PACK;
    T_word16   checksum          PACK;
} T_packetHeader ;

typedef struct {
    T_packetHeader header        PACK;
    T_byte8        data[SHORT_PACKET_LENGTH]
                                 PACK;
} T_packetShort ;

typedef struct {
    T_packetHeader header        PACK;
    T_byte8        data[LONG_PACKET_LENGTH]
                                 PACK;
} T_packetLong ;

typedef struct {
    T_packetHeader header        PACK;
    T_byte8        data[LONG_PACKET_LENGTH]       PACK;
} T_packetEitherShortOrLong ;

T_sword16 PacketSendShort(T_packetShort *p_shortPacket) ;

T_sword16 PacketSendLong(T_packetLong *p_longPacket) ;

T_sword16 PacketSendAnyLength(T_packetEitherShortOrLong *p_anyPacket) ;

T_sword16 PacketSend(T_packetEitherShortOrLong *p_packet) ;

T_sword16 PacketGet(T_packetLong *p_packet) ;

T_void PacketSetId (T_packetEitherShortOrLong *p_packet, T_word32 packetID);

T_void PacketReceiveData(T_void *p_data, T_byte8 size);

/* !!! Needs to be moved! */
typedef struct {
    T_byte8 command              PACK;
    T_sword16 x                  PACK;
    T_sword16 y                  PACK;
    T_word16 angleAndStance      PACK;
    T_byte8 player               PACK;
    T_sword16 height             PACK;
} T_movePacketData ;

typedef struct {
    T_byte8 command              PACK;
    T_byte8 visage               PACK;
    T_byte8 demeanor             PACK;
    T_word32 accountNum          PACK ;
} T_loginPacket;

typedef struct {
    T_byte8 command              PACK;
    T_word16 player              PACK;
    T_word16 objectId            PACK;
} T_logoffPacket ;

typedef struct {
    T_byte8 command              PACK;
    T_word16 player              PACK;
    T_word16 weapon              PACK;
} T_changeWeaponPacket ;

typedef struct {
    T_byte8 command              PACK;
    T_word16 player              PACK;
    T_word16 action              PACK;
} T_actionPacket ;

typedef struct {
    T_byte8 command              PACK;
    T_byte8 player               PACK;
    T_word16 damage              PACK;
    T_byte8 damageType           PACK;
} T_attackPacket ;

typedef struct {
    T_byte8 command              PACK;
    T_word16 player              PACK;
    T_word16 damage              PACK;
    T_byte8 type                 PACK;
    T_sword16 x                  PACK;
    T_sword16 y                  PACK;
    T_word16 ownerObjID          PACK;
} T_damagePacket ;

typedef struct {
    T_byte8 command              PACK;
    T_sword16 x, y               PACK;
    T_byte8 radius               PACK;
    T_byte8 damage               PACK;
    T_byte8 player               PACK;
} T_damageAtPacket ;

typedef struct {
    T_byte8 command              PACK;
    T_word16 sector              PACK;
    T_word16 activity            PACK;
    T_word16 player              PACK;
} T_reverseSectorPacket ;

typedef struct {
    T_byte8 command              PACK ;
    T_gameGroupID groupID        PACK ;
    T_byte8 syncData[1]          PACK ;
} T_syncPacket ;

typedef struct {
    T_byte8 command              PACK;
    T_word16 objNum              PACK;
    T_word16 player              PACK;
} T_pickUpPacket ;

typedef struct {
    T_byte8 command              PACK;
    T_word16 player              PACK;
    T_gameGroupID groupID        PACK;
    T_byte8 message[MAX_MESSAGE_LEN+1]
                                 PACK;
} T_messagePacket ;

#define DOOR_PACKET_OPEN_DOOR        0
#define DOOR_PACKET_CLOSE_DOOR       1
typedef struct {
    T_byte8 command              PACK;
    T_word16 sector              PACK;
    T_word16 player              PACK;
    T_byte8 open_or_close        PACK;  /* 0 = open, 1 = close */
} T_doorPacket ;

typedef struct {
    T_byte8 command              PACK;
    T_word16 player              PACK;
    T_sword16 x                  PACK;
    T_sword16 y                  PACK;
    T_word16 saying              PACK;
    T_byte8 loudnessAndVoice     PACK;
} T_cannedSayingPacket ;

/** Masks for the loudness/voice field **/
#define CANNED_SAYING_LOUDNESS   0x0F    /** ---- XXXX **/
#define CANNED_SAYING_VOICE      0xF0    /** XXXX ---- **/
#define CANNED_SAYING_LOUDNESS_AND_VOICE(loud,voice) \
             ((T_byte8)(((voice & 0x0F) << 4) | (loud & 0x0F)))

typedef struct {
    T_byte8   command            PACK ;
    T_word16  id                 PACK ;
    T_sword16 x                  PACK ;
    T_sword16 y                  PACK ;
    T_sword16 z                  PACK ;
    T_byte8   angleAndStance     PACK ;
} T_objectPositionPacket ;

/* Define masks for the object position packet */
#define OBJECT_POSITION_PACKET_ANGLE  0xF8
#define OBJECT_POSITION_PACKET_STANCE 0x07
#define OBJECT_POS_ANGLE_AND_STANCE(ang, sta)  ((T_byte8)  \
            ( (((ang) & 0xF800) >> 8) | ((sta) & 7) ) )


typedef struct {
    T_byte8 command              PACK;
    T_word16 objectID            PACK;
} T_objectDestroyPacket;


/** Sent to perform a "special effect" **/
/** Current special effects are: **/

#define SPECIAL_EFFECT_AREA_SOUND   0  /** Area sound: id=sound # **/

typedef struct {
    T_byte8 command              PACK;
    T_byte8 effect               PACK;
    T_sword16 x                  PACK;
    T_sword16 y                  PACK;
    T_word16 radius              PACK;
    T_word16 id                  PACK;
} T_specialEffectPacket;


/** Sent to request permission to take an object **/
typedef struct {
    T_byte8 command              PACK;
    T_word16 id                  PACK;  /** 'Server ID' of object **/
    T_word16 playerID            PACK;  /** Player ID **/
} T_objectRequestTakePacket;


/** The following packet is sent to reply to a client's request **/
/** to take an object.  **/
/** the 'reply' byte can be one of the following values: **/

#define TAKE_PERMISSION_GRANTED     0 /** Okay, taken. **/
#define TAKE_NO_WAY_TOO_FAR_AWAY    1 /** Object is too far away. **/
#define TAKE_NO_WAY_STUCK_TO_GROUND 2 /** Object is not marked grabbable. **/
#define TAKE_NO_WAY_DOESNT_EXIST    3 /** Object doesn't exist! **/

typedef struct {
    T_byte8 command              PACK;
    T_byte8 reply                PACK;  /** See just above. **/
    T_word16 id                  PACK;  /** 'Server ID' of object **/
} T_objectTakeReplyPacket;


/** When a client drops an object, it needs to send the following **/
/** packet to the server, requesting the creation of an object. **/
/** Also, when the server creates an object, she needs to send this **/
/** packet to all her clients to tell them what she creates. **/

typedef struct {
    T_byte8 command              PACK;   /*   1 */ 
    T_word16 objectID            PACK;   /*   2 */  /* Ignored for C->S */
    T_word16 objectType          PACK;   /*   2 */
    T_word32 X                   PACK;   /*   4 */
    T_word32 Y                   PACK;   /*   4 */
    T_word32 Z                   PACK;   /*   4 */
    T_word32 XV                  PACK;   /*   4 */
    T_word32 YV                  PACK;   /*   4 */
    T_word32 ZV                  PACK;   /*   4 */
    T_word16 angle               PACK;   /* + 2 */

    T_word16 bodyParts[MAX_BODY_PARTS]           PACK;   /* body parts.  + 20 bytes */
} T_objectAddPacket;                     /*= 51 bytes = LONG packet. */

/** This packet is similar to an ObjectAdd, but it is for a "projectile" **/
/** -- an object entering the world projected from another object.  Thus, **/
/** if we provide an object ID and a linear velocity, the initial X, Y, **/
/** and Z are known, and XV, YV, and ZV are readily computed. **/

/** Note that I only had room for an 8-bit initial speed.  To get the **/
/** real speed, just sign-extend it. **/

#if 0
typedef struct {
    T_byte8 command              PACK;  /** CSC_PROJECTILE_CREATE **/
    T_word16 objectID            PACK;  /** New object's ID **/
    T_word16 sourceObject        PACK;  /** Object from which it comes **/
    T_word16 objectType          PACK;  /** New object's type **/
    T_byte8 initialSpeed         PACK;  /** New object's initial speed **/
    T_word16 angle               PACK;  /** New object's travel direction **/
                    /** !! Or, if the object is a fball, the TARGET. **/
} T_projectileAddPacket;
#endif
typedef struct {
    T_byte8 command              PACK;  /** CSC_PROJECTILE_CREATE **/
    T_word16 objectID            PACK;  /** New object's ID **/
    T_word16 objectType          PACK;  /** New object's type **/
    T_byte8 initialSpeed         PACK;  /** New object's initial speed **/
    T_word16 angle               PACK;  /** New object's travel direction **/
    T_sword16 x                  PACK;  /* Start location */
    T_sword16 y                  PACK;
    T_sword16 z                  PACK;
    T_sword16 vz                 PACK;  /* Velocity along z. */
    T_word16 target              PACK;  /* Target id if targetting. */
    T_word16 ownerObjID          PACK;  /* ID of owner object. */
} T_projectileAddPacket;

typedef struct {
    T_byte8 command              PACK ;
    T_word16 loginId             PACK ;
    T_word16 objectId            PACK ;
    T_sword16 xPos               PACK ;
    T_sword16 yPos               PACK ;
    T_byte8 angle                PACK ;
} T_placeStartPacket ;

typedef struct {
    T_byte8 command              PACK ;
    T_word32 placeNumber         PACK ;
    T_word16 startLocation       PACK ;
} T_gotoPlacePacket ;

/** This packet now specifies exactly what goto_place it's responding to **/
typedef struct {
    T_byte8 command              PACK ;
    T_word32 placeNumber         PACK;
    T_word16 startLocation       PACK ;
} T_gotoSucceededPacket ;

/** To transmit mail, messages, etc., we need a general file transfer **/
/** protocol.  T_genericDataPacket provides this.  To send a file,    **/
/** break it up into blocks of size GENERIC_DATA_PACKET_CHUNK_SIZE,   **/
/** and transmit them one at a time as T_genericDataPackets.          **/
/** HEADER FIELDS:                                                    **/
/** command    = PACKET_COMMANDTR_DATA_PACKET                         **/
/** size       = bytes in data[] for the last packet, 0 for all others**/
/** position   = 0-based ordinal position of the packet in the file   **/
/** fileID     = unique file ID number (see below)                    **/
/** revlevel   = given on 1st packet only; revlevel of file.          **/

#define GENERIC_DATA_PACKET_CHUNK_SIZE  (LONG_PACKET_LENGTH - \
                                         sizeof(T_genericDataPacketHeader))

typedef struct
{
   T_byte8 command 		PACK;
   T_byte8 size 		PACK;
   T_word16 position 		PACK;
   T_byte8 fileID 		PACK;
   T_word32 revlevel            PACK;
} T_genericDataPacketHeader;

typedef struct 
{
   T_genericDataPacketHeader header              PACK;
   T_byte8 data[GENERIC_DATA_PACKET_CHUNK_SIZE]  PACK;
} T_genericDataPacket;

/** When transmitting a file, the packet which initializes the transfer **/
/** is always the following packet, which declares the file size        **/
/** and a fileID for the transfer.  In addition, it provides the file   **/
/** name, expressed as a 32-bit word.  To find the ASCII filename for   **/
/** the file, use the files.c routines.                                 **/

/**    NOTE: fileID's are "owned" by serial ports; e.g. a file with ID 1**/
/** coming on on ttyS1 is distinct from ID 1 coming in on ttyS2.        **/

typedef struct
{
   T_byte8 command 		PACK;
   T_byte8 fileID 		PACK;
   T_word32 size 		PACK;
   T_word32 name		PACK;
} T_genericDataStartTransferPacket;

/** Send this packet to tell the sender to stop sending packets.   **/
/** Make the command TRANSFER_COMPLETE if okay, or TRANSFER_CANCEL **/
/** to abort.                                                      **/
typedef struct
{
   T_byte8 command              PACK;
   T_byte8 fileID               PACK;
} T_fileTransferCompletePacket;

/** This is the packet which requests a particular filename. **/
typedef struct
{
   T_byte8 command		PACK;
   T_word32 name		PACK;
   T_word32 revlevel            PACK;
} T_requestFileFromServerPacket;


/** And this is the packet which says, "Sorry, we're fresh out of those" **/
typedef struct
{
   T_byte8 command              PACK;
   T_word32 filename            PACK;
} T_fileNotHerePacket;

/** This is the packet which requests that the sender re-transmit a lost **/
/** packet. **/
typedef struct
{
   T_byte8 command		PACK;
   T_byte8 fileID		PACK;
   T_word16 position		PACK;
} T_resendPleasePacket;

typedef struct {
    T_byte8 command       PACK ;
    T_word32 size         PACK ;
    T_word32 pointer      PACK ;
    T_word32 callback     PACK ;
    T_word32 extraData    PACK ;
    T_word32 originalSize PACK ;
    T_word32 originalPointer PACK ;
} T_requestMemoryTransferPacket ;

typedef struct {
    T_byte8 command                PACK ;
    T_byte8 transferID             PACK ;
    T_requestMemoryTransferPacket request PACK ;
} T_readyForMemoryTransferPacket ;

#define TRANSFER_MEMORY_BLOCK_LENGTH   \
             (LONG_PACKET_LENGTH-2*sizeof(T_byte8)-sizeof(T_word32))

typedef struct {
    T_byte8 command               PACK ;
    T_byte8 transferID            PACK ;
    T_word32 position             PACK ;
    T_byte8 data[TRANSFER_MEMORY_BLOCK_LENGTH]   PACK ;
} T_transferMemoryPacket ;     /* Long Packet */
/* --- */

typedef struct {
    T_byte8 command               PACK ;
    T_byte8 bodyLocation          PACK ;
    T_word16 newType              PACK ;
    T_word16 objServerID          PACK ;
    T_word16 player               PACK ;
} T_changeBodyPartPacket ;

typedef struct {
    T_byte8 command               PACK ;
    T_byte8 outsideLightLevel     PACK ;
} T_dayNightPacket ;

/* A ping packet is sent by either client or server */
/* to determine if the opposing client or server is */
/* still there.  All that ever occurs with a ping packet */
/* is that it is ack'ed. */
typedef struct {
    T_byte8 command               PACK ;
} T_pingPacket ;

typedef struct {
    T_byte8 command               PACK ;
    T_word16 wallNum              PACK ;
    T_word16 wallState            PACK ;
} T_wallStateChangePacket ;

typedef struct {
    T_byte8 command               PACK ;
    T_word16 sideNum              PACK ;
    T_word16 sideState            PACK ;
} T_sideStateChangePacket ;

typedef struct {
    T_byte8 command               PACK ;
    T_word16 sectorNum            PACK ;
    T_word16 sectorState          PACK ;
} T_sectorStateChangePacket ;

typedef struct {
    T_byte8 command               PACK ;
    T_word16 groupNum             PACK ;
} T_groupStateChangePacket ;

typedef struct {
    T_byte8 command               PACK ;
    T_sword16 experience          PACK ;
} T_experiencePacket ;

typedef struct {
    T_byte8 command               PACK ;
} T_requestServerIDPacket ;

typedef struct {
    T_byte8 command               PACK ;
    T_word32 serverID             PACK ;
} T_serverIDPacket ;

/* ------------------------------------------------------------------------ */

typedef struct {
    T_byte8 command               PACK ;
} T_requestEnterPacket ;

typedef T_byte8 E_requestEnterStatus ;
#define REQUEST_ENTER_STATUS_WAITING       0
#define REQUEST_ENTER_STATUS_OK            1
#define REQUEST_ENTER_STATUS_CLOSED        2
#define REQUEST_ENTER_STATUS_COMPLETED     3
#define REQUEST_ENTER_STATUS_FULL          4
#define REQUEST_ENTER_STATUS_UNKNOWN       5

typedef struct {
    T_byte8 command               PACK ;
    E_requestEnterStatus status   PACK ;
} T_requestEnterStatusPacket ;

typedef struct {
    T_byte8 command               PACK ;
} T_requestCharacterListPacket ;

/* ------------------------------------------------------------------------ */

typedef struct {
    T_byte8 command               PACK ;
    T_word32 checksum             PACK ;
    T_byte8 slot                  PACK ;
} T_loadCharacterPacket ;

typedef T_byte8 E_loadCharStatus ;
#define LOAD_CHARACTER_STATUS_INCORRECT      0
#define LOAD_CHARACTER_STATUS_CORRECT        1
#define LOAD_CHARACTER_STATUS_UNKNOWN        2

typedef struct {
    T_byte8 command               PACK ;
    E_loadCharStatus status       PACK ;
} T_loadCharacterStatusPacket ;

/* ------------------------------------------------------------------------ */

typedef struct {
    T_byte8 command               PACK ;
    T_byte8 slot                  PACK ;
    T_word32 checksum             PACK ;
    T_byte8 password[MAX_SIZE_PASSWORD] PACK ;
} T_createCharacterPacket ;

typedef T_byte8 E_createCharStatus ;
#define CREATE_CHARACTER_STATUS_OK             0
#define CREATE_CHARACTER_STATUS_ERROR          1
#define CREATE_CHARACTER_STATUS_UNKNOWN        2

typedef struct {
    T_byte8 command               PACK ;
    E_createCharStatus status     PACK ;
} T_createCharStatusPacket ;

/* ------------------------------------------------------------------------ */

typedef struct {
    T_byte8 command               PACK ;
    T_byte8 slot                  PACK ;
} T_deleteCharacterPacket ;

typedef T_byte8 E_deleteCharStatus ;
#define DELETE_CHARACTER_STATUS_OK             0
#define DELETE_CHARACTER_STATUS_ERROR          1
#define DELETE_CHARACTER_STATUS_UNKNOWN        2

typedef struct {
    T_byte8 command               PACK ;
    E_deleteCharStatus status     PACK ;
} T_deleteCharStatusPacket ;

/* ------------------------------------------------------------------------ */
/* The T_checkPasswordPacket and T_checkPasswordStatusPacket are used */
/* together to check a character's password and then to return a status */
/* telling if the password matches the character. */

typedef struct {
    T_byte8 command               PACK ;
    T_byte8 slot                  PACK ;
    T_byte8 password[MAX_SIZE_PASSWORD]  PACK ;
} T_checkPasswordPacket ;

typedef T_byte8 E_checkPasswordStatus ;
#define CHECK_PASSWORD_STATUS_OK             0
#define CHECK_PASSWORD_STATUS_WRONG          1
#define CHECK_PASSWORD_STATUS_UNKNOWN        2

typedef struct {
    T_byte8 command               PACK ;
    E_checkPasswordStatus status  PACK ;
} T_checkPasswordStatusPacket ;

/* ------------------------------------------------------------------------ */
/* The T_changePasswordPacket and T_changePasswordStatusPacket are used */
/* together to change a character's password and then to return a status */
/* telling if the password was successfully changed. */

typedef struct {
    T_byte8 command               PACK ;
    T_byte8 slot                  PACK ;
    T_byte8 password[MAX_SIZE_PASSWORD]  PACK ;
    T_byte8 newPassword[MAX_SIZE_PASSWORD]  PACK ;
} T_changePasswordPacket ;

typedef T_byte8 E_changePasswordStatus ;
#define CHANGE_PASSWORD_STATUS_OK             0
#define CHANGE_PASSWORD_STATUS_WRONG          1
#define CHANGE_PASSWORD_STATUS_ABORT          2
#define CHANGE_PASSWORD_STATUS_UNKNOWN        3

typedef struct {
    T_byte8 command               PACK ;
    E_changePasswordStatus status  PACK ;
} T_changePasswordStatusPacket ;

/* ------------------------------------------------------------------------ */

typedef struct {
    T_byte8 command               PACK ;
    T_word16 dataBlockType        PACK ;
    T_word32 extraData            PACK ;
} T_requestDataBlockPacket ;

/* ------------------------------------------------------------------------ */

typedef struct {
    T_byte8 command               PACK ;
    T_word16 objectID             PACK ;
    T_word16 damage               PACK ;
    T_byte8 damageType            PACK ;
    T_word16 ownerObjID           PACK ;
} T_damageObjectPacket ;

/* ------------------------------------------------------------------------ */

typedef struct {
    T_byte8 command               PACK ;
} T_requestPiecewiseListPacket ;

/* ------------------------------------------------------------------------ */

typedef struct {
    T_byte8 command               PACK ;
    T_word16 parts[MAX_BODY_PARTS] PACK ;
} T_piecewiseListPacket ;

/* ------------------------------------------------------------------------ */

typedef struct {
    T_byte8 command ;
    T_word16 item ;
    T_word16 numItems ;
    T_word16 storeID ;            /* 0 to MAX_STORES (63) */
} T_requestStoreAddItemPacket ;

/* ------------------------------------------------------------------------ */

typedef struct {
    T_byte8 command ;
    T_word16 item ;
    T_word16 numItems ;
    T_word16 storeID ;
} T_requestStoreRemoveItemPacket ;

/* ------------------------------------------------------------------------ */

typedef struct {
    T_byte8 command ;
    T_byte8 removeResult ;
    T_word16 storeID ;
} T_storeRemoveResultPacket ;

/* ------------------------------------------------------------------------ */

typedef struct {
    T_byte8 command ;
    T_byte8 addResult ;
    T_word16 storeID ;
} T_storeAddResultPacket ;

/* ------------------------------------------------------------------------ */

typedef struct {
    T_byte8 command ;
    T_byte8 fromPlayer ;
    T_byte8 toPlayer ;
    T_byte8 transmitStart ;
    T_gameGroupID groupID ;
} T_retransmitPacket ;

/* ------------------------------------------------------------------------ */

/* Block for transmitting a character block (particularly to be stored) */
typedef struct {
    T_word16 slot ;
    T_byte8 password[MAX_SIZE_PASSWORD] ;
    T_word32 size ;
    T_byte8 charData[] ;
} T_characterBlock ;

/* ------------------------------------------------------------------------ */

/* Packet for town ui messages. */
typedef struct {
    T_byte8 command ;
    T_byte8 name[30] ;
    T_byte8 msg[40] ;
} T_townUIMessagePacket ;

/* ------------------------------------------------------------------------ */

typedef T_byte8 T_playerIDLocation ;
#define PLAYER_ID_LOCATION_NOWHERE           0
#define PLAYER_ID_LOCATION_TOWN              1
#define PLAYER_ID_LOCATION_GUILD             2
#define PLAYER_ID_LOCATION_GAME              3
#define PLAYER_ID_LOCATION_UNKNOWN           4

typedef T_byte8 T_playerIDState ;
#define PLAYER_ID_STATE_NONE                 0
#define PLAYER_ID_STATE_CREATING_GAME        1
#define PLAYER_ID_STATE_JOINING_GAME         2
#define PLAYER_ID_STATE_UNKNOWN              3

typedef struct {
    T_byte8 name[30] ;
    T_directTalkUniqueAddress uniqueAddress ;
    T_playerIDLocation location ;
    T_playerIDState state ;
    T_gameGroupID groupID ;
    T_word16 adventure ;
} T_playerIDSelf ;

typedef struct {
    T_byte8 command ;
    T_playerIDSelf id ;
} T_playerIDSelfPacket ;

/* ------------------------------------------------------------------------ */

typedef struct {
    T_byte8 command ;
    T_directTalkUniqueAddress uniqueAddress ;
    T_gameGroupID groupID ;
    T_word16 adventure ;
} T_gameRequestJoinPacket ;

/* ------------------------------------------------------------------------ */

typedef T_byte8 E_respondJoin ;
#define GAME_RESPOND_JOIN_OK         0
#define GAME_RESPOND_JOIN_FULL       1
#define GAME_RESPOND_JOIN_CANCELED   2
#define GAME_RESPOND_JOIN_UNKNOWN    3

typedef struct {
    T_byte8 command ;
    T_directTalkUniqueAddress uniqueAddress ;
    T_gameGroupID groupID ;
    T_word16 adventure ;
    E_respondJoin response ;
} T_gameRespondJoinPacket ;

/* ------------------------------------------------------------------------ */

typedef struct {
    T_byte8 command ;
    T_gameGroupID groupID ;
    T_word16 adventure ;
    T_byte8 numPlayers ;
    T_directTalkUniqueAddress players[4] ;
    T_word32 timeOfDay ;
    T_word16 firstLevel ;
} T_gameStartPacket ;

/* ------------------------------------------------------------------------ */

#endif


/****************************************************************************/
/*    END OF FILE:  PACKET.H                                                */
/****************************************************************************/
