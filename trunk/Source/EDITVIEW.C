/****************************************************************************/
/*    FILE:  EDITVIEW.C                                                     */
/****************************************************************************/

#include "mapedit.h"
#include "ack3d.h"

#define EDIT_VIEW_CUBE_WIDTH (WALL_WIDTH+EDIT_VIEW_SQUARE_WIDTH)
#define EDIT_VIEW_CUBE_HEIGHT (WALL_WIDTH+EDIT_VIEW_SQUARE_HEIGHT)

typedef T_byte8 T_cube[EDIT_VIEW_CUBE_HEIGHT][EDIT_VIEW_CUBE_WIDTH] ;

typedef T_cube T_cubeArray[256] ;

typedef T_byte8 T_shrunkObj[EDIT_VIEW_SQUARE_HEIGHT][EDIT_VIEW_SQUARE_WIDTH] ;

typedef T_shrunkObj T_shrunkObjs[256] ;

/* Cubes for keeping preshrunk vertical walls, horizontal walls, floors, */
/* and ceilings (the floors and ceilings are the same part). */
static T_cubeArray *G_textureCubes ;

/* Keep a pointer to the allocate memory for the shrunk object pictures. */
static T_shrunkObjs *G_shrunkObjs ;

/* Place to draw without being seen. */
static T_screen G_editViewWorkspace = SCREEN_BAD ;

/* Flag to note if the editor view has been initialized. */
static E_Boolean G_editViewInitialized = FALSE ;

/* Coordinates of the upper left hand corner of the view. */
static T_word16 G_editViewX = 0 ;
static T_word16 G_editViewY = 0 ;

static E_Boolean F_ceilingOn = FALSE ;
static E_Boolean F_actsOn = FALSE ;

static T_word16 G_multiHeight = 0 ;

/* Internal prototypes: */
static T_void IEditViewDrawOverhead(T_void) ;

static T_void IEditViewTransfer(T_void) ;

static T_void IEditViewDrawSquare(
                  T_word16 squareX,
                  T_word16 squareY,
                  T_word16 screenX,
                  T_word16 screenY) ;

static T_void IEditViewDrawHorizontalWall(
               T_word16 screenX,
               T_word16 screenY,
               T_word16 wallType) ;

static T_void IEditViewDrawVerticalWall(
               T_word16 screenX,
               T_word16 screenY,
               T_word16 wallType) ;

static T_void IEditViewDrawFloor(
               T_word16 screenX,
               T_word16 screenY,
               T_word16 floorType) ;

static T_void IEditViewDrawCeiling(
               T_word16 screenX,
               T_word16 screenY,
               T_word16 ceilingType) ;

static T_void IEditViewDrawSquareFast(
                  T_word16 squareX,
                  T_word16 squareY,
                  T_word16 screenX,
                  T_word16 screenY) ;

static T_void IEditViewDrawHorizontalWallFast(
               T_word16 screenX,
               T_word16 screenY,
               T_word16 wallType) ;

static T_void IEditViewDrawVerticalWallFast(
               T_word16 screenX,
               T_word16 screenY,
               T_word16 wallType) ;

static T_void IEditViewDrawFloorFast(
               T_word16 screenX,
               T_word16 screenY,
               T_word16 floorType) ;

static T_void IEditViewDrawCeilingFast(
               T_word16 screenX,
               T_word16 screenY,
               T_word16 ceilingType) ;

static T_void IEditViewDrawObjectFast(
               T_word16 screenX,
               T_word16 screenY,
               T_word16 objectNum) ;

static T_void IDrawTransparentRaster(
               T_byte8 *whereFrom,
               T_word16 x,
               T_word16 y,
               T_word16 length) ;

/****************************************************************************/
/*  Routine:  EditViewInitialize                                            */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    EditViewInitialize prepares the editor view for all future actions.   */
/*  Mainly it allocates memory for a background process.                    */
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
/*    GrScreenAlloc                                                         */
/*    PreshrinkTextures                                                     */
/*    PreshrinkObjectPictures                                               */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  12/26/94  Created                                                */
/*    LES  12/27/94  Added allocating memory for preshrunk cubes            */
/*                   Added call to PreshrinkTextures                        */
/*                                                                          */
/****************************************************************************/

T_void EditViewInitialize(T_void)
{
    T_word16 i ;

    DebugRoutine("EditViewInitialize") ;
    DebugCheck(G_editViewInitialized == FALSE) ;

    /* Allocate memory for the view. */
    G_editViewWorkspace = GrScreenAlloc() ;
    DebugCheck(G_editViewWorkspace != SCREEN_BAD) ;

    /* Note that we are now initialized. */
    G_editViewInitialized = TRUE ;

    /* Home the coordinates. */
    G_editViewX = 0 ;
    G_editViewY = 0 ;

    /* Allocate memory for the preshrunk texture cubes. */
    G_textureCubes = MemAlloc(sizeof(T_cubeArray)) ;
    DebugCheck(G_textureCubes != NULL) ;

    /* Shrink everything before hand. */
    PreshrinkTextures() ;

    /* Allocate memory for the object pictures. */
    G_shrunkObjs = MemAlloc(sizeof(T_shrunkObjs)) ;
    DebugCheck(G_shrunkObjs != NULL) ;

    /* Shrink the object pictures, too. */
    PreshrinkObjectPictures() ;

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  EditViewFinish                                                */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    EditViewFinish is called to deallocate any memory used by the         */
/*  editor view.                                                            */
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
/*    GrScreenFree                                                          */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  12/26/94  Created                                                */
/*    LES  12/27/94  Added preshrunk cubes freeing                          */
/*                                                                          */
/****************************************************************************/

T_void EditViewFinish(T_void)
{
    T_word16 i ;

    DebugRoutine("EditViewFinish") ;
    DebugCheck(G_editViewInitialized == TRUE) ;

    /* Make sure the workspace is not the current screen. */
    GrScreenSet(GRAPHICS_ACTUAL_SCREEN) ;

    /* Free up the work space. */
    GrScreenFree(G_editViewWorkspace) ;

    /* Note that the view is no longer initialized. */
    G_editViewInitialized = TRUE ;

    /* Free memory used by the preshrunk texture cubes. */
    MemFree(G_textureCubes) ;

    /* Free the memory used by the shrunk pricture objects. */
    MemFree(G_shrunkObjs) ;

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  EditViewGoto                                                  */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    EditViewGoto moves where the edit screen is located based on the      */
/*  upper left hand coordinate.                                             */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_word16 topLeftX           -- X position of top left corner          */
/*                                                                          */
/*    T_word16 topLeftY           -- Y position of top left corner          */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Calls:                                                                  */
/*                                                                          */
/*    ViewRegionGoto                                                        */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  12/26/94  Created                                                */
/*                                                                          */
/****************************************************************************/

T_void EditViewGoto(T_word16 topLeftX, T_word16 topLeftY)
{
    DebugRoutine("EditViewGoto") ;
    DebugCheck(G_editViewInitialized == TRUE) ;

    /* Make sure the top left x is in the bounds.  If not, adjust it. */
    if (topLeftX > EDIT_VIEW_GREATEST_X)
        topLeftX = EDIT_VIEW_GREATEST_X ;

    /* Make sure the top left y is in the bounds.  If not, adjust it. */
    if (topLeftY > EDIT_VIEW_GREATEST_Y)
        topLeftY = EDIT_VIEW_GREATEST_Y ;

    /* See if the coordinates are different. */
    if ((G_editViewX != topLeftX) || (G_editViewY != topLeftY))  {
        /* Set the XY coordinates. */
        G_editViewX = topLeftX ;
        G_editViewY = topLeftY ;

        /* Back on the actual screen, draw some changes. */
        GrScreenSet(GRAPHICS_ACTUAL_SCREEN) ;

        ViewRegionGoto(
            G_editViewX,
            G_editViewY,
            EDIT_VIEW_WIDTH,
            EDIT_VIEW_HEIGHT) ;
    }

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  EditViewDraw                                                  */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    EditViewDraw does all the work necessary to draw and show the         */
/*  overhead editor view.  All floors, ceilings, and walls are drawn        */
/*  in the appropriate textures.                                            */
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
/*    IEditViewDrawOverhead                                                 */
/*    IEditViewTransfer                                                     */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  12/26/94  Created                                                */
/*                                                                          */
/****************************************************************************/

T_void EditViewDraw(T_void)
{
    DebugRoutine("EditViewDraw") ;
    DebugCheck(G_editViewInitialized == TRUE) ;

    /* Draw the screen. */
    IEditViewDrawOverhead() ;

    /* Show the upper part. */
    IEditViewTransfer() ;

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  IEditViewDrawOverhead              * INTERNAL *               */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    IEditViewDrawOverhead draws the semi 3D overhead perspective of       */
/*  the map to the workspace (after clearing the workspace).                */
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
/*    LES  12/26/94  Created                                                */
/*                                                                          */
/****************************************************************************/

static T_void IEditViewDrawOverhead(T_void)
{
    T_word16 x, y ;
    T_word16 screenX, screenY ;

    DebugRoutine("IEditViewDrawOverhead") ;

    /* Use the workspace. */
    GrScreenSet(G_editViewWorkspace) ;

    /* Clear the workspace. */
    GrDrawRectangle(0, 0, SCREEN_SIZE_X-1, SCREEN_SIZE_Y-1, COLOR_BLACK) ;

    /* Draw the matrix of squares on a square by square basis. */
    for (y=0, screenY=0;
         y<EDIT_VIEW_HEIGHT;
         y++, screenY += EDIT_VIEW_SQUARE_HEIGHT)
        for (x=0, screenX = 0;
             x<EDIT_VIEW_WIDTH;
             x++, screenX += EDIT_VIEW_SQUARE_WIDTH)
            IEditViewDrawSquareFast(
                G_editViewX+x,
                G_editViewY+y,
                screenX,
                screenY) ;

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  IEditViewTransfer                  * INTERNAL *               */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    IEditViewTransfer displays on the screen the current workspace.       */
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
/*    LES  12/26/94  Created                                                */
/*                                                                          */
/****************************************************************************/

static T_void IEditViewTransfer(T_void)
{
    DebugRoutine("IEditViewTransfer") ;

    /* Use the workspace. */
    GrScreenSet(G_editViewWorkspace) ;

    /* Transfer the upper part. */
    GrTransferRectangle(
        GRAPHICS_ACTUAL_SCREEN,
        0,
        0,
        (EDIT_VIEW_WIDTH * EDIT_VIEW_SQUARE_WIDTH)-1,
        (EDIT_VIEW_HEIGHT * EDIT_VIEW_SQUARE_HEIGHT)-1,
        0,
        0) ;

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  IEditViewDrawSquare                * INTERNAL *               */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    IEditViewDrawSquare draws a given (x,y) map square with floor,        */
/*  ceiling (optional), and walls to the given screen (x,y) coordinate.     */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_word16 squareX            -- X location on map                      */
/*                                                                          */
/*    T_word16 squareY            -- Y location on map                      */
/*                                                                          */
/*    T_word16 screenX            -- X location on the current screen       */
/*                                                                          */
/*    T_word16 screenY            -- Y location on the current screen       */
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
/*    LES  12/26/94  Created                                                */
/*                                                                          */
/****************************************************************************/

static T_void IEditViewDrawSquare(
                  T_word16 squareX,
                  T_word16 squareY,
                  T_word16 screenX,
                  T_word16 screenY)
{
    T_word16 wallType ;
    T_word16 floorType ;
    T_word16 ceilingType ;

    DebugRoutine("IEditViewDrawSquare") ;
    DebugCheck(squareX < MAP_WIDTH) ;
    DebugCheck(squareY < MAP_HEIGHT) ;
    DebugCheck(screenX < SCREEN_SIZE_X) ;
    DebugCheck(screenY < SCREEN_SIZE_Y) ;

    /* Get the floor type. */
    floorType = ViewGetFloor(squareX, squareY) ;

    /* Draw the floor section. */
    IEditViewDrawFloor(screenX, screenY, floorType) ;

    /* Get the horizontal wall type. */
    wallType = ViewGetHorizontalWall(squareX, squareY) ;

    /* Draw that type of wall. */
    IEditViewDrawHorizontalWall(screenX, screenY, wallType) ;

    /* Get the vertical wall type. */
    wallType = ViewGetVerticalWall(squareX, squareY) ;

    /* Draw that type of wall. */
    IEditViewDrawVerticalWall(screenX, screenY, wallType) ;

    if (F_ceilingOn == TRUE)  {
        /* Get the ceiling type. */
        ceilingType = ViewGetCeiling(squareX, squareY) ;

        /* Draw the ceiling section. */
        IEditViewDrawCeiling(screenX, screenY, ceilingType) ;
    }

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  IEditViewDrawHorizontalWall        * INTERNAL *               */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    IEditViewDrawHorizontalWall draws the top edge of a map square on the */
/*  current screen (usually the edit view workspace).                       */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_word16 screenX            -- Screen X position to draw at           */
/*                                                                          */
/*    T_word16 screenY            -- Screen Y position to draw at           */
/*                                                                          */
/*    T_word16 wallType           -- Type of wall to draw (picture)         */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Calls:                                                                  */
/*                                                                          */
/*    GrDrawPixel                                                           */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  12/26/94  Created                                                */
/*                                                                          */
/****************************************************************************/

static T_void IEditViewDrawHorizontalWall(
               T_word16 screenX,
               T_word16 screenY,
               T_word16 wallType)
{
    T_byte8 *p_bitmap ;
    T_byte8 picNum ;
    T_word16 x, y ;
    T_word16 xPos, yPos ;
    T_byte8 color ;
    E_Boolean isTransparent ;

    DebugRoutine("IEditViewDrawHorizontalWall") ;

    /* Extract the picture's number. */
    picNum = wallType & 255 ;

    /* Determine if this a transparent wall. */
    isTransparent = ((wallType & WALL_TYPE_TRANS)==WALL_TYPE_TRANS)?TRUE:FALSE ;

    /* if it is zero, don't draw. */
    if (picNum != 0)  {
        /* Not zero, therefore we have something to draw. */
        /* Get ahold of the picture's bitmap. */
        p_bitmap = ViewGetTexture(picNum) ;

        /* Is it a good picture? */
        if (p_bitmap != NULL)  {
            /* Yes draw it. */
            for (y=0, yPos=0;
                 y<WALL_WIDTH;
                 y++, yPos += (64/WALL_WIDTH))
                for (x=0, xPos=0;
                     x<EDIT_VIEW_SQUARE_HEIGHT;
                     x++, xPos += (64/EDIT_VIEW_SQUARE_HEIGHT))  {
                    color = p_bitmap[(yPos<<6)+xPos] ;
                    if ((color != 0) || (isTransparent == FALSE))  {
                        if (screenX+y+x < SCREEN_SIZE_X)
                            GrDrawPixel(
                                screenX+y+x,
                                screenY+y,
                                p_bitmap[(xPos<<6)+yPos]) ;
                    }
                }
        } else {
            /* No, put a weird gray out image. */
            for (y=0; y<WALL_WIDTH; y++)
                for (x=0; x<EDIT_VIEW_SQUARE_HEIGHT; x++)
                    if (((x+y)^x)&1)  {
                        if (screenX+y+x < SCREEN_SIZE_X)
                            GrDrawPixel(
                                screenX+y+x,
                                screenY+y,
                                COLOR_DARK_GRAY) ;
                    }
        }
    }

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  IEditViewDrawVerticalWall          * INTERNAL *               */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    IEditViewDrawVerticalWall draws the left edge of a map square on the  */
/*  current screen (usually the edit view workspace).                       */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_word16 screenX            -- Screen X position to draw at           */
/*                                                                          */
/*    T_word16 screenY            -- Screen Y position to draw at           */
/*                                                                          */
/*    T_word16 wallType           -- Type of wall to draw (picture)         */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Calls:                                                                  */
/*                                                                          */
/*    GrDrawPixel                                                           */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  12/26/94  Created                                                */
/*                                                                          */
/****************************************************************************/

static T_void IEditViewDrawVerticalWall(
               T_word16 screenX,
               T_word16 screenY,
               T_word16 wallType)
{
    T_byte8 *p_bitmap ;
    T_byte8 picNum ;
    T_word16 x, y ;
    T_word16 xPos, yPos ;
    E_Boolean isTransparent ;
    T_byte8 color ;

    DebugRoutine("IEditViewDrawVerticalWall") ;

    /* Extract the picture's number. */
    picNum = wallType & 255 ;

    /* Determine if this a transparent wall. */
    isTransparent = ((wallType & WALL_TYPE_TRANS)==WALL_TYPE_TRANS)?TRUE:FALSE ;

    /* if it is zero, don't draw. */
    if (picNum != 0)  {
        /* Not zero, therefore we have something to draw. */
        /* Get ahold of the picture's bitmap. */
        p_bitmap = ViewGetTexture(picNum) ;

        /* Is it a good picture? */
        if (p_bitmap != NULL)  {
            /* Yes draw it. */
            for (y=0, yPos=0;
                 y<EDIT_VIEW_SQUARE_HEIGHT;
                 y++, yPos += (64/EDIT_VIEW_SQUARE_HEIGHT))
                for (x=0, xPos=0;
                     x<WALL_WIDTH;
                     x++, xPos += (64/WALL_WIDTH))  {
                    color = p_bitmap[(yPos<<6)+xPos] ;
                    if ((color != 0) || (isTransparent == FALSE))  {
                        if (screenX+x < SCREEN_SIZE_X)
                            GrDrawPixel(
                                screenX+x,
                                screenY+y+x,
                                color) ;
                    }
                }
        } else {
            /* No, put a weird gray out image. */
            for (y=0; y<EDIT_VIEW_SQUARE_HEIGHT; y++)
                for (x=0; x<WALL_WIDTH; x++)
                    if (((x+y)^x)&1)  {
                        if (screenX+x < SCREEN_SIZE_X)
                            GrDrawPixel(
                                screenX+x,
                                screenY+y+x,
                                COLOR_DARK_GRAY) ;
                    }
        }
    }

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  IEditViewDrawFloor                 * INTERNAL *               */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    IEditViewDrawFloor draws the floor texture at the given screen        */
/*  coordinates using the given floor type.                                 */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_word16 screenX            -- Screen X position to draw at           */
/*                                                                          */
/*    T_word16 screenY            -- Screen Y position to draw at           */
/*                                                                          */
/*    T_word16 floorType          -- Type of floor to draw (picture)        */
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
/*    LES  12/26/94  Created                                                */
/*                                                                          */
/****************************************************************************/

static T_void IEditViewDrawFloor(
               T_word16 screenX,
               T_word16 screenY,
               T_word16 floorType)
{
    T_byte8 *p_bitmap ;
    T_byte8 picNum ;
    T_word16 x, y ;
    T_word16 xPos, yPos ;

    DebugRoutine("IEditViewDrawFloor") ;

    /* Extract the picture's number. */
    picNum = floorType & 255 ;

    /* Get ahold of the picture's bitmap. */
    p_bitmap = ViewGetTexture(picNum) ;

    /* Is it a good picture? */
    if (p_bitmap != NULL)  {
        /* Yes draw it. */
        for (y=WALL_WIDTH, yPos=0;
             y<EDIT_VIEW_SQUARE_HEIGHT+WALL_WIDTH;
             y++, yPos += (64/EDIT_VIEW_SQUARE_HEIGHT))
            for (x=WALL_WIDTH, xPos=0;
                 x<EDIT_VIEW_SQUARE_WIDTH+WALL_WIDTH;
                 x++, xPos += (64/EDIT_VIEW_SQUARE_WIDTH))  {
                if (screenX+x < SCREEN_SIZE_X)  {
                    GrDrawPixel(
                        screenX+x,
                        screenY+y,
                        p_bitmap[(xPos<<6)+yPos]) ;
                }
            }
    } else {
        /* No, put a weird grayed out image. */
        for (y=WALL_WIDTH, yPos=0;
             y<EDIT_VIEW_SQUARE_HEIGHT+WALL_WIDTH;
             y++, yPos += (64/EDIT_VIEW_SQUARE_HEIGHT))
            for (x=WALL_WIDTH, xPos=0;
                 x<EDIT_VIEW_SQUARE_WIDTH+WALL_WIDTH;
                 x++, xPos += (64/EDIT_VIEW_SQUARE_WIDTH))  {
                if (screenX+x < SCREEN_SIZE_X)
                    GrDrawPixel(
                        screenX+x,
                        screenY+y,
                        ((x^y)&1)?COLOR_LIGHT_GRAY:COLOR_DARK_GRAY) ;
            }
    }

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  IEditViewDrawCeiling               * INTERNAL *               */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    IEditViewDrawCeiling draws the ceiling square at the given screen     */
/*  coordinates using the given ceiling type.                               */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_word16 screenX            -- Screen X position to draw at           */
/*                                                                          */
/*    T_word16 screenY            -- Screen Y position to draw at           */
/*                                                                          */
/*    T_word16 ceilingType        -- Type of ceiling to draw (picture)      */
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
/*    LES  12/26/94  Created                                                */
/*                                                                          */
/****************************************************************************/

static T_void IEditViewDrawCeiling(
               T_word16 screenX,
               T_word16 screenY,
               T_word16 ceilingType)
{
    T_byte8 *p_bitmap ;
    T_byte8 picNum ;
    T_word16 x, y ;
    T_word16 xPos, yPos ;

    DebugRoutine("IEditViewDrawCeiling") ;

    /* Extract the picture's number. */
    picNum = ceilingType & 255 ;

    /* Is it a ceiling or show through to the sky?  We don't draw anything */
    /* When it goes to the sky. */
    if (picNum != 0)  {
        /* Yes, there is a ceiling to draw. */
        /* Get ahold of the picture's bitmap. */
        p_bitmap = ViewGetTexture(picNum) ;

        /* Is it a good picture? */
        if (p_bitmap != NULL)  {
            /* Yes draw it. */
            for (y=0, yPos=0;
                 y<EDIT_VIEW_SQUARE_HEIGHT;
                 y++, yPos += (64/EDIT_VIEW_SQUARE_HEIGHT))
                for (x=0, xPos=0;
                     x<EDIT_VIEW_SQUARE_WIDTH;
                     x++, xPos += (64/EDIT_VIEW_SQUARE_WIDTH))  {
                    if (screenX+x < SCREEN_SIZE_X)
                        GrDrawPixel(
                            screenX+x,
                            screenY+y,
                            p_bitmap[(xPos<<6)+yPos]) ;
                }
        } else {
            /* No, put a weird grayed out image. */
            for (y=0, yPos=0;
                 y<EDIT_VIEW_SQUARE_HEIGHT;
                 y++, yPos += (64/EDIT_VIEW_SQUARE_HEIGHT))
                for (x=0, xPos=0;
                     x<EDIT_VIEW_SQUARE_WIDTH;
                     x++, xPos += (64/EDIT_VIEW_SQUARE_WIDTH))  {
                    if ((x^y)&1)  {
                        if (screenX+x < SCREEN_SIZE_X)
                            GrDrawPixel(
                                screenX+x,
                                screenY+y,
                                COLOR_DARK_GRAY) ;
                    }
                }
        }
    }
    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  EditViewToggleCeiling                                         */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    EditViewToggleCeiling alternates between drawing a ceiling and        */
/*  not drawing a ceiling for the overhead view.                            */
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
/*    E_Boolean                   -- TRUE if ceiling is now on.             */
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
/*    LES  12/26/94  Created                                                */
/*                                                                          */
/****************************************************************************/

E_Boolean EditViewToggleCeiling(T_void)
{
    DebugRoutine("EditViewToggleCeiling") ;

    /* Toggle the ceiling mode. */
    F_ceilingOn = (F_ceilingOn == TRUE)?FALSE:TRUE ;

    DebugCheck(F_ceilingOn < BOOLEAN_UNKNOWN) ;
    DebugEnd() ;

    return F_ceilingOn ;
}

/****************************************************************************/
/*  Routine:  EditViewToggleAct                                             */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    EditViewToggleAct alternates between drawing activation markers and   */
/*  not drawing activation markers.                                         */
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
/*    E_Boolean                   -- TRUE if acts is now on.                */
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
/*    LES  12/26/94  Created                                                */
/*                                                                          */
/****************************************************************************/

E_Boolean EditViewToggleAct(T_void)
{
    DebugRoutine("EditViewToggleAct") ;

    /* Toggle the acts mode. */
    F_actsOn = (F_actsOn == TRUE)?FALSE:TRUE ;

    DebugCheck(F_actsOn < BOOLEAN_UNKNOWN) ;
    DebugEnd() ;

    return F_actsOn ;
}

/****************************************************************************/
/*  Routine:  PreshrinkTextures                                             */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    To help make drawing of the overhead map faster, preshrunk "cubes"    */
/*  are used.  These cubes are bitmaps of the same texture recording the    */
/*  left, top, and floor of a single square using all the same textures.    */
/*  This cube will help make drawing much faster since the cubes do not     */
/*  have to be resized constantly.                                          */
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
/*    PreshrinkTexture                                                      */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  12/27/94  Created                                                */
/*    LES  12/29/94  Modified it to use the PreshrinkTexture subroutine.    */
/*                                                                          */
/****************************************************************************/

T_void PreshrinkTextures(T_void)
{
    T_word16 i ;

    DebugRoutine("PreshrinkTextures") ;

    /* Go through all 256 pictures. */
    for (i=0; i<256; i++)
        PreshrinkTexture(i) ;

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  PreshrinkTexture                                              */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    To help make drawing of the overhead map faster, preshrunk "cubes"    */
/*  are used.  These cubes are bitmaps of the same texture recording the    */
/*  left, top, and floor of a single square using all the same textures.    */
/*  This cube will help make drawing much faster since the cubes do not     */
/*  have to be resized constantly.                                          */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_word16 textureNum         -- Number of texture to preshrink         */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Calls:                                                                  */
/*                                                                          */
/*    GrScrenSet                                                            */
/*    GrDrawRectangle                                                       */
/*    IEditViewDrawFloor                                                    */
/*    IEditViewDrawHorizontalWall                                           */
/*    IEditViewDrawVerticalWall                                             */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  12/29/94  Created                                                */
/*                                                                          */
/****************************************************************************/

T_void PreshrinkTexture(T_word16 textureNum)
{
    T_word16 j ;

    DebugRoutine("PreshrinkTexture") ;
    DebugCheck(textureNum < 256) ;

    /* Use the workspace. */
//    GrScreenSet(G_editViewWorkspace) ;
GrScreenSet(GRAPHICS_ACTUAL_SCREEN) ;

    /* Clear out the cube position so we have a black background */
    GrDrawRectangle(
        0,
        0,
        EDIT_VIEW_CUBE_WIDTH,
        EDIT_VIEW_CUBE_HEIGHT,
        COLOR_BLACK) ;

    /* Draw the 3 parts that make a cube. */
    /* Draw the floor section. */
    IEditViewDrawFloor(0, 0, textureNum) ;

    /* Draw that type of wall. */
    IEditViewDrawHorizontalWall(1, 0, textureNum) ;

    /* Draw that type of wall. */
    IEditViewDrawVerticalWall(0, 0, textureNum) ;

    /* Store this cube. */
    /* Go through each raster line and store that line. */
    for (j=0; j<EDIT_VIEW_CUBE_HEIGHT; j++)
        GrTransferRasterTo(
            (*G_textureCubes)[textureNum][j],
            0,
            j,
            EDIT_VIEW_CUBE_WIDTH) ;

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  PreshrinkObjectPictures                                       */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    IPreshrinkObjectPictures goes through all the picture textures and    */
/*  creates a shrunk version that is stored for drawing later.              */
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
/*    IPreshrinkObjectPic                                                   */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  12/27/94  Created                                                */
/*                                                                          */
/****************************************************************************/

T_void PreshrinkObjectPictures(T_void)
{
    T_word16 i ;

    DebugRoutine("PreshrinkObjectPictures") ;

    /* Go through all 256 pictures and shrink. */
    for (i=0; i<256; i++)
        PreshrinkObjectPic(i) ;

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  PreshrinkObjectPic                                            */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    PreshrinkObjectPic shrinks one object picture and stores it           */
/*  and the shrunk picture array.                                           */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_byte8 picNumber           -- Number of picture to shrink            */
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
/*    LES  12/28/94  Created                                                */
/*                                                                          */
/****************************************************************************/

T_void PreshrinkObjectPic(T_byte8 picNumber)
{
    T_byte8 *p_picture ;
    T_byte8 *p_shrunk ;
    T_word16 x, y ;

    DebugRoutine("PreshrinkObjectPic");

    /* Get a pointer to the object picture */
    p_picture = ViewGetObjectPicture(picNumber) ;

    /* Get a pointer to the shrunk position. */
    p_shrunk = &((*G_shrunkObjs)[picNumber][0][0]) ;

    /* Make sure we have a good picture. */
    if (p_picture != NULL)  {
        /* The picture exists. */
        /* Fill the slot with a smaller picture. */
        for (y=0; y<EDIT_VIEW_SQUARE_HEIGHT; y++)  {
            for (x=0; x<EDIT_VIEW_SQUARE_WIDTH; x++, p_shrunk++)  {
                *p_shrunk = *p_picture ;
                p_picture += 256 ;
            }
            p_picture -= 4092 ;
        }
    } else {
        /* The picture does not exist. */
        /* Fill the slot with a greyed out pattern. */
        for (y=0; y<EDIT_VIEW_SQUARE_HEIGHT; y++)
            for (x=0; x<EDIT_VIEW_SQUARE_WIDTH; x++, p_shrunk++)
                *p_shrunk = ((x^y)&1)?COLOR_BLACK:COLOR_LIGHT_GREEN ;
    }

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  IEditViewDrawSquareFast            * INTERNAL *               */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    IEditViewDrawSquare draws a given (x,y) map square with floor,        */
/*  ceiling (optional), and walls to the given screen (x,y) coordinate.     */
/*  (Fast version that uses preshrunk cube textures)                        */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_word16 squareX            -- X location on map                      */
/*                                                                          */
/*    T_word16 squareY            -- Y location on map                      */
/*                                                                          */
/*    T_word16 screenX            -- X location on the current screen       */
/*                                                                          */
/*    T_word16 screenY            -- Y location on the current screen       */
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

static T_void IEditViewDrawSquareFast(
                  T_word16 squareX,
                  T_word16 squareY,
                  T_word16 screenX,
                  T_word16 screenY)
{
    T_word16 wallType ;
    T_word16 floorType ;
    T_word16 ceilingType ;
    T_word16 objType ;

    DebugRoutine("IEditViewDrawSquareFast") ;
    DebugCheck(squareX < MAP_WIDTH) ;
    DebugCheck(squareY < MAP_HEIGHT) ;
    DebugCheck(screenX < SCREEN_SIZE_X) ;
    DebugCheck(screenY < SCREEN_SIZE_Y) ;

    /* Make sure there is floor. */
    /* Get the floor type. */
    floorType = ViewGetFloor(squareX, squareY) ;

    /* Draw the floor section. */
    IEditViewDrawFloorFast(screenX, screenY, floorType) ;

    /* Get the horizontal wall type. */
    wallType = ViewGetHorizontalWallAtHeight(squareX, squareY, G_multiHeight) ;

    /* Draw that type of wall. */
    IEditViewDrawHorizontalWallFast(screenX, screenY, wallType) ;

    /* Get the vertical wall type. */
    wallType = ViewGetVerticalWallAtHeight(squareX, squareY, G_multiHeight) ;

    /* Draw that type of wall. */
    IEditViewDrawVerticalWallFast(screenX, screenY, wallType) ;

    /* Get the object type at this square. */
    objType = ViewGetObject(squareX, squareY) ;

    /* Draw that type of object. */
    IEditViewDrawObjectFast(screenX, screenY, objType) ;

    if (F_actsOn == TRUE)  {
        if ((ViewGetFloorActivation(squareX, squareY) != 0) ||
            (ViewGetVerticalActivation(squareX, squareY) != 0) ||
            (ViewGetHorizontalActivation(squareX, squareY) != 0))  {
            GrDrawRectangle(
                screenX+WALL_WIDTH+4,
                screenY+WALL_WIDTH+4,
                screenX+WALL_WIDTH+8,
                screenY+WALL_WIDTH+8,
                COLOR_BLACK) ;
            GrDrawFrame(
                screenX+WALL_WIDTH+3,
                screenY+WALL_WIDTH+3,
                screenX+WALL_WIDTH+9,
                screenY+WALL_WIDTH+9,
                COLOR_WHITE) ;
        }

    }

    if (F_ceilingOn == TRUE)  {
        /* Get the ceiling type. */
        ceilingType = ViewGetCeiling(squareX, squareY) ;

        /* Draw the ceiling section. */
        IEditViewDrawCeilingFast(screenX, screenY, ceilingType) ;
    }

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  IEditViewDrawHorizontalWallFast    * INTERNAL *               */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    IEditViewDrawHorizontalWallFast draws the top edge of a map square on */
/*  the current screen (usually the edit view workspace).                   */
/*  (Fast version that uses preshrunk cube textures)                        */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_word16 screenX            -- Screen X position to draw at           */
/*                                                                          */
/*    T_word16 screenY            -- Screen Y position to draw at           */
/*                                                                          */
/*    T_word16 wallType           -- Type of wall to draw (picture)         */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Calls:                                                                  */
/*                                                                          */
/*    GrDrawPixel                                                           */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  12/27/94  Created                                                */
/*                                                                          */
/****************************************************************************/

static T_void IEditViewDrawHorizontalWallFast(
               T_word16 screenX,
               T_word16 screenY,
               T_word16 wallType)
{
    T_cube *p_cube ;
    T_byte8 picNum ;
    T_byte8 *p_floor ;
    T_word16 i ;
    T_word16 x, y ;
    E_Boolean isTransparent ;

    DebugRoutine("IEditViewDrawHorizontalWallFast") ;

    /* Extract the picture's number. */
    picNum = wallType & 255 ;

    /* If it is zero, don't draw the wall. */
    if (picNum != 0)  {
        /* Draw the wall. */
        /* Determine if this a transparent wall. */
        isTransparent = ((wallType & WALL_TYPE_TRANS)==WALL_TYPE_TRANS)?TRUE:FALSE ;

        /* Get ahold of the picture's bitmap. */
        p_cube = (*G_textureCubes)[picNum] ;

        /* Get a pointer to the cube top wall part */
        p_floor = &((*p_cube)[0][0]) ;

        /* See if this is transparent. */
        if (isTransparent == FALSE)  {
            /* Nope, solid. */
            /* Draw it. */
            for (i=0, y=screenY, x=screenX;
                 i<WALL_WIDTH;
                 i++, y++, x++, p_floor+=EDIT_VIEW_CUBE_WIDTH+1)
                GrTransferRasterFrom(p_floor, x, y, EDIT_VIEW_SQUARE_WIDTH+1) ;
        } else {
            /* Transparent here. */
            /* Draw it. */
            for (i=0, y=screenY, x=screenX;
                 i<WALL_WIDTH;
                 i++, y++, x++, p_floor+=EDIT_VIEW_CUBE_WIDTH+1)
                IDrawTransparentRaster(p_floor, x, y, EDIT_VIEW_SQUARE_WIDTH+1) ;
        }

    }

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  IEditViewDrawVerticalWallFast      * INTERNAL *               */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    IEditViewDrawVerticalWall draws the left edge of a map square on the  */
/*  current screen (usually the edit view workspace).                       */
/*  (Fast version that uses preshrunk cube textures)                        */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_word16 screenX            -- Screen X position to draw at           */
/*                                                                          */
/*    T_word16 screenY            -- Screen Y position to draw at           */
/*                                                                          */
/*    T_word16 wallType           -- Type of wall to draw (picture)         */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Calls:                                                                  */
/*                                                                          */
/*    GrDrawPixel                                                           */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  12/27/94  Created                                                */
/*                                                                          */
/****************************************************************************/

static T_void IEditViewDrawVerticalWallFast(
               T_word16 screenX,
               T_word16 screenY,
               T_word16 wallType)
{
    T_cube *p_cube ;
    T_byte8 picNum ;
    T_byte8 *p_floor ;
    T_word16 i ;
    T_word16 x, y ;
    E_Boolean isTransparent ;

    DebugRoutine("IEditViewDrawVerticalWallFast") ;

    /* Extract the picture's number. */
    picNum = wallType & 255 ;

    /* If it is zero, don't draw the wall. */
    if (picNum != 0)  {
        /* Draw the wall. */
        /* Determine if this a transparent wall. */
        isTransparent = ((wallType & WALL_TYPE_TRANS)==WALL_TYPE_TRANS)?TRUE:FALSE ;

        /* Get ahold of the picture's bitmap. */
        p_cube = (*G_textureCubes)[picNum] ;

        /* Get a pointer to the cube left wall part */
        p_floor = &((*p_cube)[0][0]) ;

        /* See if this is transparent. */
        if (isTransparent == FALSE)  {
            /* Nope, solid. */
            /* Draw top triangle part. */
            for (i=0, y=screenY, x=screenX;
                 i<WALL_WIDTH;
                 i++, y++, p_floor+=EDIT_VIEW_CUBE_WIDTH)
                GrTransferRasterFrom(p_floor, x, y, i+1) ;

            /* Draw middle part. */
            for (i=0;
                 i<EDIT_VIEW_SQUARE_HEIGHT-WALL_WIDTH;
                 i++, y++, p_floor+=EDIT_VIEW_CUBE_WIDTH)
                GrTransferRasterFrom(p_floor, x, y, WALL_WIDTH) ;

            /* Draw the lower triangle part. */
            for (i=WALL_WIDTH;
                 i>0;
                 i--, y++, x++, p_floor+=EDIT_VIEW_CUBE_WIDTH+1)
                GrTransferRasterFrom(p_floor, x, y, i) ;
        } else {
            /* Transparent here. */
            /* Draw top triangle part. */
            for (i=0, y=screenY, x=screenX;
                 i<WALL_WIDTH;
                 i++, y++, p_floor+=EDIT_VIEW_CUBE_WIDTH)
                IDrawTransparentRaster(p_floor, x, y, i+1) ;

            /* Draw middle part. */
            for (i=0;
                 i<EDIT_VIEW_SQUARE_HEIGHT-WALL_WIDTH;
                 i++, y++, p_floor+=EDIT_VIEW_CUBE_WIDTH)
                IDrawTransparentRaster(p_floor, x, y, WALL_WIDTH) ;

            /* Draw the lower triangle part. */
            for (i=WALL_WIDTH;
                 i>0;
                 i--, y++, x++, p_floor+=EDIT_VIEW_CUBE_WIDTH+1)
                IDrawTransparentRaster(p_floor, x, y, i) ;
        }

    }

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  IEditViewDrawFloorFast             * INTERNAL *               */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    IEditViewDrawFloor draws the floor texture at the given screen        */
/*  coordinates using the given floor type.                                 */
/*  (Fast version that uses preshrunk cube textures)                        */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_word16 screenX            -- Screen X position to draw at           */
/*                                                                          */
/*    T_word16 screenY            -- Screen Y position to draw at           */
/*                                                                          */
/*    T_word16 floorType          -- Type of floor to draw (picture)        */
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

static T_void IEditViewDrawFloorFast(
               T_word16 screenX,
               T_word16 screenY,
               T_word16 floorType)
{
    T_cube *p_cube ;
    T_byte8 picNum ;
    T_byte8 *p_floor ;
    T_word16 i ;
    T_word16 x, y ;

    DebugRoutine("IEditViewDrawFloorFast") ;

    /* Extract the picture's number. */
    picNum = floorType & 255 ;

    /* Get ahold of the picture's bitmap. */
    p_cube = (*G_textureCubes)[picNum] ;

    /* Get a pointer to the cube floor part */
    p_floor = &((*p_cube)[WALL_WIDTH][WALL_WIDTH]) ;

    /* Determine left edge of drawing. */
    x = screenX + WALL_WIDTH ;

    /* Draw the floor part of the cube. */
    for (i=0, y=WALL_WIDTH+screenY;
         i<EDIT_VIEW_SQUARE_HEIGHT;
         i++, y++, p_floor += EDIT_VIEW_CUBE_WIDTH)
        GrTransferRasterFrom(p_floor, x, y, EDIT_VIEW_SQUARE_WIDTH) ;

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  IEditViewDrawCeilingFast           * INTERNAL *               */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    IEditViewDrawCeiling draws the ceiling square at the given screen     */
/*  coordinates using the given ceiling type.                               */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_word16 screenX            -- Screen X position to draw at           */
/*                                                                          */
/*    T_word16 screenY            -- Screen Y position to draw at           */
/*                                                                          */
/*    T_word16 ceilingType        -- Type of ceiling to draw (picture)      */
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

static T_void IEditViewDrawCeilingFast(
               T_word16 screenX,
               T_word16 screenY,
               T_word16 ceilingType)
{
    T_cube *p_cube ;
    T_byte8 picNum ;
    T_byte8 *p_floor ;
    T_word16 i ;
    T_word16 x, y ;

    DebugRoutine("IEditViewDrawCeilingFast") ;

    /* Extract the picture's number. */
    picNum = ceilingType & 255 ;

    /* Do we have a ceiling, or is it sky? */
    if (picNum != 0)  {
        /* We have a ceiling, let's draw it. */
        /* Get ahold of the picture's bitmap. */
        p_cube = (*G_textureCubes)[picNum] ;

        /* Get a pointer to the cube floor part (which is the same as the) */
        /* ceiling part. */
        p_floor = &((*p_cube)[WALL_WIDTH][WALL_WIDTH]) ;

        /* Determine left edge of drawing. */
        x = screenX ;

        /* Draw the floor part of the cube. */
        for (i=0, y=screenY;
             i<EDIT_VIEW_SQUARE_HEIGHT;
             i++, y++, p_floor += EDIT_VIEW_CUBE_WIDTH)
            GrTransferRasterFrom(p_floor, x, y, EDIT_VIEW_SQUARE_WIDTH) ;
    }

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  IEditViewDrawObjectFast            * INTERNAL *               */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    IEditViewDrawObjectFast draws the object at the given screen          */
/*  coordinates using the given the object number.                          */
/*  (Fast version that uses preshrunk cube textures)                        */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_word16 screenX            -- Screen X position to draw at           */
/*                                                                          */
/*    T_word16 screenY            -- Screen Y position to draw at           */
/*                                                                          */
/*    T_word16 objectNum          -- Number of object to draw               */
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

static T_void IEditViewDrawObjectFast(
               T_word16 screenX,
               T_word16 screenY,
               T_word16 objectNum)
{
    T_byte8 picNum ;
    T_word16 x, y, i ;
    T_byte8 *p_objPic ;

    DebugRoutine("IEditViewDrawObjectFast") ;

    /* Check to see if this is a object or just an empty space. */
    if (objectNum != 0)  {
        /* We don't care about any other information besides the object. */
        objectNum &= 255 ;

        /* OK, it's a real object that needs to be drawn. */
        /* Get the picture for this object. */
        picNum = ViewGetObjectsPictureNumber(objectNum) ;

        /* Get ahold of the picture's bitmap. */
        p_objPic = &((*G_shrunkObjs)[picNum][0][0]) ;

        /* Determine left edge of drawing. */
        x = screenX + WALL_WIDTH ;

        /* Draw the object using transparent mode. */
        for (i=0, y=WALL_WIDTH+screenY;
             i<EDIT_VIEW_SQUARE_HEIGHT;
             i++, y++, p_objPic += EDIT_VIEW_SQUARE_WIDTH)
            IDrawTransparentRaster(p_objPic, x, y, EDIT_VIEW_SQUARE_WIDTH) ;
    }

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  IDrawTransparentRaster             * INTERNAL *               */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    IDrawTransparentRaster draws one line of graphics on the current      */
/*  screen.  As it comes accross black (0) pixels, they are not drawn.      */
/*  This allows the background to show through at certain points.           */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_byte8 *whereFrom          -- Pointer to memory to transfer          */
/*                                                                          */
/*    T_byte16 x                  -- Screen x coordinate                    */
/*                                                                          */
/*    T_byte16 y                  -- Screen y coordinate                    */
/*                                                                          */
/*    T_byte16 length             -- Number of bytes to transfer            */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Calls:                                                                  */
/*                                                                          */
/*    GrDrawPixel                                                           */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  12/27/94  Created                                                */
/*                                                                          */
/****************************************************************************/

static T_void IDrawTransparentRaster(
               T_byte8 *whereFrom,
               T_word16 x,
               T_word16 y,
               T_word16 length)
{
    T_word16 i ;

    DebugRoutine("IDrawTransparentRaster") ;

    /* Check to see we have room for the raster.  If not, cut it down. */
    if (length+x >= SCREEN_SIZE_X)
        length = SCREEN_SIZE_X-x ;

    /* Loop through all the pixels. */
    for (i=0; i<length; i++, whereFrom++, x++)
        if (*whereFrom != 0)
            GrDrawPixel(x, y, *whereFrom) ;

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  EditViewSetMultiHeight                                        */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    EditViewSetMultiHeight changes the height that the view is drawn.     */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_word16 height             -- The new height                         */
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
/*    LES  01/06/95  Created                                                */
/*                                                                          */
/****************************************************************************/

T_void EditViewSetMultiHeight(T_word16 height)
{
    DebugRoutine("EditViewSetMultiHeight") ;
    DebugCheck(height <= MAX_MULTI) ;

    G_multiHeight = height ;

    DebugEnd() ;
}

/****************************************************************************/
/*    END OF FILE:  EDITVIEW.C                                              */
/****************************************************************************/

