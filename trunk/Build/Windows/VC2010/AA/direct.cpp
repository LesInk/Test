#include "standard.h"
#include "cgscreen.hpp" 

static CGraphicsScreen *G_screen = NULL ;
extern HWND G_windowHandle ;

#ifdef __cplusplus
extern "C" {
#endif

static T_buttonClick G_mouseButton = 0 ;
static T_word16 G_mouseX = 0 ;
static T_word16 G_mouseY = 0 ;

T_void DirectMouseSet(T_word16 newX, T_word16 newY)
{
    if (newX > 320)
        newX = 320  ;
    if (newY > 200)
        newY = 200 ;
    G_mouseX = newX ;
    G_mouseY = newY ;
}


void OutsideMouseDriverGet(T_word16 *xPos, T_word16 *yPos)
{
    *xPos = G_mouseX ;
    *yPos = G_mouseY ;
}

T_void DirectMouseSetButton(T_buttonClick click)
{
    G_mouseButton = click ;
}

T_buttonClick DirectMouseGetButton(T_void)
{
    return G_mouseButton ;
}

#if 0
extern void KeyboardUpdate(E_Boolean updateBuffers) ;

void _cdecl DirectDrawOn(void)
{
    G_screen = new CGraphicsScreen(
                       G_windowHandle,
                       320,
                       200,
                       GRAPHICS_SCREEN_TYPE_8BIT) ;
}

void _cdecl DirectDrawOff(void)
{
    delete G_screen ;
    G_screen = NULL ;
}

static CBmpGraphic *G_directBmp = NULL ;

extern int WindowsUpdateMessages(void) ;
void _cdecl WindowsUpdate(char *p_screen, char *palette)
{
    char *p_data ;
    CBmpGraphic *p_pal;
    HDC dc ;
    int i;
    RGBQUAD newPal[256] ;
    T_byte8 *p ;
    E_Boolean isNew ;

    if (!G_directBmp)  {
        G_directBmp = new CBmpGraphic(320, 200) ;
        p_pal = new CBmpGraphic("pal.bmp") ;
        G_screen->setPalette(p_pal->getPalette()) ;
        G_directBmp->forcePalette(p_pal->getPalette()) ;
        G_directBmp->fixUpPalette() ;
        delete p_pal ;
        isNew = TRUE ;
    } else {
        isNew = FALSE;
    }
    p = (T_byte8 *)palette ;
    for (i=0; i<256; i++)  {
        newPal[i].rgbRed = (p[0]&0x3F)<<2 ;
        newPal[i].rgbGreen = (p[1]&0x3F)<<2 ;
        newPal[i].rgbBlue = (p[2]&0x3F)<<2 ;
/*
        newPal[i].rgbRed = i ;
        newPal[i].rgbGreen = i ;
        newPal[i].rgbBlue = i ;
*/
        newPal[i].rgbReserved = 0 ;
        p+=3 ;
    }
//    if (isNew)
        G_directBmp->forcePalette(newPal) ;
    G_screen->setPalette(newPal) ;
    p_data = (char *)G_directBmp->getRawData() ;
    p_data += 199*320 ;
    for (i=0; i<200; i++)  {
        memcpy(p_data, p_screen, 320) ;
        p_data -= 320 ;
        p_screen += 320 ;
    }
    if ((G_screen) && ((dc = G_screen->GetDC()) != NULL))  {
//        RealizePalette(dc) ;
        G_directBmp->drawToDC(dc, 0, 0) ;
        G_screen->ReleaseDC(dc) ;

        G_screen->Flip(1) ;
    }
    WindowsUpdateMessages() ;
    KeyboardUpdate(TRUE) ;
}
#else
void _cdecl WindowsUpdate(char *p_screen, char *palette)
{
}
#endif

#ifdef __cplusplus
}
#endif

