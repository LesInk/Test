/***********************************************/
/*  FILE: CGOBJECT.H                           */
/***********************************************/

#ifndef _CGOBJECT_H_
#define _CGOBJECT_H_

#include <windows.h>
#include <ddraw.h>
#include "debug.hpp"
#include "cbmp.hpp"

class CGraphicsObject : public C_debugObject
{
    protected:
        CBmpGraphic *p_bmp ;
        LPDIRECTDRAWSURFACE p_surface ;
        BOOL isTransparent ;

        BOOL createSurface(LPDIRECTDRAW directDraw) ;
    public:
        CGraphicsObject() ;
        CGraphicsObject(LPDIRECTDRAW directDraw, char *p_filenameBMP) ;
        CGraphicsObject(LPDIRECTDRAW directDraw, CBmpGraphic *p_bmpGraphic) ;
        CGraphicsObject(char *p_filenameBMP) ;
        CGraphicsObject(CBmpGraphic *p_bmpGraphic) ;
        ~CGraphicsObject() ;
        BOOL isEmpty(void) ;
        BOOL render(void) ;
        void draw(LPDIRECTDRAWSURFACE p_destSurface, int x, int y, int limitX, int limitY) ;
        void drawClipped(
            LPDIRECTDRAWSURFACE p_destSurface, 
            int x, int y, 
            int clipLeft, int clipTop,
            int clipRight, int clipBottom) ;
        void draw(CBmpGraphic *p_dest, int x, int y, int limitX, int limitY) ;
        void drawClipped(
            CBmpGraphic *p_dest, 
            int x, int y, 
            int clipLeft, int clipTop,
            int clipRight, int clipBottom) ;
        void drawRotated(CGraphicsObject *p_to, WORD angle) ; /* Surface version */
        void getSize(WORD *p_width, WORD *p_height) ;

        /* Macros */
        void setTransparency(BOOL trans)  
            { isTransparent = trans ; }
        BOOL getTransparency(void)
            { return isTransparent ; }
        void changeBMP(CBmpGraphic *p_bmp)  
            { this->p_bmp = p_bmp ; render() ; }
} ;

#endif

/***********************************************/
/*  END OF FILE: CGOBJECT.H                    */
/***********************************************/
