/****************************************************************************/
/*    FILE:  UPDATE.H                                                       */
/****************************************************************************/

#include "standard.h"

T_void UpdateGameBegin(T_void) ;

T_void UpdateGameEnd(T_void) ;

T_void UpdateMapBegin(T_void) ;

T_void UpdateMapEnd(T_void) ;

T_void UpdateFrame(T_void) ;

T_void UpdateEveryFive(T_void) ;

T_void UpdateOften(T_void) ;

#ifndef SERVER_ONLY
T_void UpdateStart3dView(T_void) ;

T_void UpdateEnd3dView(T_void) ;
#else
#define UpdateStart3dView()  ((T_void) 0)
#define UpdateEnd3dView()  ((T_void) 0)
#endif

/****************************************************************************/
/*    END OF FILE:  UPDATE.H                                                */
/****************************************************************************/

