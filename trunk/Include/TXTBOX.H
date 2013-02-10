/****************************************************************************/
/*    FILE:  TXTBOX.H                                                       */
/****************************************************************************/

#ifndef _TXTBOX_H_
#define _TXTBOX_H_

#define MAX_TXTBOXES 50

typedef T_void *T_txtboxID;
typedef T_void (*T_txtboxHandler) (T_txtboxID txtboxID);

typedef enum
{
    TXTBOX_MODE_EDIT_FIELD,
    TXTBOX_MODE_EDIT_FORM,
    TXTBOX_MODE_VIEW_SCROLL_FORM,
    TXTBOX_MODE_VIEW_NOSCROLL_FORM,
    TXTBOX_MODE_SELECTION_BOX,
    TXTBOX_MODE_FIXED_WIDTH_FIELD,
    TXTBOX_MODE_UNKNOWN
} E_txtboxMode;

typedef enum
{
    TXTBOX_JUSTIFY_LEFT,
    TXTBOX_JUSTIFY_CENTER,
    TXTBOX_JUSTIFY_UNKNOWN
} E_txtboxJustify;


typedef enum
{
    TXTBOX_ACTION_NO_ACTION,
    TXTBOX_ACTION_GAINED_FOCUS,
    TXTBOX_ACTION_LOST_FOCUS,
    TXTBOX_ACTION_ACCEPTED,
    TXTBOX_ACTION_DATA_CHANGED,
    TXTBOX_ACTION_SELECTION_CHANGED,
    TXTBOX_ACTION_UNKNOWN
} E_txtboxAction;


typedef struct
{

    T_graphicID     p_graphicID;

    /* current data string */
    T_byte8         *data;

    /* location in pixels for the window */
    T_word16        lx1;
    T_word16        ly1;
    T_word16        lx2;
    T_word16        ly2;

    /* window mode */
    E_txtboxMode    mode;

    /* default window justification */
    E_txtboxJustify justify;

    /* maximum length in bytes for data */
    T_word32        maxlength;

    /* hotkey for window selection */
    T_byte8         hotkey;

    /* is data numeric or alpha-numeric? */
    E_Boolean       numericonly;

    /* window colors */
    T_byte8         textcolor;
    T_byte8         textshadow;
    T_byte8         backcolor;
    T_byte8         bordercolor1;
    T_byte8         bordercolor2;
    T_byte8         hbackcolor;
    T_byte8         hbordercolor1;
    T_byte8         hbordercolor2;
    T_byte8         htextcolor;

    /* cursor position (in characters) */
    T_word32        cursorl;
    /* cursor position (in rows) */
    T_word16        cursorline;
    /* cursor position (in pixels) */
    T_word16        cursorx;
    T_word16        cursory;

    /* row of data that display starts at */
    T_word16        windowstartline;
    /* number of rows visible in the window */
    T_word16        windowrows;
    /* total number of rows of data */
    T_word16        totalrows;

    /* callback routine to be activated when an event happens to this window */
    T_txtboxHandler txtboxcallback;

    /* window font */
    T_resource      font;

    /* height of font */
    T_byte8         fontheight;

    /* is this text box selected */
    E_Boolean       isselected;

    /* is this text box full */
    E_Boolean       isfull;

    /* parsing data */
    T_word32 *linestarts;
    T_word16 *linewidths;

    /* scroll bar data */
    T_buttonID sbupID;
    T_buttonID sbdnID;
    T_graphicID sbgrID;
    /* start y position of sbar box */
    T_word16 sbstart;
    /* length of sbar box */
    T_word16 sblength;

} T_txtboxStruct;

T_txtboxID TxtboxCreate (T_word16 x1,
                         T_word16 y1,
                         T_word16 x2,
                         T_word16 y2,
                         T_byte8 *fontname,
                         T_word32 maxlength,
                         T_byte8  hotkey,
                         E_Boolean numericonly,
                         E_txtboxJustify justify,
                         E_txtboxMode boxmode,
                         T_txtboxHandler callback);

T_void TxtboxDelete (T_txtboxID txtboxID);
T_void TxtboxCleanUp (T_void);

T_void TxtboxCursTop  (T_txtboxID txtboxID);
T_void TxtboxCursBot  (T_txtboxID txtboxID);
T_void TxtboxCursHome (T_txtboxID txtboxID);
T_void TxtboxCursEnd  (T_txtboxID txtboxID);
T_void TxtboxCursUp   (T_txtboxID txtboxID);
T_void TxtboxCursDn   (T_txtboxID txtboxID);
T_void TxtboxCursLeft (T_txtboxID txtboxID);
T_void TxtboxCursRight(T_txtboxID txtboxID);
T_void TxtboxCursPgUp (T_txtboxID txtboxID);
T_void TxtboxCursPgDn (T_txtboxID txtboxID);
T_void TxtboxCursSetRow (T_txtboxID txtboxID, T_word16 row);

T_void TxtboxCursMoveTo (T_txtboxID txtboxID, T_word16 x, T_word16 y);

T_void TxtboxAppendKey (T_txtboxID txtboxID, T_byte8 scankey);
T_void TxtboxAppendString (T_txtboxID txtboxID, T_byte8 *data);
T_void TxtboxSetData (T_txtboxID txtboxID, T_byte8 *data);
T_void TxtboxSetNData (T_txtboxID txtboxID, T_byte8 *data, T_word32 len);

T_byte8 *TxtboxGetData (T_txtboxID txtboxID);
T_word32 TxtboxGetDataLength (T_txtboxID txtboxID);

T_void TxtboxSetColor (T_txtboxID txtboxID,
                       T_byte8 txtcolor,
                       T_byte8 bkcolor,
                       T_byte8 txtshadow,
                       T_byte8 bordclr1,
                       T_byte8 bordclr2);
T_void TxtboxBackSpace (T_txtboxID txtboxID);

T_void TxtboxKeyControl (E_keyboardEvent event, T_byte8 scankey);
T_void TxtboxMouseControl (E_mouseEvent event, T_word16 x, T_word16 y, T_buttonClick button);
T_void TxtboxRepaginate (T_txtboxID txtboxID);
T_void TxtboxRepaginateAll (T_txtboxID txtboxID);
T_word32 TxtboxScanRow (T_txtboxID txtboxID, T_word16 rowinc, T_word16 ox);
T_void TxtboxDrawCallBack (T_graphicID graphicID, T_word16 index);
T_void TxtboxUpdate (T_txtboxID txtboxID);
T_void TxtboxNextBox (T_void);
T_void TxtboxLastBox (T_void);
T_void TxtboxFirstBox (T_void);
E_Boolean TxtboxIsAt (T_txtboxID txtboxID, T_word16 x, T_word16 y);
E_Boolean TxtboxIsSelected (T_txtboxID txtboxID);

T_void TxtboxSetScrollBarObjIDs (T_txtboxID txtboxID,
                                 T_buttonID sbupID,
                                 T_buttonID sbdnID,
                                 T_graphicID sbgrID);

T_void TxtboxHandleSBDn (T_buttonID buttonID);
T_void TxtboxHandleSBUp (T_buttonID buttonID);
T_void TxtboxUpdateSB (T_txtboxID txtboxID);
T_void TxtboxMoveSB (T_txtboxID txtboxID, T_word16 x);
T_void TxtboxAllocLine (T_txtboxID txtboxID);
T_void TxtboxSetCallback (T_txtboxID txtboxID, T_txtboxHandler newcallback);
T_void TxtboxSetNumericOnlyFlag (T_txtboxID txtboxID, E_Boolean newflag);
E_txtboxAction TxtboxGetAction (T_void);
E_Boolean TxtboxValidateID (T_txtboxID txtboxID);
T_word16 TxtboxGetSelectionNumber (T_txtboxID txtboxID);
T_void TxtboxSetMaxLength (T_txtboxID txtboxID,T_word32 newmaxlen);
T_word16 TxtboxCanFit (T_txtboxID txtboxID, T_byte8 *stringToFit);

T_void *TxtboxGetStateBlock(T_void) ;
T_void TxtboxSetStateBlock(T_void *p_state) ;

#endif
