/****************************************************************************/
/*    FILE:  CONFIG.H                                                       */
/****************************************************************************/

#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "inifile.h"

typedef T_byte8 E_musicType ;
#define MUSIC_TYPE_NONE       0
#define MUSIC_TYPE_STREAM_IO  1
#define MUSIC_TYPE_MIDI       2
#define MUSIC_TYPE_UNKNOWN    3

T_void ConfigLoad(T_void) ;

T_void ConfigOpen(T_void) ;

T_void ConfigClose(T_void) ;

T_dataBlockAccountInfo *ConfigCreateAccountInfoBlock(T_void) ;

T_word32 ConfigGetAccountNumber(T_void) ;

T_iniFile ConfigGetINIFile(T_void) ;

E_musicType ConfigGetMusicType(T_void) ;

T_byte8 ConfigGetCDROMDrive(T_void) ;

E_Boolean ConfigGetBobOffFlag(T_void) ;

#endif

/****************************************************************************/
/*    END OF FILE:  CONFIG.H                                                */
/****************************************************************************/
