/***********************************************/
/*  FILE: CGSCREEN.H                           */
/***********************************************/

#ifndef _CGSCREEN_H_
#define _CGSCREEN_H_

#include <windows.h>
#include <ddraw.h>
#include "options.hpp"
#include "debug.hpp"
#include "cbmp.hpp"
#include "cgobject.hpp"

typedef BYTE E_graphicsScreenType ;
#define GRAPHICS_SCREEN_TYPE_8BIT       0
#define GRAPHICS_SCREEN_TYPE_16BIT      1
#define GRAPHICS_SCREEN_TYPE_24BIT      2
#define GRAPHICS_SCREEN_TYPE_UNKNOWN    3

typedef WORD T_graphicsObjectHandle ;
#define GRAPHICS_OBJECT_HANDLE_BAD      0xFFFF

#define MAX_GRAPHICS_OBJECTS     100

typedef union {
    CGraphicsObject *p_object ;
    T_graphicsObjectHandle index ;
} T_objectOrIndex ;

class CGraphicsScreen : public C_debugObject
{
    protected:
        WORD width ;
        WORD height ;
        HWND window ;
        E_graphicsScreenType type ;
        LPDIRECTDRAW directDraw ;
        LPDIRECTDRAWSURFACE primarySurface ;
        LPDIRECTDRAWSURFACE secondarySurface ;
        LPDIRECTDRAWCLIPPER clipper ;
        LPDIRECTDRAWCLIPPER secondaryClipper ;
        PALETTEENTRY palette[256] ;
        LPDIRECTDRAWPALETTE directDrawPalette ;

        T_objectOrIndex listObjects[MAX_GRAPHICS_OBJECTS] ;
        T_graphicsObjectHandle numObjects ;
        T_graphicsObjectHandle lastObject ;
        T_graphicsObjectHandle freeObject ;
        BOOL clearScreenOnFlip ;

        T_graphicsObjectHandle allocObjectSlot(void) ;
        void deallocObjectSlot(WORD objectIndex) ;

        CBmpGraphic *p_backScreen ;
    public:
        CGraphicsScreen(
            HWND windowHandle,
            WORD width,
            WORD height,
            E_graphicsScreenType type) ;
        ~CGraphicsScreen() ;
        void Flip(int mode) ;
        HDC GetDC(void) ;
        void ReleaseDC(HDC dc) ;
        T_graphicsObjectHandle createGraphicsObject(char *p_filename) ;
        T_graphicsObjectHandle createGraphicsObject(CBmpGraphic *p_bmp) ;
        void destroyGraphicsObject(T_graphicsObjectHandle gObject) ;
        void drawObject(
                 T_graphicsObjectHandle gObject,
                 int x,
                 int y) ;
        void drawObjectRotated(
                 T_graphicsObjectHandle srcObject,
                 T_graphicsObjectHandle destObject,
                 WORD angle) ;
        void drawObjectClipped(
                 T_graphicsObjectHandle gObject,
                 int x,
                 int y,
                 int clipLeft,
                 int clipTop,
                 int clipRight,
                 int clipBottom) ;
        void getGraphicsObjectSize(
                 T_graphicsObjectHandle gObject,
                 WORD *p_width,
                 WORD *p_height) ;
        void setGraphicsObjectTransparency(
                 T_graphicsObjectHandle gObject,
                 BOOL trans) ;
        void setGraphicsObjectBMP(
                 T_graphicsObjectHandle gObject,
                 CBmpGraphic *p_bmp) ;
        BOOL validateGraphicsObjectHandle(T_graphicsObjectHandle gObject) ;
        void setPalette(LPRGBQUAD p_newPalette) ;
        void drawBack(void) ;
        void clear(void) ;
#ifndef NDEBUG
        void Test(void) ;
#else
        void Test(void)   {}
#endif
        void getClientRect(RECT *p_rect) ;
        CBmpGraphic *getBack(void)
            { return p_backScreen ; }
} ;

#endif

/***********************************************/
/*  END OF FILE: CGSCREEN.H                    */
/***********************************************/
