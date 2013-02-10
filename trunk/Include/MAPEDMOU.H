/****************************************************************************/
/*    FILE:  MAPEDMOU.H                                                     */
/****************************************************************************/

#ifndef _MAPEDMOU_H_
#define _MAPEDMOU_H_

#define MAPPED_MOUSE_BOX_X 240
#define MAPPED_MOUSE_BOX_Y 160
#define MAPPED_MOUSE_BOX_WIDTH 80
#define MAPPED_MOUSE_BOX_HEIGHT 40

T_void MappedMouseBoxInitialize(T_void) ;

T_void MappedMouseBoxFinish(T_void) ;

T_void MappedMouseBoxSetStart(T_word16 x, T_word16 y) ;

T_void MappedMouseBoxSetEnd(T_word16 x, T_word16 y) ;

T_void MappedMouseBoxGetStart(T_word16 *p_x, T_word16 *p_y) ;

T_void MappedMouseBoxGetEnd(T_word16 *p_x, T_word16 *p_y) ;

#endif

/****************************************************************************/
/*    END OF FILE:  MAPEDMOU.H                                              */
/****************************************************************************/
