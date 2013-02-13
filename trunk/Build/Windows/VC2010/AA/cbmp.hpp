/***********************************************/
/*  FILE: CBMP.H                               */
/***********************************************/

#ifndef _CBMP_H_
#define _CBMP_H_

#include <windows.h>
#include "options.hpp"
#include "debug.hpp"

class CBmpGraphic : public C_debugObject
{
    protected:
        BITMAPFILEHEADER *p_header ;    // Pointer to the header 
        LPBITMAPINFO p_bitmapInfo;      // Pointer to BITMAPINFO struct
	    LPRGBQUAD    p_palette;			// Pointer to RGBQUAD
        VOID*        p_rawBitmap;       // Pointer to the bits
        BOOL         palfix ;           // Flag telling if palette is fixed(1)
    public:
        CBmpGraphic() ;
        CBmpGraphic(char *p_filename) ;
        CBmpGraphic(WORD sizeX, WORD sizeY) ;
        ~CBmpGraphic() ;
        BOOL load(char *p_filename) ;
        BOOL save(char *p_filename) ;
        DWORD numberOfColors() ;
        void drawToDC(HDC hDC, int x, int y) ;
        void fixUpPalette(void) ;
        BOOL create(WORD sizeX, WORD sizeY) ;
        void empty(void) ;
        void drawToDCClipped(
                 HDC hDC, 
                 int x, int y, 
                 int clipLeft, int clipTop, 
                 int clipRight, int clipBottom) ;
        void forcePalette(LPRGBQUAD p_list) ;
        void drawRotated(CBmpGraphic *p_to, WORD angle) ;
        void blitFast(int x, int y, CBmpGraphic *p_bmp, RECT *p_srcRect, BOOL isTrans) ;
        void blitFastSolid(
                 int x, 
                 int y, 
                 CBmpGraphic *p_bmp, 
                 RECT *p_srcRect, 
                 BYTE solidColor) ;
        void clear(BYTE value) ;
        BOOL compare(CBmpGraphic *p_otherBMP) ;
        BOOL compare(CBmpGraphic *p_otherBMP, RECT *p_from) ;
        BOOL compare(RECT *p_to, CBmpGraphic *p_otherBMP, RECT *p_from) ;

        /* Macro members */
        BOOL isEmpty(void)  
            { return ((p_header == NULL)?TRUE:FALSE) ; }
        LONG getWidth(void)  
            { return p_bitmapInfo->bmiHeader.biWidth; }
        LONG getCorrectedWidth(void)  
            { return (p_bitmapInfo->bmiHeader.biWidth+3)&0xFFFFFFFC; }
        LONG getHeight(void) 
            { return p_bitmapInfo->bmiHeader.biHeight; } 
        LPRGBQUAD getPalette(void)
            { return p_palette ; }
        void *getRawData(void)
            { return p_rawBitmap ; }
} ;

#endif

/* Other notes: */
/* (1) The term 'fixed' means that a bitmap's palette has been changed from RGB */
/*     values to a list of 16 bit words that lookup into the logical palette. */
/*     This technique assumes that the palette of the given bitmap exactly matches */
/*     the logical palette.  In truth, this makes the bitmap drawing faster. */

/***********************************************/
/*  END OF FILE: CBMP.H                        */
/***********************************************/
