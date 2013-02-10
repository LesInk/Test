/****************************************************************************/
/*    FILE:  EDITVIEW.H                                                     */
/****************************************************************************/

#ifndef _EDITVIEW_H_
#define _EDITVIEW_H_

#define WALL_WIDTH 6
#define MAP_WIDTH 64
#define MAP_HEIGHT 64
#define EDIT_VIEW_WIDTH 20
#define EDIT_VIEW_HEIGHT 10
#define EDIT_VIEW_GREATEST_X (MAP_WIDTH-EDIT_VIEW_WIDTH)
#define EDIT_VIEW_GREATEST_Y (MAP_HEIGHT-EDIT_VIEW_HEIGHT)
#define EDIT_VIEW_SQUARE_HEIGHT 16
#define EDIT_VIEW_SQUARE_WIDTH 16
#define EDIT_VIEW_BOTTOM (EDIT_VIEW_HEIGHT*EDIT_VIEW_SQUARE_HEIGHT)
#define EDIT_VIEW_RIGHT (EDIT_VIEW_WIDTH*EDIT_VIEW_SQUARE_WIDTH)

T_void EditViewInitialize(T_void) ;

T_void EditViewFinish(T_void) ;

T_void EditViewGoto(T_word16 topLeftX, T_word16 topLeftY) ;

T_void EditViewDraw(T_void) ;

E_Boolean EditViewToggleCeiling(T_void) ;

E_Boolean EditViewToggleAct(T_void) ;

T_void PreshrinkObjectPic(T_byte8 picNumber) ;

T_void PreshrinkTexture(T_byte8 picNumber) ;

T_void PreshrinkTextures(T_void) ;

T_void PreshrinkObjectPictures(T_void) ;

T_void EditViewSetMultiHeight(T_word16 height) ;

#endif

/****************************************************************************/
/*    END OF FILE:  EDITVIEW.H                                              */
/****************************************************************************/
