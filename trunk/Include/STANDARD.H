/****************************************************************************/
/*    FILE:  STANDARD.H                                                     */
/****************************************************************************/

#ifndef _STANDARD_H_
#define _STANDARD_H_

#ifndef USE_SOS_LIBRARY
//#define USE_SOS_LIBRARY 1
#endif

/** CONTROL DEFINES **/
#include "options.h"

/** Use to produce a server-only compile. **/
/* #define SERVER_ONLY */

/** Use to generate a UNIX compile. **/
/* #define TARGET_UNIX */

/** We can only generate a server for the UNIX box. **/
#ifdef TARGET_UNIX
# ifndef SERVER_ONLY
#  define SERVER_ONLY
# endif
#endif


/** C library and system call interface includes. **/
#  include <stdio.h>
#  include <stdlib.h>
#  include <string.h>

#ifdef WIN32
/** DOS includes **/
#  include <math.h>
#else
/** DOS includes **/
#  include <mem.h>
#  include <i86.h>
#  include <dos.h>
#  include <conio.h>
#  include <math.h>
#endif

/** General includes from our own custom libraries. **/
#  include "general.h"

#  include "debug.h"
#  include "ticker.h"
#  include "synctime.h"
#  include "memory.h"
#  include "file.h"
#  include "comm.h"
#  include "ditalk.h"

#  include "resource.h"
#  include "colorize.h"
#  include "graphics.h"
#  include "script.h"         /* General scripting utility. */
#  include "scriptev.h"
#  include "dbllink.h"        /* Double link list module. */
#  include "random.h"         /* Random number generator. */

/** Client and player services. **/
#  include "mousemod.h"
#  include "keyboard.h"
#  include "keyscan.h"
#  include "sound.h"
#  include "overhead.h"

/** DOS-specific low-level stuff. **/
#  include "comio.h"

/* 3d engine, object system, mapping, view-drawing, pictures, activities, */
/* message sending, timed events, doors, creatures, file transfer, */
/* and sounds. */
#  include "objmove.h" /* LES */
#  include "objtype.h" /* LES */

#  include "viewfile.h"
#  include "map.h"     /* LES */

#  include "3d_view.h"
#  include "3d_trig.h"
#  include "3d_io.h"
#  include "3d_colli.h"


#  include "packet.h"
#  include "view.h"
#  include "pics.h"
#  include "activity.h"
#  include "message.h"
#  include "schedule.h"
#  include "slider.h"
#  include "cmdqueue.h"
#  include "server.h"
#  include "crelogic.h"
#  include "door.h"
#  include "light.h"
#  include "mapanim.h"
#  include "areasnd.h"
#  include "object.h"

#  include "filetran.h"
#  include "files.h"


#  include "config.h"
#  include "keymap.h"
#  include "color.h" /* JDA */

#  include "graphic.h"
#  include "text.h"
#  include "button.h"
#  include "control.h"
#  include "txtfld.h"
#  include "Txtbox.h"
#  include "slidr.h"
#  include "comwin.h"

#  include "effect.h"
#  include "efx.h"    /* JDA */
#  include "spells.h" /* JDA */
#  include "cli_send.h"
#  include "cli_recv.h"
#  include "equip.h"
#  include "inventor.h"
#  include "player.h"
#  include "form.h"
#  include "banner.h"

#  include "stats.h" /* JDA */
#  include "connUI.h"
#  include "mainui.h"
#  include "mailui.h"
#  include "prompt.h"
#  include "notes.h"
#  include "scrform.h"
#  include "objgen.h"  /* LES */

/** UNIX-specific stuff. **/
#ifdef TARGET_UNIX
#  define strnicmp strncasecmp
#  define stricmp strcasecmp
#endif

#include "memtrans.h"
#include "MapDiff.h"

#include "overlay.h"
#include "update.h"
#include "hash32.h"
#include "packlog.h"
#include "serplog.h"
#include "smachine.h"
#include "flatfile.h"
#include "sounds.h"
#include "serversh.h"
#include "smconnec.h"
#include "smmain.h"
#include "smcchoos.h"
#include "smcplay.h"
#include "smclogof.h"
#include "smcdis.h"
#include "smcleave.h"
#include "inifile.h"
#include "lookup32.h"
#include "account.h"          /** Note: this is for testing before */

#include "chardb.h"
#include "store.h"
#include "bankui.h"
#include "innui.h"
#include "houseui.h"
#include "townui.h"
#include "guildui.h"
#include "hardform.h"
#include "storedb.h"
#include "look.h"
                              /* it is moved to the server. */
#  include "client.h"
#include "compress.h"

#  include "syncpack.h"
#  include "csyncpck.h"
#include "peophere.h"
#  include "syncmem.h"

#endif

/****************************************************************************/
/*    END OF FILE:  STANDARD.H                                              */
/****************************************************************************/
