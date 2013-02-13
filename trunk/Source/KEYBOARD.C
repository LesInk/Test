/****************************************************************************/
/*    FILE:  UIBUTTON.C                                                     */
/****************************************************************************/

#include "standard.h"

#ifdef WATCOM
#define ALLOW_KEYBOARD_INTERRUPT
#endif

#ifdef ALLOW_KEYBOARD_INTERRUPT
#include <bios.h>

#define KEYBOARD_INTERRUPT_NUMBER 9
#define DISABLE_KEYBOARD 0xAD
#define ENABLE_KEYBOARD 0xAE
#define KEYBOARD_DATA 0x60
#define STATUS_PORT 0x64
#define INPUT_BUFFER_FULL 0x02
#endif

#define KEYBOARD_HIGHEST_SCAN_CODE 255

/* Number of characters that can be held in keyboard buffer. */
/* Must be a power of two. */
#define MAX_SCAN_KEY_BUFFER 128

/* Note if the keyboard is on: */
static E_Boolean F_keyboardOn = FALSE ;

/* Note if we are using the keyboard buffer: */
static E_Boolean F_keyboardBufferOn = TRUE ;

/* Table to keep track of which keys are pressed: */
static E_Boolean G_keyTable[256] ;

/* Also keep a table to track which events for what keys have occured: */
static E_Boolean G_keyEventTable[256] ;

/* Information about how long the key was pressed. */
static T_word32 G_keyHoldTime[256] ;
static T_word32 G_keyPressTime[256] ;

/* Internal buffer of press and release keys: */
static T_word16 G_scanKeyBuffer[MAX_SCAN_KEY_BUFFER] ;
static T_word16 G_scanKeyStart = 0 ;
static T_word16 G_scanKeyEnd = 0 ;

/* Number of keys being pressed simultaneously: */
static T_word16 G_keysPressed = 0 ;

/* Pointer to old BIOS key handler. */
#ifdef ALLOW_KEYBOARD_INTERRUPT
static T_void (__interrupt __far *IOldKeyboardInterrupt)(T_void);
#endif

/* Keep track of the event handler for the keyboard. */
static T_keyboardEventHandler G_keyboardEventHandler ;

/* Flag to tell if KeyboardGetScanCode works */
static E_Boolean G_allowKeyScans = TRUE ;

/* Internal routines: */
#ifdef ALLOW_KEYBOARD_INTERRUPT
static T_void __interrupt __far IKeyboardInterrupt(T_void);
#endif

static T_void IKeyboardClear(T_void) ;

static T_void IKeyboardSendCommand(T_byte8 keyboardCommand) ;

static E_Boolean G_escapeCode = FALSE ;

static T_doubleLinkList G_eventStack = DOUBLE_LINK_LIST_BAD ;

static T_word16 G_pauseLevel = 0 ;

#ifdef WIN32
char G_keyboardToASCII[256] = {
     '\0',  '\0',  '1',   '2',   '3',   '4',   '5',   '6',
     '7',   '8',   '9',   '0',   '-',   '=',   '\b',  '\t',
     'q',   'w',   'e',   'r',   't',   'y',   'u',   'i',
     'o',   'p',   '[',   ']',   '\r',  '\0',  'a',   's',
     'd',   'f',   'g',   'h',   'j',   'k',   'l',   ';',
     '\'',  '`',  '\0',  '\\',  'z',   'x',   'c',   'v',
     'b',   'n',   'm',   ',',   '.',   '/',  '\0',  '\0',
     '\0',  '\040', '\0',  '\0',  '\0',  '\0',  '\0',  '\0',
     '\0',  '\0',  '\0',  '\0',  '\0',  '\0',  '\0',  '\0',
     '\0',  '\0',  '\0',  '\0',  '\0',  '\0',  '\0',  '\0',
     '\0',  '\0',  '\0',  '\0',  '\0',  '\0',  '\0',  '\0',
     '\0',  '\0',  '\0',  '\0',  '\0',  '\0',  '\0',  '\0',
     '\0',  '\0',  '\0',  '\0',  '\0',  '\0',  '\0',  '\0',
     '\0',  '\0',  '\0',  '\0',  '\0',  '\0',  '\0',  '\0',
     '\0',  '\0',  '\0',  '\0',  '\0',  '\0',  '\0',  '\0',
     '\0',  '\0',  '\0',  '\0',  '\0',  '\0',  '\0',  '\0',
     '\0',  '\0',  '!',   '@',   '#',   '$',   '%',  '^',
     '&',   '*',   '(',   ')',   '_',   '+',   '\b',  '\t',
     'Q',   'W',   'E',   'R',   'T',   'Y',   'U',   'I',
     'O',   'P',   '{',   '}',   '\r',  '\0',  'A',   'S',
     'D',   'F',   'G',   'H',   'J',   'K',   'L',   ':',
     '\"',  '~',  '\0',  '|',   'Z',   'X',   'C',   'V',
     'B',   'N',   'M',   '<',   '>',   '?',  '\0',  '\0',
     '\0',  '\040', '\0',  '\0',  '\0',  '\0',  '\0',  '\0',
     '\0',  '\0',  '\0',  '\0',  '\0',  '\0',  '\0',  '\0',
     '\0',  '\0',  '\0',  '\0',  '\0',  '\0',  '\0',  '\0',
     '\0',  '\0',  '\0',  '\0',  '\0',  '\0',  '\0',  '\0',
     '\0',  '\0',  '\0',  '\0',  '\0',  '\0',  '\0',  '\0',
     '\0',  '\0',  '\0',  '\0',  '\0',  '\0',  '\0',  '\0',
     '\0',  '\0',  '\0',  '\0',  '\0',  '\0',  '\0',  '\0',
     '\0',  '\0',  '\0',  '\0',  '\0',  '\0',  '\0',  '\0',
     '\0',  '\0',  '\0',  '\0',  '\0',  '\0',  '\0',  '\0',
} ;
#endif

#ifdef WATCOM
char G_keyboardToASCII[256] = {
     '\0',  '\0',  '1',   '2',   '3',   '4',   '5',   '6',
     '7',   '8',   '9',   '0',   '-',   '=',   '\b',  '\t',
     'q',   'w',   'e',   'r',   't',   'y',   'u',   'i',
     'o',   'p',   '[',   ']',   '\r',  '\0',  'a',   's',
     'd',   'f',   'g',   'h',   'j',   'k',   'l',   ';',
     '\'',  '\`',  '\0',  '\\',  'z',   'x',   'c',   'v',
     'b',   'n',   'm',   ',',   '.',   '\/',  '\0',  '\0',
     '\0',  '\040', '\0',  '\0',  '\0',  '\0',  '\0',  '\0',
     '\0',  '\0',  '\0',  '\0',  '\0',  '\0',  '\0',  '\0',
     '\0',  '\0',  '\0',  '\0',  '\0',  '\0',  '\0',  '\0',
     '\0',  '\0',  '\0',  '\0',  '\0',  '\0',  '\0',  '\0',
     '\0',  '\0',  '\0',  '\0',  '\0',  '\0',  '\0',  '\0',
     '\0',  '\0',  '\0',  '\0',  '\0',  '\0',  '\0',  '\0',
     '\0',  '\0',  '\0',  '\0',  '\0',  '\0',  '\0',  '\0',
     '\0',  '\0',  '\0',  '\0',  '\0',  '\0',  '\0',  '\0',
     '\0',  '\0',  '\0',  '\0',  '\0',  '\0',  '\0',  '\0',
     '\0',  '\0',  '!',   '@',   '#',   '$',   '\%',  '\^',
     '&',   '*',   '(',   ')',   '_',   '+',   '\b',  '\t',
     'Q',   'W',   'E',   'R',   'T',   'Y',   'U',   'I',
     'O',   'P',   '{',   '}',   '\r',  '\0',  'A',   'S',
     'D',   'F',   'G',   'H',   'J',   'K',   'L',   ':',
     '\"',  '\~',  '\0',  '|',   'Z',   'X',   'C',   'V',
     'B',   'N',   'M',   '<',   '>',   '?',  '\0',  '\0',
     '\0',  '\040', '\0',  '\0',  '\0',  '\0',  '\0',  '\0',
     '\0',  '\0',  '\0',  '\0',  '\0',  '\0',  '\0',  '\0',
     '\0',  '\0',  '\0',  '\0',  '\0',  '\0',  '\0',  '\0',
     '\0',  '\0',  '\0',  '\0',  '\0',  '\0',  '\0',  '\0',
     '\0',  '\0',  '\0',  '\0',  '\0',  '\0',  '\0',  '\0',
     '\0',  '\0',  '\0',  '\0',  '\0',  '\0',  '\0',  '\0',
     '\0',  '\0',  '\0',  '\0',  '\0',  '\0',  '\0',  '\0',
     '\0',  '\0',  '\0',  '\0',  '\0',  '\0',  '\0',  '\0',
     '\0',  '\0',  '\0',  '\0',  '\0',  '\0',  '\0',  '\0',
} ;
#endif


static char G_asciiBuffer[32] ;
static T_word16 G_asciiStart = 0 ;
static T_word16 G_asciiEnd = 0 ;

/****************************************************************************/
/*  Routine:  KeyboardOn                                                    */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    KeyboardOn installs the keyboard functions of the following commands. */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    This routine can not be called twice in a row and must be called      */
/*  once for the rest of the routines to work correctly (Except the         */
/*  keyboard buffer commands).                                              */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Calls:                                                                  */
/*                                                                          */
/*    _disable                                                              */
/*    _enable                                                               */
/*    _dos_getvect                                                          */
/*    _dos_setvect                                                          */
/*    IKeyboardClear                                                        */
/*    DoubleLinkListCreate                                                  */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  11/22/94  Created                                                */
/*    LES  02/28/96  Added construction of G_eventStack                     */
/*                                                                          */
/****************************************************************************/

T_void KeyboardOn(T_void)
{
    DebugRoutine("KeyboardOn") ;
    DebugCheck(F_keyboardOn == FALSE) ;

    /* Note that the keyboard is now on. */
    F_keyboardOn = TRUE ;

#ifdef ALLOW_KEYBOARD_INTERRUPT
    /* We are doing somewhat sensitive stuff, so turn off the interrupts. */
    _disable() ;

    /* First get the old keyboard interrupt we used to use. */
    IOldKeyboardInterrupt = _dos_getvect(KEYBOARD_INTERRUPT_NUMBER);

    /* Now declare our new interrupt to the timer. */
    _dos_setvect(KEYBOARD_INTERRUPT_NUMBER, IKeyboardInterrupt);

    /* Clear the keyboard and event keyboard before we let things go. */
    IKeyboardClear() ;

    /* Done twiddling with the hardware, turn back on the interrupts. */
    _enable() ;
#else
    /* Clear the keyboard and event keyboard before we let things go. */
    IKeyboardClear() ;
#endif
    G_eventStack = DoubleLinkListCreate() ;
    DebugCheck(G_eventStack != DOUBLE_LINK_LIST_BAD) ;

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  KeyboardOff                                                   */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    When you are done with the keyboard, you can turn it off by calling   */
/*  KeyboardOff.  This will remove the keyboard driver from memory.         */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    This routine MUST be called before exiting or DOS will crash.         */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Calls:                                                                  */
/*                                                                          */
/*    DoubleLinkListDestroy                                                 */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  11/22/94  Created                                                */
/*    LES  02/28/96  Added deconstruction of G_eventStack                   */
/*                                                                          */
/****************************************************************************/

T_void KeyboardOff(T_void)
{
    DebugRoutine("KeyboardOff") ;
    DebugCheck(F_keyboardOn == TRUE) ;

    DebugCheck(G_eventStack != DOUBLE_LINK_LIST_BAD) ;
    DoubleLinkListDestroy(G_eventStack) ;

#ifdef ALLOW_KEYBOARD_INTERRUPT
    /* We are doing somewhat sensitive stuff, so turn off the interrupts. */
    _disable() ;

    /* Replace the new driver with the old driver. */
    _dos_setvect(KEYBOARD_INTERRUPT_NUMBER, IOldKeyboardInterrupt);

    /* Done twiddling with the hardware, turn back on the interrupts. */
    _enable() ;
#endif

    /* Note that the keyboard is now off. */
    F_keyboardOn = FALSE ;

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  KeyboardGetScanCode                                           */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    KeyboardGetScanCode is used to check if the key for the corresponding */
/*  scan code is being pressed.  If it is, a TRUE is returned, or else      */
/*  false is returned.                                                      */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_byte8 scanCode            -- Scan code to check                     */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    E_Boolean                   -- TRUE  = Key is pressed                 */
/*                                   FALSE = Key is not pressed             */
/*                                                                          */
/*                                                                          */
/*  Calls:                                                                  */
/*                                                                          */
/*    Nothing.                                                              */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  11/22/94  Created                                                */
/*    LES  06/06/94  Modified to handle dual key requests                   */
/*                                                                          */
/****************************************************************************/

static E_Boolean IGetAdjustedKey(T_word16 scanCode)
{
    switch (scanCode)
    {
        case 0x38 /** Left alt key **/ :
        case 0xB8 /** Right alt key **/ :
           return G_keyTable[0x38] |
                  G_keyTable[0xB8] ;
           break;

        case KEY_SCAN_CODE_RIGHT_SHIFT:
        case KEY_SCAN_CODE_LEFT_SHIFT:
           return G_keyTable[KEY_SCAN_CODE_RIGHT_SHIFT] |
                  G_keyTable[KEY_SCAN_CODE_LEFT_SHIFT] ;
           break;

        case 0x9D /** Right control key **/ :
        case 0x1D /** Left control key **/ :
           return G_keyTable[0x9D] |
                  G_keyTable[0x1D] ;
           break;

        default:
           return G_keyTable[scanCode] ;
           break;
    }
}

E_Boolean KeyboardGetScanCode(T_word16 scanCodes)
{
    E_Boolean isPressed ;
    T_byte8 scanCode, scanCode2 ;

    DebugRoutine("KeyboardGetScanCode") ;
    DebugCheck(F_keyboardOn == TRUE) ;

    /* Check to see if key scans are allowed */
    /* (ESC and ALT and ENTER key break this rule) */
    /* Also function keys */
    if ((G_allowKeyScans == FALSE)
        && (IGetAdjustedKey(KEY_SCAN_CODE_ESC) == FALSE)
        && (IGetAdjustedKey(KEY_SCAN_CODE_ENTER) == FALSE)
        && (IGetAdjustedKey(KEY_SCAN_CODE_ALT) == FALSE)
        && (IGetAdjustedKey(KEY_SCAN_CODE_F1) == FALSE)
        && (IGetAdjustedKey(KEY_SCAN_CODE_F2) == FALSE)
        && (IGetAdjustedKey(KEY_SCAN_CODE_F3) == FALSE)
        && (IGetAdjustedKey(KEY_SCAN_CODE_F4) == FALSE)
        && (IGetAdjustedKey(KEY_SCAN_CODE_F5) == FALSE)
        && (IGetAdjustedKey(KEY_SCAN_CODE_F6) == FALSE)
        && (IGetAdjustedKey(KEY_SCAN_CODE_F7) == FALSE)
        && (IGetAdjustedKey(KEY_SCAN_CODE_F8) == FALSE)
        && (IGetAdjustedKey(KEY_SCAN_CODE_F9) == FALSE)
        && (IGetAdjustedKey(KEY_SCAN_CODE_F10) == FALSE)
        && (IGetAdjustedKey(KEY_SCAN_CODE_F11) == FALSE)
        && (IGetAdjustedKey(KEY_SCAN_CODE_F12) == FALSE))  {
        /* Pretend the key is not pressed if in this mode. */
        isPressed = FALSE ;
    } else {
        scanCode = scanCodes & 0xFF ;
        scanCode2 = (scanCodes >> 8) ;

        DebugCheck(scanCode <= KEYBOARD_HIGHEST_SCAN_CODE) ;
        DebugCheck(scanCode2 <= KEYBOARD_HIGHEST_SCAN_CODE) ;

        if (scanCode2 == 0)
            isPressed = IGetAdjustedKey(scanCode) ;
        else
            isPressed = IGetAdjustedKey(scanCode) |
                        IGetAdjustedKey(scanCode) ;
    }

    DebugCheck(isPressed < BOOLEAN_UNKNOWN) ;
    DebugEnd() ;

    return isPressed ;
}

/****************************************************************************/
/*  Routine:  KeyboardDebounce                                              */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    Sometimes it is useful to wait for all keys to be released.           */
/*  KeyboardDebounce is a simple routine that just waits until the user     */
/*  has released all pressed keys.                                          */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Calls:                                                                  */
/*                                                                          */
/*    Nothing.                                                              */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  11/22/94  Created                                                */
/*                                                                          */
/****************************************************************************/

T_void KeyboardDebounce(T_void)
{
    T_word32 time ;

    DebugRoutine("KeyboardDebounce") ;
    DebugCheck(F_keyboardOn == TRUE) ;

#if 0
    time = TickerGet() ;

//puts("Waiting") ;
    while (G_keysPressed != 0)
        { /* wait */
            if ((TickerGet() - time) > 210)
                break ;
        }
//puts("Done") ;

    /* Clear out the keyboard stats. */
//    IKeyboardClear() ;
#endif

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  KeyboardSetEventHandler                                       */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    KeyboardSetEventHandler declares the function to be called for        */
/*  each keyboard event as dispatched by the KeyboardUpdateEvents routine.  */
/*  If you no longer need a keyboard handler, give this routine an          */
/*  input parameter of NULL.                                                */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_keyboardEventHandler keyboardEventHandler                           */
/*                                -- function to call on events, or NULL    */
/*                                   for none.                              */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Calls:                                                                  */
/*                                                                          */
/*    Nothing.                                                              */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  11/22/94  Created                                                */
/*                                                                          */
/****************************************************************************/

T_void KeyboardSetEventHandler(T_keyboardEventHandler keyboardEventHandler)
{
    DebugRoutine("KeyboardSetEventHandler") ;
    /* Nothing to do a DebugCheck on ... */

    /* Record the event handler for future use. */
    G_keyboardEventHandler = keyboardEventHandler ;

    DebugEnd() ;
}


T_keyboardEventHandler KeyboardGetEventHandler (T_void)
{
    return (G_keyboardEventHandler);
}



/****************************************************************************/
/*  Routine:  KeyboardUpdateEvents                                          */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    KeyboardUpdateEvents should be called often for it to work correctly  */
/*  with a keyboard event handler.  Events are sent by this routine to      */
/*  tell what keys are being pressed and released.                          */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Calls:                                                                  */
/*                                                                          */
/*    Whatever is the current G_keyboardEventHandler (if it exists)         */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  11/22/94  Created                                                */
/*    LES  12/12/94  Added buffer mode checking to buffered keystroke       */
/*                   events.                                                */
/*                                                                          */
/****************************************************************************/

T_void KeyboardUpdateEvents(T_void)
{
    T_word16 scanCode ;

    DebugRoutine("KeyboardUpdateEvents") ;
    DebugCheck(F_keyboardOn == TRUE) ;

    while (G_scanKeyStart != G_scanKeyEnd)  {
        scanCode = G_scanKeyBuffer[G_scanKeyStart] ;
        /* Is this a press or a release? */
        if (scanCode & 0x100)  {
            /* THis is a release. */
            G_keyboardEventHandler(KEYBOARD_EVENT_RELEASE, scanCode & 0xFF) ;
        } else {
            G_keyboardEventHandler(KEYBOARD_EVENT_PRESS, scanCode) ;
        }

        /* Special case for the right ctrl key -- we want it to act */
        /* just like the left control key. */
        if ((scanCode & 0xFF) == KEY_SCAN_CODE_RIGHT_CTRL)  {
            if (scanCode & 0x100)  {
                /* THis is a release. */
                G_keyboardEventHandler(KEYBOARD_EVENT_RELEASE, KEY_SCAN_CODE_CTRL) ;
            } else {
                G_keyboardEventHandler(KEYBOARD_EVENT_PRESS, KEY_SCAN_CODE_CTRL) ;
            }
        }
        _disable();
        G_scanKeyStart = (G_scanKeyStart+1) & (MAX_SCAN_KEY_BUFFER-1) ;
        _enable();
    }

    /* Now we will look for all keys that are being held and send */
    /* events appropriately. */
    for (scanCode = 0; scanCode <= KEYBOARD_HIGHEST_SCAN_CODE; scanCode++)
        /* Check to see if key is being held. */
        if (G_keyTable[scanCode] == TRUE)
            /* We need to send a event. */
            G_keyboardEventHandler(KEYBOARD_EVENT_HELD, scanCode) ;

    /* Add an event for the right ctrl to be the left ctrl. */
    if (G_keyTable[KEY_SCAN_CODE_RIGHT_CTRL] == TRUE)
        G_keyboardEventHandler(KEYBOARD_EVENT_HELD, KEY_SCAN_CODE_CTRL) ;

    /* Check to see if we are in buffered mode. */
    if (F_keyboardBufferOn == TRUE)  {
        /* If we are, loop while there are characters in the buffer. */
        while (KeyboardBufferReady())  {
            /* Send a special buffered event for each */
            /* of these characters. */
            G_keyboardEventHandler(
                KEYBOARD_EVENT_BUFFERED,
                KeyboardBufferGet()) ;
        }
    }

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  KeyboardBufferOn                                              */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    KeyboardBufferOn will allow keys to be buffered up for use later.     */
/*  Although useful in word processors, when the buffer gets full it        */
/*  causes a beeping noise that is not wonderful in games.  Depending on    */
/*  your need, you may wnat the buffer on.                                  */
/*    Note: KeyboardBufferOn CAN be called even if KeyboardOn is not.       */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Calls:                                                                  */
/*                                                                          */
/*    Nothing.                                                              */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  11/22/94  Created                                                */
/*                                                                          */
/****************************************************************************/

T_void KeyboardBufferOn(T_void)
{
    DebugRoutine("KeyboardBufferOn") ;

    F_keyboardBufferOn = TRUE ;

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  KeyboardBufferOff                                             */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    KeyboardBufferOff will not allow keys to be buffered up for use later.*/
/*  Although useful in word processors, when the buffer gets full it        */
/*  causes a beeping noise that is not wonderful in games.  Depending on    */
/*  your need, you may wnat the buffer off.                                 */
/*    Note: KeyboardBufferOff CAN be called even if KeyboardOn is not.      */
/*    The default is for the buffer to be on.                               */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Calls:                                                                  */
/*                                                                          */
/*    Nothing.                                                              */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  11/22/94  Created                                                */
/*                                                                          */
/****************************************************************************/

T_void KeyboardBufferOff(T_void)
{
    DebugRoutine("KeyboardBufferOff") ;

//    F_keyboardBufferOn = FALSE ;

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  KeyboardBufferGet                                             */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    If you are using a buffer, you can get keystrokes from the buffer     */
/*  by using this routine.                                                  */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    T_byte8                     -- ASCII character of gained character    */
/*                                   or 0 for none.                         */
/*                                                                          */
/*                                                                          */
/*  Calls:                                                                  */
/*                                                                          */
/*    getch                                                                 */
/*    kbhit                                                                 */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  11/22/94  Created                                                */
/*                                                                          */
/****************************************************************************/

T_byte8 KeyboardBufferGet(T_void)
{
    T_byte8 key ;
    T_word16 next ;

    DebugRoutine("KeyboardBufferGet") ;

#if 0
    if (_bios_keybrd(1))  {
        key = _bios_keybrd(0) ;
//        if (key == 0)
//            getch() ;
    } else
        key = 0 ;
#endif

    /* Are there any keys waiting? */
    if (G_asciiStart != G_asciiEnd)  {
        /* Find where the next position will be */
        next = (G_asciiStart+1)&31 ;

        /* Get this character */
        key = G_asciiBuffer[G_asciiStart] ;

        /* Advance in the queue. */
        G_asciiStart = next ;
    } else {
        /* No keys waiting. */
        key = 0 ;
    }

    DebugEnd() ;

    return key ;
}

/****************************************************************************/
/*  Routine:  IKeyboardInterrupt                 * INTERNAL *               */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    IKeyboardInterrupt is the routine that is called every time a key     */
/*  on the keyboard is either pressed or released.  If keyboard buffering   */
/*  is on, the regular BIOS routines are also called.                       */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Calls:                                                                  */
/*                                                                          */
/*    Nothing.                                                              */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  11/22/94  Created                                                */
/*    LES  01/05/95  Added CTRL-F12 break out function in debug only.       */
/*    LES  07/05/95  Added CTRL-F11 to break out using the debug stop       */
/*                   routine.                                               */
/*    AMT  07/23/95  Made right and left equivalent for CTRL, ALT, & SHIFT  */
/*                                                                          */
/****************************************************************************/
#ifdef ALLOW_KEYBOARD_INTERRUPT
static T_void __interrupt __far IKeyboardInterrupt(T_void)
{
    T_byte8 portStatus ;
    T_byte8 scanCode ;
    E_Boolean newStatus ;
    T_word16 next ;
    T_word16 c ;

    INDICATOR_LIGHT(8, INDICATOR_GREEN) ;
    /* We first must disable any new key strokes from occuring. */
    IKeyboardSendCommand(DISABLE_KEYBOARD) ;

    /* Turn off the standard interrupts. */
    _disable() ;

    /* Wait for the keyboard to prepare itself. */
	do {
		portStatus = inp(STATUS_PORT) ;
	} while ((portStatus & INPUT_BUFFER_FULL)==INPUT_BUFFER_FULL) ;

    /* Now that we are ready, let me get the scanCode. */
	scanCode = inp(KEYBOARD_DATA) ;

    /* Turn back on the interrupts. */
	_enable() ;

    if (scanCode == 0xE0)  {
        G_escapeCode = TRUE ;
    } else if (scanCode == 0xE1)  {
        /* Pause key was hit */
        G_pauseLevel++ ;
    } else {
        /* Store either a true or false based on if the key is pressed. */
        newStatus = (scanCode > 127) ? FALSE : TRUE ;

        /* Find reference into tables. */
        scanCode &= 127 ;

        if (G_escapeCode == TRUE)  {
            scanCode |= 128 ;
            G_escapeCode = FALSE ;
        }

        /* Check to see if there is a pause level */
        if ((scanCode == 0x45) || (scanCode == 0xC5))  {
            if (G_pauseLevel == 1)  {
                scanCode = KEY_SCAN_CODE_PAUSE ;
                newStatus = TRUE ;
            } else if (G_pauseLevel == 2)  {
                scanCode = KEY_SCAN_CODE_PAUSE ;
                newStatus = FALSE ;
                G_pauseLevel = 3 ;
            }
        }

        /* Store the key in the scan key buffer. */
        G_scanKeyBuffer[G_scanKeyEnd] = scanCode | ((newStatus)?0:0x100) ;
        next = (G_scanKeyEnd+1) & (MAX_SCAN_KEY_BUFFER-1) ;
        if (next != G_scanKeyStart)
            G_scanKeyEnd = next ;

        /* If a release and an ascii character, store that in the */
        /* keyboard buffer. */
        if ((newStatus) &&
            (G_keyTable[KEY_SCAN_CODE_ALT]==FALSE) &&
            (G_keyTable[KEY_SCAN_CODE_RIGHT_CTRL]==FALSE) &&
            (G_keyTable[KEY_SCAN_CODE_LEFT_CTRL]==FALSE))  {
            c = scanCode & 0xFF;

            /* Translate for the shift key */
            if (G_keyTable[KEY_SCAN_CODE_RIGHT_SHIFT] |
                  G_keyTable[KEY_SCAN_CODE_LEFT_SHIFT])
                c |= 0x80 ;

            /* What ascii character is this? */
            c = G_keyboardToASCII[c] ;

            /* Don't do if a null character */
//            if (c != 0)  {
                /* Where are we going to roll over to next */
                next = (G_asciiEnd+1)&31 ;

                /* If back at start, don't do */
                if (next != G_asciiStart)  {
                    /* Store in the buffer */
                    G_asciiBuffer[G_asciiEnd] = c ;
                    G_asciiEnd = next ;
                }
//            }
        }

        /* Check to see if keystroke changed from pressed to released or */
        /* visa-versa.  If it has, update the key count appropriately. */
        if ((G_keyTable[scanCode] == FALSE) && (newStatus == TRUE))  {
            G_keysPressed++ ;
            /* Note the time that key is pressed. */
            G_keyPressTime[scanCode] = TickerGet() ;
        } else if ((G_keyTable[scanCode] == TRUE) && (newStatus == FALSE))  {
            G_keysPressed-- ;
            /* Note how long the key has been held down. */
            G_keyHoldTime[scanCode] += TickerGet() - G_keyPressTime[scanCode] ;
        }

#if 0
        /* In any case, store the new state. */
        /** Make the right and left side codes for ALT, CTRL, and SHIFT **/
        /** the same. **/
        switch (scanCode)
        {
            case 0x38 /** Left alt key **/ :
            case 0xB8 /** Right alt key **/ :
               G_keyTable[KEY_SCAN_CODE_ALT] = newStatus;
               break;

            case KEY_SCAN_CODE_RIGHT_SHIFT:
            case KEY_SCAN_CODE_LEFT_SHIFT:
               G_keyTable[KEY_SCAN_CODE_RIGHT_SHIFT] =
                  G_keyTable[KEY_SCAN_CODE_LEFT_SHIFT] = newStatus;
               break;

            case 0x9D /** Right control key **/ :
            case 0x1D /** Left control key **/ :
               G_keyTable[KEY_SCAN_CODE_CTRL] = newStatus;
               break;

            default:
               G_keyTable[scanCode] = newStatus ;
               break;
        }
#endif
        G_keyTable[scanCode] = newStatus ;

    }

    /* Turn the keyboard back on. */
    IKeyboardSendCommand(ENABLE_KEYBOARD) ;

    /* If keyboard buffer is on, go ahead and let the BIOS also */
    /* process this keystroke. */
#if 0
    if ((F_keyboardBufferOn) &&
        (G_pauseLevel == 0) &&
        (G_keyTable[KEY_SCAN_CODE_LEFT_CTRL] == FALSE) &&
        (G_keyTable[KEY_SCAN_CODE_PAUSE] == FALSE) &&
        (G_keyTable[KEY_SCAN_CODE_RIGHT_CTRL] == FALSE))  {
        /* Call old interrupt. */
        INDICATOR_LIGHT(8, INDICATOR_YELLOW) ;
//!!!Never call the old interrupt.  Don't want it! ///
//        IOldKeyboardInterrupt() ;
        INDICATOR_LIGHT(8, INDICATOR_BLUE) ;
    } else  {
#endif
        /* Otherwise, signify end of interrupt */
	    outp(0x20,0x20) ;
#if 0
    }
#endif

    if (G_pauseLevel == 3)
        G_pauseLevel = 0 ;


    /* If we are doing a debug compile, we want to have the capability */
    /* to press F12 and turn on the keyboard buffer. */
    /* This allows CTRL-ALT-DEL and CTRL-C to work correctly. */
#ifndef NDEBUG
    if (G_keyTable[KEY_SCAN_CODE_F12] == TRUE)
        F_keyboardBufferOn = TRUE ;

    /* As well, we also want to be able to break out of the program */
    /* when we hit CTRL-F12.  This should keep the system from */
    /* having to reboot. */
    if ((G_keyTable[KEY_SCAN_CODE_F12] == TRUE) &&
        (G_keyTable[KEY_SCAN_CODE_CTRL]==TRUE))  {
        DebugRoutine("IKeyboardInterrupt -- Keyboard break") ;
//        DebugCheck(FALSE) ;
        exit(1) ;
    }
    if ((G_keyTable[KEY_SCAN_CODE_F11] == TRUE) &&
        (G_keyTable[KEY_SCAN_CODE_CTRL]==TRUE))  {
        DebugStop() ;
    }
#endif
    INDICATOR_LIGHT(8, INDICATOR_RED) ;
}
#endif

/****************************************************************************/
/*  Routine:  IKeyboardSendCommand               * INTERNAL *               */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    IKeyboardSendCommand is used by IKeyboardInterrupt to tell the        */
/*  keyboard that certain events are taking place (most likely turning      */
/*  on and off the keyboard interrupts).                                    */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_byte8 *keyboardCommand    -- command to send keyboard               */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Calls:                                                                  */
/*                                                                          */
/*    Nothing.                                                              */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  11/22/94  Created                                                */
/*                                                                          */
/****************************************************************************/
#ifdef ALLOW_KEYBOARD_INTERRUPT
static T_void IKeyboardSendCommand(T_byte8 keyboardCommand)
{
    T_byte8 statusPort ;

    /* cannot easily debug this routine since part of interrupt. */

    /* Don't allow any interrupts. */
	_disable() ;

    /* Wait for the status port to say we are ready. */
	do {
		statusPort = inp(STATUS_PORT) ;
	} while ((statusPort & INPUT_BUFFER_FULL) == INPUT_BUFFER_FULL) ;

    /* Send the command. */
	outp(STATUS_PORT, keyboardCommand) ;

    /* Turn back on interrupts. */
	_enable() ;
}
#endif

/****************************************************************************/
/*  Routine:  IKeyboardClear                     * INTERNAL *               */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    IKeyboardClear is a routine that is called to clear the whole         */
/*  key tables of their state and assume that all keys are NOT being        */
/*  pressed (even if they are).                                             */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    If this routine is called and a key IS being pressed, it will be      */
/*  ignored until the key is released and then pressed again.  In other     */
/*  words, you should only call this routine on startup.                    */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Calls:                                                                  */
/*                                                                          */
/*    Nothing.                                                              */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  11/22/94  Created                                                */
/*                                                                          */
/****************************************************************************/

static T_void IKeyboardClear(T_void)
{
    T_word16 scanCode ;

    DebugRoutine("IKeyboardClear") ;

    _disable();

    /* Clear all scan codes stored in the key table and key event table. */
    for (scanCode = 0; scanCode <= KEYBOARD_HIGHEST_SCAN_CODE; scanCode++)  {
        G_keyTable[scanCode] = FALSE ;
        G_keyEventTable[scanCode] = FALSE ;
        G_keyHoldTime[scanCode] = 0 ;
        G_keyPressTime[scanCode] = 0 ;
    }

    memset(G_scanKeyBuffer, 0, sizeof(G_scanKeyBuffer)) ;
    G_scanKeyStart = G_scanKeyEnd = 0 ;

    /* Note that none of the keys are pressed. */
    G_keysPressed = 0 ;

    G_asciiStart = G_asciiEnd = 0 ;

    _enable();

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  KeyboardGetHeldTimeAndClear        * INTERNAL *               */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    KeyboardGetHeldTimeAndClear tells how long a key has been held        */
/*  down since the key was last cleared.  This capability is useful         */
/*  for getting accurate readings of how long the user desired the key      */
/*  to be pressed, and not based on when calls were able to be made.        */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    Calling this routine can be a problem if it has been a long time      */
/*  since it was previously called on a key.  If you are unsure, be sure    */
/*  to make a call to KeyboardDebounce which clears the timing values.      */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Calls:                                                                  */
/*                                                                          */
/*    Nothing.                                                              */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  09/05/95  Created                                                */
/*                                                                          */
/****************************************************************************/

T_word32 KeyboardGetHeldTimeAndClear(T_word16 scanCode)
{
    T_word32 timeHeld ;
    T_word32 time ;

    DebugRoutine("KeyboardGetHeldTimeAndClear") ;

    /* No interrupts, please. */
    _disable() ;

    /* Is the key being currently pressed? */
    if (G_keyTable[scanCode])  {
        /* It is being pressed. */
        /* Return the time held in the past plus the time currently held */
        time = TickerGet() ;
        timeHeld = G_keyHoldTime[scanCode] + (time-G_keyPressTime[scanCode]) ;
        G_keyPressTime[scanCode] = time ;
    } else {
        /* It is not being pressed. */
        /* Return the time held in the register. */
        timeHeld = G_keyHoldTime[scanCode] ;
    }

    /* Always clear the time held. */
    G_keyHoldTime[scanCode] = 0 ;

    /* Allow interrupts again. */
    _enable() ;

    /* If no key scans are allowed, pretend the key was not hit. */
    if (G_allowKeyScans == FALSE)
        timeHeld = 0 ;

    DebugEnd() ;

    return timeHeld ;
}

/****************************************************************************/
/*  Routine:  KeyboardPushEventHandler                                      */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    KeyboardPushEventHandler removes the old handler by placing it on     */
/*  a stack and sets up the new event handler.  The old event handler       */
/*  will be restored when a call to KeyboardPopEventHandler is called.      */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_keyboardEventHandler keyboardEventHandler                           */
/*                                -- function to call on events, or NULL    */
/*                                   for none.                              */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Calls:                                                                  */
/*                                                                          */
/*    DoubleLinkListAddElementAtFront                                       */
/*    KeyboardSetEventHandler                                               */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  02/28/96  Created                                                */
/*                                                                          */
/****************************************************************************/

T_void KeyboardPushEventHandler(T_keyboardEventHandler keyboardEventHandler)
{
    DebugRoutine("KeyboardPushEventHandler") ;

    /* Store the old event handler. */
    DoubleLinkListAddElementAtFront(
        G_eventStack,
        (T_void *)KeyboardGetEventHandler) ;

    /* set up the new handler. */
    KeyboardSetEventHandler(keyboardEventHandler) ;

    DebugEnd() ;
}


/****************************************************************************/
/*  Routine:  KeyboardPopEventHandler                                       */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    KeyboardPopEventHandler restores the last handler on the stack        */
/*  (if there is one).                                                      */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Calls:                                                                  */
/*                                                                          */
/*    DoubleLinkListGetFirst                                                */
/*    DoubleLinkListElementGetData                                          */
/*    KeyboardSetEventHandler                                               */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  02/28/96  Created                                                */
/*                                                                          */
/****************************************************************************/

T_void KeyboardPopEventHandler(T_void)
{
    T_doubleLinkListElement first ;
    T_keyboardEventHandler handler ;

    DebugRoutine("KeyboardPopEventHandler") ;

    /* Get the old event handler. */
    first = DoubleLinkListGetFirst(G_eventStack) ;
    DebugCheck(first != DOUBLE_LINK_LIST_ELEMENT_BAD) ;

    if (first != DOUBLE_LINK_LIST_ELEMENT_BAD)  {
        handler = (T_keyboardEventHandler)
                      DoubleLinkListElementGetData(first) ;
        KeyboardSetEventHandler(handler) ;
        DoubleLinkListRemoveElement(first) ;
    }

    DebugEnd() ;
}

T_void KeyboardDisallowKeyScans(T_void)
{
    G_allowKeyScans = FALSE ;
}

T_void KeyboardAllowKeyScans(T_void)
{
    G_allowKeyScans = TRUE ;
}

E_Boolean KeyboardBufferReady(T_void)
{
    if (G_asciiStart != G_asciiEnd)
        return TRUE ;
    else
        return FALSE ;
}



#ifdef WIN32
#include <direct.h>
#define KEY_IS_DOWN 0x80
#define KEY_IS_CHANGED 0x01
static T_byte8 G_lastKeyState[256] ;
T_void KeyboardUpdate(E_Boolean updateBuffers)
{
    //T_byte8 keys[256] ;
    T_byte8 *keys;
    T_word32 time ;
    T_word16 scanCode ;
    T_byte8 c ;
    T_word16 next ;
    T_word16 i ;
    E_Boolean changed ;
    E_Boolean newValue ;

    time = TickerGet() ;
    //GetKeyboardState(keys) ;
    keys = SDL_GetKeyState(NULL);

    /* Only care about up/down status */
    for (scanCode=0; scanCode<256; scanCode++)
        keys[scanCode] &= KEY_IS_DOWN ;

    if (updateBuffers)  {
        for (i=1; i<256; i++)  {
            changed = (keys[i] != G_lastKeyState[i])?TRUE:FALSE ;
            //scanCode = MapVirtualKey(i, 0) ;
            scanCode = i;

            /* Record the state of the keypress */
            newValue = (keys[i])?TRUE:FALSE ;
            changed = (newValue != G_keyTable[scanCode])?TRUE:FALSE ;

            /* Find keys that have changed */
            if (changed)  {
                G_keyTable[scanCode] = newValue ;
                /* Store the key in the scan key buffer */
                G_scanKeyBuffer[G_scanKeyEnd] = scanCode ; /* | ((G_keyTable[scanCode]==FALSE)?0:0x100) ; */
                next = (G_scanKeyEnd+1) & (MAX_SCAN_KEY_BUFFER-1) ;
                if (next != G_scanKeyStart)
                    G_scanKeyEnd = next ;

                /* If a release and an ascii character, store that in the */
                /* keyboard buffer. */
                if ((G_keyTable[scanCode] == FALSE) && 
                    (IGetAdjustedKey(KEY_SCAN_CODE_ALT) == FALSE) &&
                    (G_keyTable[KEY_SCAN_CODE_RIGHT_CTRL]==FALSE) &&
                    (G_keyTable[KEY_SCAN_CODE_LEFT_CTRL]==FALSE))  {
                    c = scanCode & 0xFF;

                    /* Translate for the shift key */
                    if (G_keyTable[KEY_SCAN_CODE_RIGHT_SHIFT] |
                          G_keyTable[KEY_SCAN_CODE_LEFT_SHIFT])
                        c |= 0x80 ;

                    /* What ascii character is this? */
                    c = G_keyboardToASCII[c] ;

                    /* Where are we going to roll over to next */
                    next = (G_asciiEnd+1)&31 ;

                    /* If back at start, don't do */
                    if (next != G_asciiStart)  {
                        /* Store in the buffer */
                        G_asciiBuffer[G_asciiEnd] = c ;
                        G_asciiEnd = next ;
                    }
                }

                /* Check to see if keystroke changed from pressed to released or */
                /* visa-versa.  If it has, update the key count appropriately. */
                if (G_keyTable[scanCode] == TRUE)  {
                    G_keysPressed++ ;
                    /* Note the time that key is pressed. */
                    G_keyPressTime[scanCode] = time ;
                } else if (G_keyTable[scanCode] == FALSE)  {
                    G_keysPressed-- ;
                    /* Note how long the key has been held down. */
                    G_keyHoldTime[scanCode] += time - G_keyPressTime[scanCode] ;
                }
            }
        }
    }
    memcpy(G_lastKeyState, keys, sizeof(G_lastKeyState)) ;
}
#endif


/****************************************************************************/
/*    END OF FILE:  UIBUTTON.C                                              */
/****************************************************************************/
