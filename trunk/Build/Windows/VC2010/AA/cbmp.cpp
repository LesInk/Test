/***********************************************/
/*  FILE: CBMP.C                               */
/***********************************************/

#include <windowsx.h>
#include <math.h>
#include "cbmp.hpp"
#include "crawfile.hpp" 

/*------------------------------------------------------------------------*/
CBmpGraphic::CBmpGraphic()
{
    _debug("CBmpGraphic::CBmpGraphic()") ;

    p_header = NULL ;
    p_bitmapInfo = NULL ;
    p_rawBitmap = NULL ;
    p_palette = NULL ;
    palfix = FALSE ;
}

/*------------------------------------------------------------------------*/
CBmpGraphic::CBmpGraphic(char *p_filename)
{
    _debug("CBmpGraphic::CBmpGraphic(filename)") ;

    p_header = NULL ;
    p_bitmapInfo = NULL ;
    p_rawBitmap = NULL ;
    p_palette = NULL ;
    palfix = FALSE ;

    load(p_filename) ;
}

/*------------------------------------------------------------------------*/
CBmpGraphic::CBmpGraphic(WORD sizeX, WORD sizeY)
{
    _debug("CBmpGraphic::CBmpGraphic(sizeX, sizeY)") ;

    p_header = NULL ;
    p_bitmapInfo = NULL ;
    p_rawBitmap = NULL ;
    p_palette = NULL ;
    palfix = FALSE ;

    create(sizeX, sizeY) ;
}

/*------------------------------------------------------------------------*/
CBmpGraphic::~CBmpGraphic()
{
    _debug("CBmpGraphic::~CBmpGraphic") ;

    /* Remove the related graphics junk. */
    empty() ;
}

/*------------------------------------------------------------------------*/
void CBmpGraphic::empty()
{
    _debug("CBmpGraphic::empty") ;
    _check(this != NULL) ;

    if (p_header)  
        GlobalFreePtr(p_header);

    p_bitmapInfo = NULL ;
    p_header = NULL ;
    p_palette = NULL ;
    p_rawBitmap = NULL ;
    palfix = FALSE ;
}

/*------------------------------------------------------------------------*/
BOOL CBmpGraphic::load(char *p_filename)
{
    CRawFile file ;                  // Access to files 
    BOOL didLoad = FALSE ;           // Return status

    _debug("CBmpGraphic::load") ;

    /* Load in the whole file */
    p_header = (BITMAPFILEHEADER *)file.load(p_filename, NULL) ;
    if (p_header)  {
        /* Check the header for the tell-tale BM signature */
        if (p_header->bfType == 0x4D42)  {
            /* Fixup the pointers to their respective data */
            p_bitmapInfo = (LPBITMAPINFO)(p_header+1) ;
            p_palette = p_bitmapInfo->bmiColors ;
            p_rawBitmap = (VOID *)(((BYTE *)p_header) + p_header->bfOffBits) ;

            /* Got everything we need */
            didLoad = TRUE ;
        } else {
            _check(FALSE) ;

            /* Ooops!  Not a bitmap file */
            GlobalFreePtr(p_header) ;
            p_header = NULL ;
        }
    }

    return didLoad ;
}

/*------------------------------------------------------------------------*/
DWORD CBmpGraphic::numberOfColors()
{
    DWORD numColors = 0 ;

    _debug("CBmpGraphic::numberOfColors") ;
    _check(p_bitmapInfo != NULL) ;
    
    /* If we are given the number of colors, use that value */
    /* else calculate the Power of Two (R) value */
    if (p_bitmapInfo->bmiHeader.biClrUsed != 0)
        numColors = p_bitmapInfo->bmiHeader.biClrUsed ;
    else
        numColors = (1<<(p_bitmapInfo->bmiHeader.biBitCount)) ;

    return numColors ;
}

/*------------------------------------------------------------------------*/
void CBmpGraphic::drawToDC(HDC hDC, int x, int y)
{
    _debug("CBmpGraphic::drawToDC") ;
    _check(this != NULL) ;

    SetStretchBltMode(hDC, COLORONCOLOR) ;
    StretchDIBits(hDC,
                    x,                          // Destination x
                    y,                          // Destination y
                    p_bitmapInfo->bmiHeader.biWidth, // Destination width
                    p_bitmapInfo->bmiHeader.biHeight,// Destination height
                    0,                          // Source x
                    0,                          // Source y
                    p_bitmapInfo->bmiHeader.biWidth, // Source width
                    p_bitmapInfo->bmiHeader.biHeight,// Source height
                    p_rawBitmap,                     // Pointer to bits
                    p_bitmapInfo,                    // BITMAPINFO
                    DIB_RGB_COLORS,             // Options
                    SRCCOPY);                   // Raster operator code (ROP)
}

/*------------------------------------------------------------------------*/
void CBmpGraphic::drawToDCClipped(
                 HDC hDC, 
                 int x, int y, 
                 int clipLeft, int clipTop, 
                 int clipRight, int clipBottom)
{
    _debug("CBmpGraphic::drawToDCClipped") ;
    _check(this != NULL) ;

    int width, height, fullheight ;
    int top, left, bottom, right ;

    width = p_bitmapInfo->bmiHeader.biWidth ;
    fullheight = height = p_bitmapInfo->bmiHeader.biHeight ;

    /* Clip the size */
    if (x + width > clipRight)  {
        right = clipRight - x;
    } else {
        right = width ;
    }

    if (y + height > clipBottom)  {
        bottom = clipBottom - y;
    } else {
        bottom = height ;
    }
    
    if (x < clipLeft)  {
        left = clipLeft-x ;
        x = clipLeft ;
    } else {
        left = 0 ;
    }

    if (y < clipTop)  {
        top = clipTop-y ;
        y = clipTop ;
    } else {
        top = 0 ;
    }

    /* Determine the width and height now that the picture is clipped */
    width = right - left ;
    if (width < 0)
        return ;
    height = bottom - top  ;
    if (height < 0)
        return ;

    StretchDIBits(hDC,
                    x,                          // Destination x
                    y,                          // Destination y
                    width,                      // Destination width
                    height,                     // Destination height
                    left,                       // Source x
                    fullheight - top - height,  // Source y
                    width,                      // Source width
                    height,                     // Source height
                    p_rawBitmap,                // Pointer to bits
                    p_bitmapInfo,               // BITMAPINFO
                    (palfix)?
                        DIB_PAL_COLORS:
                        DIB_RGB_COLORS,         // Options
                    SRCCOPY);                   // Raster operator code (ROP)
}
/*------------------------------------------------------------------------*/
void CBmpGraphic::fixUpPalette(void)
{
    _debug("CBmpGraphic::fixUpPalette") ;
    _check(this != NULL) ;
    
#ifdef FAST_BMP_COLORING
    p_bitmapInfo->bmiHeader.biClrUsed = 1 ;
#endif

    /* Convert the rgb palette into an indexed palette after the first */
    /* slow drawing */
    if (palfix == FALSE)  {
        unsigned short *p_index ;
        p_index = (unsigned short *)(p_bitmapInfo->bmiColors) ;
        for (unsigned short i=0; i<256; i++)
            p_index[i] = i ;
        palfix = TRUE ;
    }
}

/*------------------------------------------------------------------------*/
BOOL CBmpGraphic::create(WORD sizeX, WORD sizeY)
{
    BOOL didCreate = FALSE ;
    int correctX ;

    _debug("CBmpGraphic::create") ;
    _check(this != NULL) ;

    /* Make sure the graphic is empty. */
    empty() ;

    correctX = (sizeX+3)&0xFFFC ;
    /* Load in the whole file */
    p_header = (BITMAPFILEHEADER *)GlobalAllocPtr(GMEM_MOVEABLE, 
                   sizeof(BITMAPFILEHEADER) +
                   sizeof(BITMAPINFO) +
                   256 * sizeof(RGBQUAD) +
                   (LONG)correctX * (LONG)sizeY) ;
#ifndef NDEBUG
    memset(p_header, 0x80, (sizeof(BITMAPFILEHEADER) +
                   sizeof(BITMAPINFO) +
                   256 * sizeof(RGBQUAD) +
                   (LONG)correctX * (LONG)sizeY)) ;
#endif
    p_header->bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFO) + 256 * sizeof(RGBQUAD) ;

    if (p_header)  {
        /* Fixup the pointers to their respective data */
        p_bitmapInfo = (LPBITMAPINFO)(p_header+1) ;
        p_palette = p_bitmapInfo->bmiColors ;
        p_rawBitmap = (VOID *)(((BYTE *)p_header) + p_header->bfOffBits) ;

        /* Got everything we need */
        didCreate = TRUE ;

        p_bitmapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER) ;
        p_bitmapInfo->bmiHeader.biWidth = sizeX ;
        p_bitmapInfo->bmiHeader.biHeight = sizeY ;
        p_bitmapInfo->bmiHeader.biPlanes = 1 ;
        p_bitmapInfo->bmiHeader.biBitCount = 8 ;   /* 256 color bitmap */
        p_bitmapInfo->bmiHeader.biCompression = 0 ;  /* No compression */
        p_bitmapInfo->bmiHeader.biSizeImage = correctX * sizeY ;
        p_bitmapInfo->bmiHeader.biXPelsPerMeter = 100 ;
        p_bitmapInfo->bmiHeader.biYPelsPerMeter = 100 ;
        p_bitmapInfo->bmiHeader.biClrUsed = 256 ;
        p_bitmapInfo->bmiHeader.biClrImportant = 0 ;

        /* NOTE:  We don't clear the bitmap's picture because we don't need to */
        /*        and we can keep the speed higher */

 /*
 for (int x=0; x<sizeX; x++)  {
            for (int y=0; y<sizeY; y++)  {
                ((char *)p_rawBitmap)[y*correctX+x] = y ;
            }
        }
 */
    }

    return didCreate ;
}

/*------------------------------------------------------------------------*/
void CBmpGraphic::forcePalette(LPRGBQUAD p_list)
{
    memcpy(p_palette, p_list, sizeof(RGBQUAD) * 256) ;
}

/*------------------------------------------------------------------------*/
#define TWO_PI (2*3.1415926535)

void CBmpGraphic::drawRotated(CBmpGraphic *p_to, WORD angle)
{
    _debug("CBmpGraphic::drawRotated") ;

    /* Get the size of the bitmaps */
    DWORD fromSizeX = p_bitmapInfo->bmiHeader.biWidth ;
    DWORD fromSizeY = p_bitmapInfo->bmiHeader.biHeight ;
    DWORD toSizeX = p_to->p_bitmapInfo->bmiHeader.biWidth ;
    DWORD toSizeY = p_to->p_bitmapInfo->bmiHeader.biHeight ;

    /* Calculate the "correct X widths" for both bitmaps */
    DWORD correctFromX = (fromSizeX+3)&0xFFFC ;
    DWORD correctToX = (toSizeX+3)&0xFFFC ;

    /* Calculate the center x&y for the from bitmap (in 16.16 format) */
    LONG centerFromX = ((LONG)fromSizeX >> 1) << 16 ;
    LONG centerFromY = ((LONG)fromSizeY >> 1) << 16 ;

    /* Calculate the x&y center for the to bitmap */
    LONG centerToX = toSizeX >> 1 ;
    LONG centerToY = toSizeY >> 1 ;

    /* Calculate the radian value */
    double radian = TWO_PI * ((double)angle) / 65536.0 ;

    /* Calculate the rotation vector */
    LONG vectorX = (LONG)(65536*cos(radian)) ;
    LONG vectorY = (LONG)(65536*sin(radian)) ;

    /* Calculate the perpendicular vector */
    LONG vectorPerpX = -vectorY ;
    LONG vectorPerpY = vectorX ;

    /* Determine the starting upper left points (in destination graphics) */
    LONG startPosX = 0x8000 + vectorX * -centerToX + vectorPerpX * -centerToX ;
    LONG startPosY = 0x8000 + vectorY * -centerToY + vectorPerpY * -centerToY ;

    /* Determine pointers into the raw data */
    BYTE *p_from = (BYTE *)p_rawBitmap ;
    BYTE *p_toStart = (BYTE *)(p_to->p_rawBitmap) ;

    for (DWORD y=0; y<toSizeY; y++, p_toStart += correctToX)  {
        /* Start at the left of the row. */
        LONG posX = startPosX ;
        LONG posY = startPosY ;
        BYTE *p_to = p_toStart ;
        for (DWORD x=0; x<toSizeX; x++)  {
            /* Copy a pixel if it is in the view */
            DWORD posIntX = (posX + centerFromX) >> 16 ;
            DWORD posIntY = (posY + centerFromY) >> 16 ;

            if ((posIntX < fromSizeX) && (posIntY < fromSizeY))  {
                *p_to = p_from[posIntY * correctFromX + posIntX] ;
            } else {
                *p_to = p_from[0] ;
            }

            /* Move to the next pixel */
            posX += vectorX ;
            posY += vectorY ;

            /* Next pixel */
            p_to++ ;
        }

        /* Move to the next row */
        startPosX += vectorPerpX ;
        startPosY += vectorPerpY ;
    }
}

/*------------------------------------------------------------------------*/
void BlitTrans(
        BYTE *p_to,
        BYTE *p_from,
        WORD width)
{
    BYTE c ;
    while(width--)  {
        c = *(p_from++) ;
        if (c)
            *p_to = c ;
        p_to++ ;
    }
}

/*------------------------------------------------------------------------*/
void BlitTransSolid(
        BYTE *p_to,
        BYTE *p_from,
        WORD width,
        BYTE solidColor)
{
    BYTE c ;
    while(width--)  {
        c = *(p_from++) ;
        if (c)
            *p_to = solidColor ;
            
        p_to++ ;
    }
}
/*------------------------------------------------------------------------*/
void CBmpGraphic::blitFast(
         int x, int y, 
         CBmpGraphic *p_srcBmp, 
         RECT *p_srcRect, 
         BOOL isTrans)
{
    _debug("CBmpGraphic::blitFast") ;
    _check(p_srcBmp != NULL) ;
    _check(this != NULL) ;

    /* Don't do if no source bitmap */
    if (!p_srcBmp)
        return ;

    /* If the source rectangle is NULL, do the whole bitmap */
    RECT srcRect ;
    if (p_srcRect == NULL)  {
        srcRect.left = 0 ;
        srcRect.top = 0 ;
        srcRect.right = p_srcBmp->getWidth() ;
        srcRect.bottom = p_srcBmp->getHeight() ;
        p_srcRect = &srcRect ;

        /* NOTE:  If NULL is passed, there is no need to clip */
        /* the bitmap to its source bitmap */
    } else {
        /* Clip the source rectangle to the source bitmap */
        if (p_srcRect->left < 0)  {
            x += -p_srcRect->left ;
            p_srcRect->left = 0 ;
        }
        if (p_srcRect->top < 0)  {
            y += -p_srcRect->top ;
            p_srcRect->top = 0 ;
        }
        if (p_srcRect->right > p_srcBmp->getWidth())
            p_srcRect->right = p_srcBmp->getWidth() ;
        if (p_srcRect->bottom > p_srcBmp->getHeight())
            p_srcRect->bottom = p_srcBmp->getHeight() ;
    }

    /* Calculate the edge of the destination */
    LONG thisWidth = getWidth() ;
    LONG left = x ;
    if (left >= thisWidth)
        return ;

    LONG right = left + p_srcRect->right - p_srcRect->left ;
    if (right < 0)
        return ;

    LONG thisHeight = getHeight() ;
    LONG top = y ;
    if (top >= thisHeight)
        return ;

    LONG bottom = top + p_srcRect->bottom - p_srcRect->top ;
    if (bottom < 0)
        return ;

    /* Clip that destination */
    LONG srcX = p_srcRect->left ;
    if (left < 0)  {
        srcX += -left ;
        left = 0 ;
    }

    LONG srcY = p_srcRect->top ;
    if (top < 0)  {
        srcY += -top ;
        top = 0 ;
    }

    if (right > thisWidth)
        right = thisWidth ;

    if (bottom > thisHeight)
        bottom = thisHeight ;

    /* Determine the width and height to blit */
    /* (stop if either is negative or zero) */
    LONG blitWidth = right - left ;
    if (blitWidth <= 0)
        return ;

    LONG blitHeight = bottom - top ;
    if (blitHeight <= 0)
        return ;

    /* OK, everyone is clipped and legal.  All assumptions */
    /* in place. */
    /* Get the corrected size X's */
    LONG srcCorrectX = p_srcBmp->getCorrectedWidth() ;
    LONG destCorrectX = this->getCorrectedWidth() ;

    BYTE *p_from = 
            ((BYTE *)p_srcBmp->p_rawBitmap) + 
                (p_srcBmp->getHeight() - p_srcRect->top - 1) * srcCorrectX + 
                    p_srcRect->left;
    BYTE *p_to = 
            ((BYTE *)p_rawBitmap) +
                (getHeight() - top - 1) * destCorrectX +
                    left ;

    if (isTrans)  {
        /* Transparent blit */
        while (blitHeight)  {
            BlitTrans(p_to, p_from, (WORD)blitWidth) ;
            p_from -= srcCorrectX ;
            p_to -= destCorrectX ;
            blitHeight-- ;
        }
    } else {
        /* Solid blit */
        while (blitHeight)  {
            memcpy(p_to, p_from, blitWidth) ;
            p_from -= srcCorrectX ;
            p_to -= destCorrectX ;
            blitHeight-- ;
        }
    }
}

/*------------------------------------------------------------------------*/
BOOL CBmpGraphic::save(char *p_filename)
{
    CRawFile file ;                  // Access to files 
    BOOL didLoad = FALSE ;           // Return status

    _debug("CBmpGraphic::save") ;

    p_header->bfType = 0x4D42 ;   /* tag as BMP */
    p_header->bfSize = (sizeof(BITMAPFILEHEADER) +
                       sizeof(BITMAPINFO) +
                       256 * sizeof(RGBQUAD) +
                        (LONG)getCorrectedWidth() * (LONG)getHeight()) ;
    p_header->bfReserved1 = 0 ;
    p_header->bfReserved2 = 0 ;
    p_header->bfOffBits = ((BYTE *)p_rawBitmap) - ((BYTE *)p_header) ;
    p_bitmapInfo->bmiHeader.biSize = sizeof(p_bitmapInfo->bmiHeader) ;
    p_bitmapInfo->bmiHeader.biWidth = getWidth() ;
    p_bitmapInfo->bmiHeader.biHeight = getHeight() ;
    p_bitmapInfo->bmiHeader.biPlanes = 1 ;
    p_bitmapInfo->bmiHeader.biBitCount = 8 ;
    p_bitmapInfo->bmiHeader.biCompression = BI_RGB ;
    p_bitmapInfo->bmiHeader.biSizeImage = 0 ;
    p_bitmapInfo->bmiHeader.biXPelsPerMeter = 1000 ;
    p_bitmapInfo->bmiHeader.biYPelsPerMeter = 1000 ;
    p_bitmapInfo->bmiHeader.biClrUsed = 256 ;
    p_bitmapInfo->bmiHeader.biClrImportant = 256 ;

    /* save out the whole file */
    return file.save(p_filename, p_header, p_header->bfSize) ;
}

/*------------------------------------------------------------------------*/
void CBmpGraphic::clear(BYTE value)
{
    _debug("CBmpGraphic::clear") ;
    _check(this != NULL) ;
    _check(p_rawBitmap != NULL) ;

    if (p_rawBitmap)  
        memset(p_rawBitmap, value, getCorrectedWidth() * getHeight()) ;
}

/*------------------------------------------------------------------------*/
void CBmpGraphic::blitFastSolid(
         int x, int y, 
         CBmpGraphic *p_srcBmp, 
         RECT *p_srcRect, 
         BYTE solidColor)
{
    _debug("CBmpGraphic::blitFastSolid") ;
    _check(p_srcBmp != NULL) ;
    _check(this != NULL) ;

    /* Don't do if no source bitmap */
    if (!p_srcBmp)
        return ;

    /* If the source rectangle is NULL, do the whole bitmap */
    RECT srcRect ;
    if (p_srcRect == NULL)  {
        srcRect.left = 0 ;
        srcRect.top = 0 ;
        srcRect.right = p_srcBmp->getWidth() ;
        srcRect.bottom = p_srcBmp->getHeight() ;
        p_srcRect = &srcRect ;

        /* NOTE:  If NULL is passed, there is no need to clip */
        /* the bitmap to its source bitmap */
    } else {
        /* Clip the source rectangle to the source bitmap */
        if (p_srcRect->left < 0)  {
            x += -p_srcRect->left ;
            p_srcRect->left = 0 ;
        }
        if (p_srcRect->top < 0)  {
            y += -p_srcRect->top ;
            p_srcRect->top = 0 ;
        }
        if (p_srcRect->right > p_srcBmp->getWidth())
            p_srcRect->right = p_srcBmp->getWidth() ;
        if (p_srcRect->bottom > p_srcBmp->getHeight())
            p_srcRect->bottom = p_srcBmp->getHeight() ;
    }

    /* Calculate the edge of the destination */
    LONG thisWidth = getWidth() ;
    LONG left = x ;
    if (left >= thisWidth)
        return ;

    LONG right = left + p_srcRect->right - p_srcRect->left ;
    if (right < 0)
        return ;

    LONG thisHeight = getHeight() ;
    LONG top = y ;
    if (top >= thisHeight)
        return ;

    LONG bottom = top + p_srcRect->bottom - p_srcRect->top ;
    if (bottom < 0)
        return ;

    /* Clip that destination */
    LONG srcX = p_srcRect->left ;
    if (left < 0)  {
        srcX += -left ;
        left = 0 ;
    }

    LONG srcY = p_srcRect->top ;
    if (top < 0)  {
        srcY += -top ;
        top = 0 ;
    }

    if (right > thisWidth)
        right = thisWidth ;

    if (bottom > thisHeight)
        bottom = thisHeight ;

    /* Determine the width and height to blit */
    /* (stop if either is negative or zero) */
    LONG blitWidth = right - left ;
    if (blitWidth <= 0)
        return ;

    LONG blitHeight = bottom - top ;
    if (blitHeight <= 0)
        return ;

    /* OK, everyone is clipped and legal.  All assumptions */
    /* in place. */
    /* Get the corrected size X's */
    LONG srcCorrectX = p_srcBmp->getCorrectedWidth() ;
    LONG destCorrectX = this->getCorrectedWidth() ;

    BYTE *p_from = 
            ((BYTE *)p_srcBmp->p_rawBitmap) + 
                (p_srcBmp->getHeight() - p_srcRect->top - 1) * srcCorrectX + 
                    p_srcRect->left;
    BYTE *p_to = 
            ((BYTE *)p_rawBitmap) +
                (getHeight() - top - 1) * destCorrectX +
                    left ;

    /* Transparent blit with solid */
    while (blitHeight)  {
        BlitTransSolid(
            p_to, 
            p_from, 
            (WORD)blitWidth, 
            solidColor) ;
        p_from -= srcCorrectX ;
        p_to -= destCorrectX ;
        blitHeight-- ;
    }
}

/*------------------------------------------------------------------------*/
/* NOTE:  NOT TESTED */
/* NOTE:  This routine assumes the same palette and a 256 bitmap */
BOOL CBmpGraphic::compare(CBmpGraphic *p_otherBMP)
{
    _debug("CBmpGraphic::compare(BMP)") ;
    _check(this != NULL) ;
    _check(p_header != NULL) ;
    _check(p_otherBMP != NULL) ;
    _check(p_otherBMP->p_header != NULL) ;
    DWORD totalSize ;

    if ((p_otherBMP->getWidth() == getWidth()) &&
        (p_otherBMP->getHeight() == getHeight()))  {
        totalSize = ((DWORD)getWidth()) * ((DWORD)getHeight()) ;
        /* Compare the actual bytes of data in one quick command. */
        if (memcmp(p_otherBMP->getRawData(), getRawData(), totalSize) == 0)
            return TRUE ;
    }

    /* One or more tests failed, not the same. */
    return FALSE ;
}

/*------------------------------------------------------------------------*/
/* NOTE:  NOT TESTED */
/* NOTE:  This routine assumes the same palette and a 256 bitmap */
BOOL CBmpGraphic::compare(CBmpGraphic *p_otherBMP, RECT *p_rectFrom)
{
    _debug("CBmpGraphic::compare(BMP)") ;
    _check(this != NULL) ;
    _check(p_header != NULL) ;
    _check(p_otherBMP != NULL) ;
    _check(p_otherBMP->p_header != NULL) ;

    LONG width ;
    width = p_rectFrom->right - p_rectFrom->left ;
    if (width <= 0)
        return FALSE ;

    LONG height ;
    height = p_rectFrom->bottom - p_rectFrom->top ;
    if (height <= 0)
        return FALSE ;

    BYTE *p_from = (BYTE *)p_otherBMP->getRawData() ;
    BYTE *p_to = (BYTE *)this->getRawData() ;

    LONG correctFromX = p_otherBMP->getCorrectedWidth() ;
    LONG correctToX = this->getCorrectedWidth() ;

    /* Offset the pointer to the correct position */
    p_from += (p_otherBMP->getHeight() - p_rectFrom->top - 1) * 
                  correctFromX +
                      p_rectFrom->left ;
    p_to += (this->getHeight() - 1) * correctToX ;

    for (LONG i=0; i<height; i++)  {
        /* Compare line by line the tiles.  At any point there is a difference, */
        /* just quit and get out */
        if (memcmp(p_from, p_to, width) != 0)
            return FALSE ;
        p_from -= correctFromX ;
        p_to -= correctToX ;
    }

    /* If we have made it to here, all the lines are the same. */
    return TRUE ;
}

/*------------------------------------------------------------------------*/
/* NOTE:  NOT TESTED */
/* NOTE:  This routine assumes the same palette and a 256 bitmap */
BOOL CBmpGraphic::compare(RECT *p_rectTo, CBmpGraphic *p_otherBMP, RECT *p_rectFrom)
{
    _debug("CBmpGraphic::compare(BMP)") ;
    _check(this != NULL) ;
    _check(p_header != NULL) ;
    _check(p_otherBMP != NULL) ;
    _check(p_otherBMP->p_header != NULL) ;

    LONG width = p_rectFrom->right - p_rectFrom->left ;
    if (width <= 0)
        return FALSE ;

    LONG height = p_rectFrom->bottom - p_rectFrom->top ;
    if (height <= 0)
        return FALSE ;

    /* If the two sizes don't match, there is nothing to compare */
    if (((p_rectTo->right - p_rectTo->left) != width) ||
        ((p_rectTo->bottom - p_rectTo->top) != height))
        return FALSE ;

    BYTE *p_from = (BYTE *)p_otherBMP->getRawData() ;
    BYTE *p_to = (BYTE *)this->getRawData() ;

    LONG correctFromX = p_otherBMP->getCorrectedWidth() ;
    LONG correctToX = this->getCorrectedWidth() ;

    /* Offset the pointer to the correct position */
    p_from += (p_otherBMP->getHeight() - p_rectFrom->top - 1) * 
                  correctFromX +
                      p_rectFrom->left ;
    p_to += (this->getHeight() - p_rectTo->top - 1) * 
                  correctToX +
                      p_rectTo->left ;

    for (LONG i=0; i<height; i++)  {
        /* Compare line by line the tiles.  At any point there is a difference, */
        /* just quit and get out */
        if (memcmp(p_from, p_to, width) != 0)
            return FALSE ;
        p_from -= correctFromX ;
        p_to -= correctToX ;
    }

    /* If we have made it to here, all the lines are the same. */
    return TRUE ;
}

/***********************************************/
/*  END OF FILE: CBMP.C                        */
/***********************************************/
