/****************************************************************************/
/*    FILE:  CONTROL.H                                                       */
/****************************************************************************/

#ifndef _CONTROL_H_
#define _CONTROL_H_

typedef enum
{
    CONTROL_MOUSE_MODE_NORMAL,
    CONTROL_MOUSE_MODE_LOOK,
    CONTROL_MOUSE_MODE_MOVE,
    CONTROL_MOUSE_MODE_UNKNOWN
} E_controlMouseModes;

typedef enum
{
    CONTROL_MOUSE_POINTER_DEFAULT,
    CONTROL_MOUSE_POINTER_LOOK,
    CONTROL_MOUSE_POINTER_NOLOOK,
    CONTROL_MOUSE_POINTER_MOVE_N,
    CONTROL_MOUSE_POINTER_MOVE_NE,
    CONTROL_MOUSE_POINTER_MOVE_E,
    CONTROL_MOUSE_POINTER_MOVE_SE,
    CONTROL_MOUSE_POINTER_MOVE_S,
    CONTROL_MOUSE_POINTER_MOVE_SW,
    CONTROL_MOUSE_POINTER_MOVE_W,
    CONTROL_MOUSE_POINTER_MOVE_NW,
    CONTROL_MOUSE_POINTER_UNKNOWN
} E_controlMousePointerType;


typedef struct
{
   E_controlMouseModes mode;    /* current mouse mode */
   T_buttonID buttonpushed;     /* Is a button being pushed? */
   T_word16 hotx, hoty;         /* Hot spot of mouse. */
   T_resource mouseRes;         /* Current pointer resource */
   T_bitmap *bitmap;            /* Current pointer bitmap **/
   E_controlMousePointerType type;
} T_controlMouseStruct ;

/* function prototypes */
T_void ControlInitForGamePlay(T_void);
T_void ControlInitForJustUI(T_void);
T_void ControlSetDefaultPointer (E_controlMousePointerType type);
T_void ControlSetObjectPointer (T_3dObject *p_obj);
T_void ControlFinish (T_void);
T_void ControlPointerReset (T_void);
T_sword16 ControlGetLookAngle (T_void);
T_sword16 ControlGetLookOffset(T_void) ;
T_void ControlDisplayControlPage (T_void);

#endif

