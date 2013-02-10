/****************************************************************************/
/*    FILE:  MAPEDMOU.C                                                     */
/****************************************************************************/

#include "mapedit.h"

#define MAPPED_MOUSE_BOX_BACKGROUND_COLOR COLOR_DARK_GRAY
#define MAPPED_MOUSE_BOX_FOREGROUND_COLOR COLOR_YELLOW
#define MAPPED_MOUSE_BOX_SHADOW_COLOR     COLOR_BLACK
#define MAPPED_MOUSE_BOX_EDGE_COLOR       COLOR_BLUE

/* Keep track if we have initialized the mapped mouse box module. */
static E_Boolean G_mappedMouseInitialized = FALSE ;

/* Keep track of last coordinates. */
static T_word16 G_startX = 0xFFFF ;
static T_word16 G_startY = 0xFFFF ;
static T_word16 G_endX = 0xFFFF ;
static T_word16 G_endY = 0xFFFF ;

/****************************************************************************/
/*  Routine:  MappedMouseBoxInitialize                                      */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    MappedMouseBoxInitialize draws the initial box that all the           */
/*  box updates will occur over.                                            */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    It is assumed that the current graphics screen is where this          */
/*  box is to be drawn.                                                     */
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
/*    GrDrawFrame                                                           */
/*    GrSetCursorPosition                                                   */
/*    GrDrawShadowedText                                                    */
/*    GrDrawRectangle                                                       */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  12/27/94  Created                                                */
/*                                                                          */
/****************************************************************************/

T_void MappedMouseBoxInitialize(T_void)
{
    DebugRoutine("MappedMouseBoxInitialize") ;
    DebugCheck(G_mappedMouseInitialized == FALSE) ;

#define MAPPED_MOUSE_BOX_X 240
#define MAPPED_MOUSE_BOX_Y 160
#define MAPPED_MOUSE_BOX_WIDTH 80
#define MAPPED_MOUSE_BOX_HEIGHT 40
    /* Draw the starting box. */

    /* Draw the edge. */
    GrDrawFrame(
        MAPPED_MOUSE_BOX_X,
        MAPPED_MOUSE_BOX_Y,
        MAPPED_MOUSE_BOX_X+MAPPED_MOUSE_BOX_WIDTH-1,
        MAPPED_MOUSE_BOX_Y+MAPPED_MOUSE_BOX_HEIGHT-1,
        MAPPED_MOUSE_BOX_EDGE_COLOR) ;

    /* Draw the interior */
    GrDrawRectangle(
        MAPPED_MOUSE_BOX_X+1,
        MAPPED_MOUSE_BOX_Y+1,
        MAPPED_MOUSE_BOX_X+MAPPED_MOUSE_BOX_WIDTH-2,
        MAPPED_MOUSE_BOX_Y+MAPPED_MOUSE_BOX_HEIGHT-2,
        MAPPED_MOUSE_BOX_BACKGROUND_COLOR) ;

    /* Draw the titles: */
    GrSetCursorPosition(MAPPED_MOUSE_BOX_X+3, MAPPED_MOUSE_BOX_Y+2) ;
    GrDrawShadowedText(
        "<Start>",
        MAPPED_MOUSE_BOX_FOREGROUND_COLOR,
        MAPPED_MOUSE_BOX_SHADOW_COLOR) ;
    GrSetCursorPosition(
        MAPPED_MOUSE_BOX_X+(MAPPED_MOUSE_BOX_WIDTH/2),
        MAPPED_MOUSE_BOX_Y+2) ;
    GrDrawShadowedText(
        "<End>",
        MAPPED_MOUSE_BOX_FOREGROUND_COLOR,
        MAPPED_MOUSE_BOX_SHADOW_COLOR) ;

    /* Draw X & Y labels: */
    GrSetCursorPosition(
        MAPPED_MOUSE_BOX_X+3,
        MAPPED_MOUSE_BOX_Y+15) ;
    GrDrawShadowedText(
        "X:",
        MAPPED_MOUSE_BOX_FOREGROUND_COLOR,
        MAPPED_MOUSE_BOX_SHADOW_COLOR) ;
    GrSetCursorPosition(
        MAPPED_MOUSE_BOX_X+3,
        MAPPED_MOUSE_BOX_Y+28) ;
    GrDrawShadowedText(
        "Y:",
        MAPPED_MOUSE_BOX_FOREGROUND_COLOR,
        MAPPED_MOUSE_BOX_SHADOW_COLOR) ;
    GrSetCursorPosition(
        MAPPED_MOUSE_BOX_X+(MAPPED_MOUSE_BOX_WIDTH/2),
        MAPPED_MOUSE_BOX_Y+15) ;
    GrDrawShadowedText(
        "X:",
        MAPPED_MOUSE_BOX_FOREGROUND_COLOR,
        MAPPED_MOUSE_BOX_SHADOW_COLOR) ;
    GrSetCursorPosition(
        MAPPED_MOUSE_BOX_X+(MAPPED_MOUSE_BOX_WIDTH/2),
        MAPPED_MOUSE_BOX_Y+28) ;
    GrDrawShadowedText(
        "Y:",
        MAPPED_MOUSE_BOX_FOREGROUND_COLOR,
        MAPPED_MOUSE_BOX_SHADOW_COLOR) ;

    /* Note that we are now initialized. */
    G_mappedMouseInitialized = TRUE ;

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  MappedMouseBoxFinish                                          */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    MappedMouseBoxFinish finishes up any data and varaibles that is       */
/*  used by the mapped mouse module.                                        */
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
/*    LES  12/27/94  Created                                                */
/*                                                                          */
/****************************************************************************/

T_void MappedMouseBoxFinish(T_void)
{
    DebugRoutine("MappedMouseBoxFinish") ;
    DebugCheck(G_mappedMouseInitialized == TRUE) ;

    /* Note that we are now uninitialized. */
    G_mappedMouseInitialized = FALSE ;

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  MappedMouseBoxSetStart                                        */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    MappedMouseBoxSetStart changes the start coordinates and redraws      */
/*  them if necessary.                                                      */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    It is assumed that the current graphics screen is where this          */
/*  box is to be drawn.                                                     */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_word16 x                  -- New start x coordinate                 */
/*                                                                          */
/*    T_word16 y                  -- New start y coordinate                 */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Calls:                                                                  */
/*                                                                          */
/*    GrDrawRectangle                                                       */
/*    GrSetCursorPosition                                                   */
/*    GrDrawShadowedText                                                    */
/*    itoa                                                                  */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  12/27/94  Created                                                */
/*                                                                          */
/****************************************************************************/

T_void MappedMouseBoxSetStart(T_word16 x, T_word16 y)
{
    T_byte8 number[10] ;

    DebugRoutine("MappedMouseBoxSetStart") ;
    DebugCheck(G_mappedMouseInitialized == TRUE) ;

    /* See if the x has changed. */
    if (G_startX != x)  {
        /* If it has, redraw it on the screen */
        GrDrawRectangle(
            MAPPED_MOUSE_BOX_X+15,
            MAPPED_MOUSE_BOX_Y+15,
            MAPPED_MOUSE_BOX_X+(MAPPED_MOUSE_BOX_WIDTH/2)-1,
            MAPPED_MOUSE_BOX_Y+24,
            MAPPED_MOUSE_BOX_BACKGROUND_COLOR) ;

        /* Is it not 0xFFFF?  If it isn't we can draw it. */
        if (y != 0xFFFF)   {
            /* Convert number into a string. */
            itoa(x, number, 10) ;

            /* Draw X number. */
            GrSetCursorPosition(
                MAPPED_MOUSE_BOX_X+15,
                MAPPED_MOUSE_BOX_Y+15) ;
            GrDrawShadowedText(
                number,
                MAPPED_MOUSE_BOX_FOREGROUND_COLOR,
                MAPPED_MOUSE_BOX_SHADOW_COLOR) ;
        }

        /* ... and store it. */
        G_startX = x ;
    }

    /* See if the y has changed. */
    if (G_startY != y)  {
        /* If it has, redraw it on the screen */
        GrDrawRectangle(
            MAPPED_MOUSE_BOX_X+15,
            MAPPED_MOUSE_BOX_Y+28,
            MAPPED_MOUSE_BOX_X+(MAPPED_MOUSE_BOX_WIDTH/2)-1,
            MAPPED_MOUSE_BOX_Y+36,
            MAPPED_MOUSE_BOX_BACKGROUND_COLOR) ;

        /* Is it not 0xFFFF?  If it isn't we can draw it. */
        if (y != 0xFFFF)   {
            /* Convert number into a string. */
            itoa(y, number, 10) ;

            /* Draw Y number. */
            GrSetCursorPosition(
                MAPPED_MOUSE_BOX_X+15,
                MAPPED_MOUSE_BOX_Y+28) ;
            GrDrawShadowedText(
                number,
                MAPPED_MOUSE_BOX_FOREGROUND_COLOR,
                MAPPED_MOUSE_BOX_SHADOW_COLOR) ;
        }

        /* ... and store it. */
        G_startY = y ;
    }

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  MappedMouseBoxSetEnd                                          */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    MappedMouseBoxSetEnd   changes the end   coordinates and redraws      */
/*  them if necessary.                                                      */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    It is assumed that the current graphics screen is where this          */
/*  box is to be drawn.                                                     */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_word16 x                  -- New end   x coordinate                 */
/*                                                                          */
/*    T_word16 y                  -- New end   y coordinate                 */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Calls:                                                                  */
/*                                                                          */
/*    GrDrawRectangle                                                       */
/*    GrSetCursorPosition                                                   */
/*    GrDrawShadowedText                                                    */
/*    itoa                                                                  */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  12/27/94  Created                                                */
/*                                                                          */
/****************************************************************************/

T_void MappedMouseBoxSetEnd(T_word16 x, T_word16 y)
{
    T_byte8 number[10] ;

    DebugRoutine("MappedMouseBoxSetEnd") ;
    DebugCheck(G_mappedMouseInitialized == TRUE) ;

    /* See if the x has changed. */
    if (G_endX != x)  {
        /* If it has, redraw it on the screen */
        GrDrawRectangle(
            MAPPED_MOUSE_BOX_X+(MAPPED_MOUSE_BOX_WIDTH/2)+13,
            MAPPED_MOUSE_BOX_Y+15,
            MAPPED_MOUSE_BOX_X+MAPPED_MOUSE_BOX_WIDTH-2,
            MAPPED_MOUSE_BOX_Y+24,
            MAPPED_MOUSE_BOX_BACKGROUND_COLOR) ;

        /* Is it not 0xFFFF?  If it isn't we can draw it. */
        if (y != 0xFFFF)   {
            /* Convert number into a string. */
            itoa(x, number, 10) ;

            /* Draw X number. */
            GrSetCursorPosition(
                MAPPED_MOUSE_BOX_X+(MAPPED_MOUSE_BOX_WIDTH/2)+13,
                MAPPED_MOUSE_BOX_Y+15) ;
            GrDrawShadowedText(
                number,
                MAPPED_MOUSE_BOX_FOREGROUND_COLOR,
                MAPPED_MOUSE_BOX_SHADOW_COLOR) ;
        }

        /* ... and store it. */
        G_endX = x ;
    }

    /* See if the y has changed. */
    if (G_endY != y)  {
        /* If it has, redraw it on the screen */
        GrDrawRectangle(
            MAPPED_MOUSE_BOX_X+(MAPPED_MOUSE_BOX_WIDTH/2)+13,
            MAPPED_MOUSE_BOX_Y+28,
            MAPPED_MOUSE_BOX_X+MAPPED_MOUSE_BOX_WIDTH-2,
            MAPPED_MOUSE_BOX_Y+36,
            MAPPED_MOUSE_BOX_BACKGROUND_COLOR) ;

        /* Is it not 0xFFFF?  If it isn't we can draw it. */
        if (y != 0xFFFF)   {
            /* Convert number into a string. */
            itoa(y, number, 10) ;

            /* Draw Y number. */
            GrSetCursorPosition(
                MAPPED_MOUSE_BOX_X+(MAPPED_MOUSE_BOX_WIDTH/2)+13,
                MAPPED_MOUSE_BOX_Y+28) ;
            GrDrawShadowedText(
                number,
                MAPPED_MOUSE_BOX_FOREGROUND_COLOR,
                MAPPED_MOUSE_BOX_SHADOW_COLOR) ;
        }

        /* ... and store it. */
        G_endY = y ;
    }

    DebugEnd() ;
}


/****************************************************************************/
/*  Routine:  MappedMouseBoxGetStart                                        */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    MappedMouseBoxGetStart uses the given pointers to return the          */
/*  originally selected mouse start x and y coordinate.                     */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_word16 *p_x               -- Pointer to store x coordinate          */
/*                                                                          */
/*    T_word16 *p_y               -- Pointer to store y coordinate          */
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
/*    LES  12/27/94  Created                                                */
/*                                                                          */
/****************************************************************************/

T_void MappedMouseBoxGetStart(T_word16 *p_x, T_word16 *p_y)
{
    DebugRoutine("MappedMouseBoxGetStart") ;
    DebugCheck(G_mappedMouseInitialized == TRUE) ;
    DebugCheck(p_x != NULL) ;
    DebugCheck(p_y != NULL) ;

    *p_x = G_startX ;
    *p_y = G_startY ;

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  MappedMouseBoxGetEnd                                          */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    MappedMouseBoxGetEnd   uses the given pointers to return the          */
/*  originally selected mouse end   x and y coordinate.                     */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_word16 *p_x               -- Pointer to store x coordinate          */
/*                                                                          */
/*    T_word16 *p_y               -- Pointer to store y coordinate          */
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
/*    LES  12/27/94  Created                                                */
/*                                                                          */
/****************************************************************************/

T_void MappedMouseBoxGetEnd(T_word16 *p_x, T_word16 *p_y)
{
    DebugRoutine("MappedMouseBoxGetEnd") ;
    DebugCheck(G_mappedMouseInitialized == TRUE) ;
    DebugCheck(p_x != NULL) ;
    DebugCheck(p_y != NULL) ;

    *p_x = G_endX ;
    *p_y = G_endY ;

    DebugEnd() ;
}


/****************************************************************************/
/*    END OF FILE:  MAPEDMOU.C                                              */
/****************************************************************************/
