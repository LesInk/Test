/****************************************************************************/
/*    FILE:  VIEWREGN.C                                                     */
/****************************************************************************/

#include "mapedit.h"

#define VIEW_REGION_BACKGROUND_COLOR COLOR_WHITE
#define VIEW_REGION_FOREGROUND_COLOR COLOR_BLACK
#define VIEW_REGION_EDGE_COLOR COLOR_BLUE

/* Keep track if initialized. */
static E_Boolean G_viewRegionInitialized = FALSE ;

/* Note where the upper left hand corner of the view region is located. */
static T_word16 G_viewRegionPosX = VIEW_REGION_DEFAULT_X ;
static T_word16 G_viewRegionPosY = VIEW_REGION_DEFAULT_Y ;

/* Keep track of the interior rectangle. */
static T_word16 G_viewRegionX = 0 ;
static T_word16 G_viewRegionY = 0 ;
static T_word16 G_viewRegionWidth = 1 ;
static T_word16 G_viewRegionHeight = 1 ;

/* Internal prototypes: */
static T_void IViewRegionDrawRect(T_color color) ;

/****************************************************************************/
/*  Routine:  ViewRegionInitialize                                          */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    ViewRegionInitialize sets up the view region for the overhead view    */
/*  and draws the first view region.                                        */
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
/*    ViewRegionDraw                                                        */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  12/27/94  Created                                                */
/*                                                                          */
/****************************************************************************/

T_void ViewRegionInitialize(T_void)
{
    DebugRoutine("ViewRegionInitialize") ;
    DebugCheck(G_viewRegionInitialized == FALSE) ;

    /* Go to the default upper left hand corner. */
    G_viewRegionPosX = VIEW_REGION_DEFAULT_X ;
    G_viewRegionPosY = VIEW_REGION_DEFAULT_Y ;

    /* Note that we are now initialized. */
    G_viewRegionInitialized = TRUE ;

    /* Make the actual screen the current screen. */
    GrScreenSet(GRAPHICS_ACTUAL_SCREEN) ;

    /* Draw the image. */
    ViewRegionDraw() ;

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  ViewRegionFinish                                              */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    ViewRegionFinish cleans up any memory or information used by          */
/*  the view region module.                                                 */
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

T_void ViewRegionFinish(T_void)
{
    DebugRoutine("ViewRegionFinish") ;
    DebugCheck(G_viewRegionInitialized == TRUE) ;

    /* Note that we are now uninitialized. */
    G_viewRegionInitialized = FALSE ;

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  ViewRegionGoto                                                */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    ViewRegionGoto moves the viewed area box to the given region.         */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_word16 mapX               -- X position on map                      */
/*                                                                          */
/*    T_word16 mapY               -- Y position on map                      */
/*                                                                          */
/*    T_word16 width              -- Width of view region in map squares    */
/*                                                                          */
/*    T_word16 height             -- Height of view region in map squares   */
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

T_void ViewRegionGoto(
           T_word16 mapX,
           T_word16 mapY,
           T_word16 width,
           T_word16 height)
{
    if ((mapX != G_viewRegionX) ||
        (mapY != G_viewRegionY) ||
        (width != G_viewRegionWidth) ||
        (height != G_viewRegionHeight))  {

        /* Erase the old position. */
        IViewRegionDrawRect(VIEW_REGION_BACKGROUND_COLOR) ;

        /* Move to the new position. */
        G_viewRegionX = mapX ;
        G_viewRegionY = mapY ;
        G_viewRegionWidth = width ;
        G_viewRegionHeight = height ;

        /* Draw the new position. */
        IViewRegionDrawRect(VIEW_REGION_FOREGROUND_COLOR) ;
    }
}

/****************************************************************************/
/*  Routine:  ViewRegionDraw                                                */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    ViewRegionDraw displays the whole view region in its entirity no      */
/*  matter what situation the current screen may be in.                     */
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
/*    IViewRegionDrawRect                                                   */
/*    GrDrawRectangle                                                       */
/*    GrDrawFrame                                                           */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  12/27/94  Created                                                */
/*                                                                          */
/****************************************************************************/

T_void ViewRegionDraw(T_void)
{
    DebugRoutine("ViewRegionDraw") ;
    DebugCheck(G_viewRegionInitialized == TRUE) ;

    /* Draw border. */
    GrDrawFrame(
        G_viewRegionPosX,
        G_viewRegionPosY,
        G_viewRegionPosX+VIEW_REGION_WIDTH-1,
        G_viewRegionPosY+VIEW_REGION_HEIGHT-1,
        VIEW_REGION_EDGE_COLOR) ;

    /* Draw interior. */
    GrDrawRectangle(
        G_viewRegionPosX+1,
        G_viewRegionPosY+1,
        G_viewRegionPosX+VIEW_REGION_WIDTH-2,
        G_viewRegionPosY+VIEW_REGION_HEIGHT-2,
        VIEW_REGION_BACKGROUND_COLOR) ;

    /* Draw interior rectangle. */
    IViewRegionDrawRect(VIEW_REGION_FOREGROUND_COLOR) ;

    DebugEnd() ;
}


/****************************************************************************/
/*  Routine:  IViewRegionDrawRect                                           */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    IViewRegionDrawRect draws the basic rectangle showing where the       */
/*  edit view is in reference to the whole map.                             */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_color color               -- Color to draw rectangle with           */
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
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  12/27/94  Created                                                */
/*                                                                          */
/****************************************************************************/

static T_void IViewRegionDrawRect(T_color color)
{
    T_word16 x1, y1, x2, y2 ;

    DebugRoutine("IViewRegionDrawRect") ;

    /* Calculate positions. */
    x1 = 1+G_viewRegionPosX +
             (G_viewRegionX * VIEW_REGION_INTERIOR_WIDTH) /
                 MAP_WIDTH ;
    y1 = 1+G_viewRegionPosY +
             (G_viewRegionY * VIEW_REGION_INTERIOR_WIDTH) /
                 MAP_WIDTH ;
    x2 = 1+G_viewRegionPosX +
             ((G_viewRegionX+G_viewRegionWidth-1) *
                 VIEW_REGION_INTERIOR_WIDTH) / MAP_WIDTH ;
    y2 = 1+G_viewRegionPosY +
             ((G_viewRegionY+G_viewRegionHeight-1) *
                 VIEW_REGION_INTERIOR_HEIGHT) / MAP_HEIGHT ;

    /* Draw the frame. */
    MouseHide() ;
    GrDrawFrame(x1, y1, x2, y2, color) ;
    MouseShow() ;

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  ViewRegionMoveToMouse                                         */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    ViewRegionMoveToMouse moves the view region (and edit view) to the    */
/*  given mouse coordinates on top of the view region.                      */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_word16 mouseX             -- X position of mouse click              */
/*                                                                          */
/*    T_word16 mouseY             -- Y position of mouse click              */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Calls:                                                                  */
/*                                                                          */
/*    MapEditGoto                                                           */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  12/27/94  Created                                                */
/*                                                                          */
/****************************************************************************/

T_void ViewRegionMoveToMouse(T_word16 mouseX, T_word16 mouseY)
{
    T_word16 x, y ;

    /* Calculate the appropriate x and y position on the map. */
    x = mouseX - G_viewRegionPosX - 1 ;
    y = mouseY - G_viewRegionPosY - 1 ;
    x = (x * MAP_WIDTH) / VIEW_REGION_INTERIOR_WIDTH ;
    y = (y * MAP_WIDTH) / VIEW_REGION_INTERIOR_HEIGHT ;

    /* Move there.  (This will cause a call back to draw ourself.) */
    MapEditGoto(x, y) ;
}

/****************************************************************************/
/*    END OF FILE:  VIEWREGN.C                                              */
/****************************************************************************/
