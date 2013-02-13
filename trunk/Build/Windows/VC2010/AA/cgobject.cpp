/***********************************************/
/*  FILE: CGOBJECT.CPP                         */
/***********************************************/

#include <windowsx.h>
#include <math.h>
#include "cgobject.hpp"

/*------------------------------------------------------------------------*/
CGraphicsObject::CGraphicsObject()
{
    _debug("CGraphicsObject::CGraphicsObject()") ;

    p_bmp = NULL ;
    p_surface = NULL ;
    isTransparent = FALSE ;
}

/*------------------------------------------------------------------------*/
CGraphicsObject::CGraphicsObject(
                     LPDIRECTDRAW directDraw, 
                     char *p_filenameBMP)
{
    _debug("CGraphicsObject::CGraphicsObject(dd, filename)") ;

    p_surface = NULL ;
    p_bmp = new CBmpGraphic(p_filenameBMP) ;
    isTransparent = FALSE ;
    _check(p_bmp != NULL) ;

    if (!p_bmp->isEmpty())  {
        createSurface(directDraw) ;
        render() ;
    }
}

/*------------------------------------------------------------------------*/
CGraphicsObject::CGraphicsObject(
                     LPDIRECTDRAW directDraw, 
                     CBmpGraphic *p_bmpGraphic)
{
    _debug("CGraphicsObject::CGraphicsObject(dd, BMP)") ;
    _check(p_bmpGraphic != NULL) ;

    p_surface = NULL ;
    p_bmp = p_bmpGraphic ;
    isTransparent = FALSE ;

    createSurface(directDraw) ;
    render() ;
}

/*------------------------------------------------------------------------*/
CGraphicsObject::CGraphicsObject(char *p_filenameBMP)
{
    _debug("CGraphicsObject::CGraphicsObject(filename)") ;

    p_surface = NULL ;
    p_bmp = new CBmpGraphic(p_filenameBMP) ;
    isTransparent = FALSE ;
    _check(p_bmp != NULL) ;
}

/*------------------------------------------------------------------------*/
CGraphicsObject::CGraphicsObject(CBmpGraphic *p_bmpGraphic)
{
    _debug("CGraphicsObject::CGraphicsObject(BMP)") ;
    _check(p_bmpGraphic != NULL) ;

    p_surface = NULL ;
    p_bmp = p_bmpGraphic ;
    isTransparent = FALSE ;
}

/*------------------------------------------------------------------------*/
CGraphicsObject::~CGraphicsObject()
{
    _debug("CGraphicsObject::~CGraphicsObject") ;

    if (p_bmp)  {
        /* Delete the graphic */
        delete p_bmp ;
        p_bmp = NULL ;
    }

    if (p_surface)  {
        /* Let go of the surface */
        p_surface->Release() ;
        p_surface = NULL ;
    }
}
/*------------------------------------------------------------------------*/
BOOL CGraphicsObject::createSurface(LPDIRECTDRAW directDraw)
{
    _debug("CGraphicsObject::createSurface") ;
    _check(p_bmp != NULL) ;
    _check(p_surface == NULL) ;

    DDSURFACEDESC surfaceDesc ;
    HRESULT ddResult ;
    
    /* Create a surface of the given size */
    memset(&surfaceDesc, 0, sizeof(surfaceDesc)) ;
    surfaceDesc.dwSize = sizeof(surfaceDesc) ;
    surfaceDesc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH ;
    surfaceDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN ;
    surfaceDesc.dwWidth = p_bmp->getWidth() ;
    surfaceDesc.dwHeight = p_bmp->getHeight() ; 
    ddResult = directDraw->CreateSurface(&surfaceDesc, &p_surface, NULL) ;

    /* If that worked, say so. */
    if (ddResult == DD_OK)  {
        return TRUE ;
    }

    /* otherwise, make sure the surface is not there and return a */
    /* negative status */
    p_surface = NULL ;

    return FALSE ;
}

/*------------------------------------------------------------------------*/
BOOL CGraphicsObject::isEmpty(void)
{
    _debug("CGraphicsObject::isEmpty") ;

    if (p_bmp == NULL)
        return TRUE ;

    return p_bmp->isEmpty() ;
}

/*------------------------------------------------------------------------*/
BOOL CGraphicsObject::render(void)
{
    BOOL isSuccess = FALSE ;
    HDC deviceContext ;
    DDCOLORKEY colorKey ;
    DDSURFACEDESC surfaceDesc ;
    DWORD color = 0;

    _debug("CGraphicsObject::render") ;
    _check(p_bmp != NULL) ;
    _check(p_surface != NULL) ;

    /* Make sure we only process this one if there are both */
    /* a BMP and a surface */
    if ((p_bmp) && (p_surface))  {
        /* Get a device context and blit to it. */
        if (p_surface->GetDC(&deviceContext) == DD_OK)  {
            /* Draw without transparency */
            SetStretchBltMode(deviceContext, COLORONCOLOR);
            p_bmp->drawToDC(deviceContext, 0, 0) ;
            p_surface->ReleaseDC(deviceContext) ;

            surfaceDesc.dwSize = sizeof(surfaceDesc);
            if (p_surface->Lock(NULL, &surfaceDesc, DDLOCK_WAIT, NULL) == DD_OK)  {
                color = *(DWORD *)surfaceDesc.lpSurface ;
                if (surfaceDesc.ddpfPixelFormat.dwRGBBitCount != 32)
                    color &= (1 << surfaceDesc.ddpfPixelFormat.dwRGBBitCount)-1 ;
                p_surface->Unlock(NULL) ;

                colorKey.dwColorSpaceLowValue = color ;
                colorKey.dwColorSpaceHighValue = color ;
                p_surface->SetColorKey(DDCKEY_SRCBLT, &colorKey) ;
                isSuccess = TRUE ;
            }
        }
    }

    return isSuccess ;
}

/*------------------------------------------------------------------------*/
void CGraphicsObject::draw(LPDIRECTDRAWSURFACE p_destSurface, int x, int y, int limitX, int limitY)
{
//    RECT destRect ;
    int width, height ;
RECT srcRect ;

    _debug("CGraphicsObject::draw") ;
    _check(p_surface != NULL) ;
    _check(p_destSurface != NULL) ;

    width = p_bmp->getWidth() ;
    height = p_bmp->getHeight() ;

//    limitY -= 4 ;
//    limitX -= 4 ;
#if 0
    destRect.left = x ;
    destRect.top = y ;
    destRect.right = x+width ;
    destRect.bottom = y+height ;
#endif

    if ((p_surface) && (p_destSurface))  {
#if 0
        p_destSurface->Blt(
                        &destRect, 
                        p_surface, 
                        NULL,
                        DDBLT_WAIT | ((isTransparent)?DDBLT_KEYSRC:0),
                        NULL) ;
#endif
        if (x + width > limitX)  {
            srcRect.right = limitX - x;
        } else {
            srcRect.right = width ;
        }

        if (y + height > limitY)  {
            srcRect.bottom = limitY - y;
        } else {
            srcRect.bottom = height ;
        }
        
        if (x < 0)  {
            srcRect.left = -x ;
            x = 0 ;
        } else {
            srcRect.left = 0 ;
        }

        if (y < 0)  {
            srcRect.top = -y ;
            y = 0 ;
        } else {
            srcRect.top = 0 ;
        }

#if 0
if ((srcRect.bottom - srcRect.top) < 8)  {
//    srcRect.top = srcRect.bottom - 8 ;
    y  -= 8 ;
    srcRect.bottom = srcRect.top + 8 ;
}
#endif

        p_destSurface->BltFast(x, y,
                  p_surface,
                  &srcRect,
                  DDBLTFAST_WAIT | (isTransparent?DDBLTFAST_SRCCOLORKEY:DDBLTFAST_NOCOLORKEY)) ;
    }
}

/*------------------------------------------------------------------------*/
void CGraphicsObject::drawClipped(
                          LPDIRECTDRAWSURFACE p_destSurface, 
                          int x, int y,
                          int clipLeft, int clipTop,
                          int clipRight, int clipBottom)
{
    int width, height ;
    RECT srcRect ;

    _debug("CGraphicsObject::drawClipped") ;
    _check(p_surface != NULL) ;
    _check(p_destSurface != NULL) ;

    width = p_bmp->getWidth() ;
    height = p_bmp->getHeight() ;

    if ((p_surface) && (p_destSurface))  {
        if (x + width > clipRight)  {
            srcRect.right = clipRight - x;
        } else {
            srcRect.right = width ;
        }

        if (y + height > clipBottom)  {
            srcRect.bottom = clipBottom - y;
        } else {
            srcRect.bottom = height ;
        }
        
        if (x < clipLeft)  {
            srcRect.left = clipLeft-x ;
            x = clipLeft ;
        } else {
            srcRect.left = 0 ;
        }

        if (y < clipTop)  {
            srcRect.top = clipTop-y ;
            y = clipTop ;
        } else {
            srcRect.top = 0 ;
        }
#if 1
        p_destSurface->BltFast(x, y,
                  p_surface,
                  &srcRect,
                  DDBLTFAST_WAIT | (isTransparent?DDBLTFAST_SRCCOLORKEY:DDBLTFAST_NOCOLORKEY)) ;
#else
        RECT dest ;
        static DDBLTFX fx ;

        dest.left = x ;
        dest.top = y ;
        dest.right = srcRect.right - srcRect.left + x ;
        dest.bottom = srcRect.bottom - srcRect.top + y ;
        fx.dwSize = sizeof(fx);
        fx.dwRotationAngle = 0  ;
        p_destSurface->Blt(&dest,
                  p_surface,
                  &srcRect,
                  /* DDBLT_ROTATIONANGLE | */ DDBLTFAST_WAIT | (isTransparent?DDBLTFAST_SRCCOLORKEY:DDBLTFAST_NOCOLORKEY),
                  &fx) ;
#endif
    }
}

/*------------------------------------------------------------------------*/
void CGraphicsObject::getSize(WORD *p_width, WORD *p_height)
{
    _debug("CGraphicsObject::getSize") ;
    _check(p_width != NULL) ;
    _check(p_height != NULL) ;

    *p_width = (WORD)p_bmp->getWidth() ;
    *p_height = (WORD)p_bmp->getHeight() ;
}

/*------------------------------------------------------------------------*/
#define TWO_PI (2*3.1415926535)

void CGraphicsObject::drawRotated(CGraphicsObject *p_to, WORD angle)
{
    DDSURFACEDESC surfaceDescFrom ;
    DDSURFACEDESC surfaceDescTo ;
    HRESULT result ;

    _debug("CBmpGraphic::drawRotated") ;

    surfaceDescFrom.dwSize = sizeof(surfaceDescFrom);
    if ((result = p_surface->Lock(NULL, (LPDDSURFACEDESC)&surfaceDescFrom, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL)) == DD_OK)  {
        surfaceDescTo.dwSize = sizeof(surfaceDescTo);
        if (p_to->p_surface->Lock(NULL, (LPDDSURFACEDESC)&surfaceDescTo, DDLOCK_SURFACEMEMORYPTR| DDLOCK_WAIT, NULL) == DD_OK)  {
            /* Get the size of the bitmaps */
            WORD fromSizeX = (WORD)surfaceDescFrom.dwWidth ;
            WORD fromSizeY = (WORD)surfaceDescFrom.dwHeight ;
            WORD toSizeX = (WORD)surfaceDescTo.dwWidth ;
            WORD toSizeY = (WORD)surfaceDescTo.dwHeight ;

            /* Calculate the "correct X widths" for both bitmaps */
            DWORD correctFromX = surfaceDescFrom.lPitch ;
            DWORD correctToX = surfaceDescTo.lPitch ;

            /* Calculate the center x&y for the from bitmap (in 16.16 format) */
            LONG centerFromX = ((LONG)fromSizeX >> 1) << 16 ;
            LONG centerFromY = ((LONG)fromSizeY >> 1) << 16 ;

            /* Calculate the x&y center for the to bitmap */
            LONG centerToX = (LONG)(toSizeX >> 1) ;
            LONG centerToY = (LONG)(toSizeY >> 1) ;

#if 1
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

            startPosX += centerFromX ;
            startPosY += centerFromY ;

            /* Determine pointers into the raw data */
            BYTE *p_from = (BYTE *)(surfaceDescFrom.lpSurface) ;
            BYTE *p_toStart = (BYTE *)(surfaceDescTo.lpSurface) ;

            for (DWORD y=0; y<toSizeY; y++, p_toStart += correctToX)  {
                /* Start at the left of the row. */
                LONG posX = startPosX;
                LONG posY = startPosY;
#if 1
                BYTE *p_to = p_toStart ;
                DWORD x ;
                for (x=0; x<toSizeX; x++)  {
                    /* Copy a pixel if it is in the view */
                    WORD posIntX = ((WORD *)(&posX))[1] ;
                    WORD posIntY = ((WORD *)(&posY))[1] ;

                    if ((posIntX < fromSizeX) && (posIntY < fromSizeY))  {
                        *p_to = p_from[(DWORD)posIntY * correctFromX + (DWORD)posIntX] ;
                    } else {
                        *p_to = p_from[0] ;
                    }

                    /* Move to the next pixel */
                    posX += vectorX ;
                    posY += vectorY ;

                    /* Next pixel */
                    p_to++ ;
                }
#else
__asm  {
    xor cx, cx
    mov edi, p_from
    mov bl, [edi]
    mov edi, p_toStart
loop1:
    cmp cx, toSizeX
    jae endloop
//    movsx si, word ptr posX[1]
    mov esi, posX
    shr esi, 16
    cmp si, fromSizeX
    jae doblank
//    movsx dx, word ptr posY[1] 
    mov edx, posY
    shr edx, 16
    cmp dx, fromSizeY
    jae doblank
    mov eax, correctFromX
    imul edx
    add eax, p_from
    mov al, byte ptr [esi+eax]
    jmp dowrite
doblank:
    mov al, bl
dowrite:
    mov [edi], al
    inc edi
    mov eax, vectorX ;
    add posX, eax ;
    mov eax, vectorY ;
    add posY, eax ;
    inc cx
    jmp loop1
endloop:
}
#endif
                /* Move to the next row */
                startPosX += vectorPerpX ;
                startPosY += vectorPerpY ;
            }
#else
            /* Determine pointers into the raw data */
#if 0
            BYTE *p_from = (BYTE *)(surfaceDescFrom.lpSurface) ;
            BYTE *p_toStart = (BYTE *)(surfaceDescTo.lpSurface) ;

            for (DWORD y=0; y<toSizeY; y++, p_toStart+=correctToX, p_from+=correctFromX)  {
                memcpy(p_toStart, p_from, toSizeX) ;
            }
#endif
#endif
#if 1
            BYTE color ;
            color = *((BYTE *)(surfaceDescTo.lpSurface)) ;
            DDCOLORKEY colorKey ;
            colorKey.dwColorSpaceLowValue = (DWORD)color ;
            colorKey.dwColorSpaceHighValue = (DWORD)color ;
            p_to->p_surface->SetColorKey(DDCKEY_SRCBLT, &colorKey) ;
#endif
            p_to->p_surface->Unlock(NULL) ;
        }
        p_surface->Unlock(NULL) ;
    }
}

/*------------------------------------------------------------------------*/
void CGraphicsObject::draw(CBmpGraphic *p_dest, int x, int y, int limitX, int limitY)
{
    int width, height ;
    RECT srcRect ;

    _debug("CGraphicsObject::draw") ;
    _check(p_surface != NULL) ;
    _check(p_dest != NULL) ;

    width = p_bmp->getWidth() ;
    height = p_bmp->getHeight() ;

    if ((p_surface) && (p_dest))  {
        if (x + width > limitX)  {
            srcRect.right = limitX - x;
        } else {
            srcRect.right = width ;
        }

        if (y + height > limitY)  {
            srcRect.bottom = limitY - y;
        } else {
            srcRect.bottom = height ;
        }
        
        if (x < 0)  {
            srcRect.left = -x ;
            x = 0 ;
        } else {
            srcRect.left = 0 ;
        }

        if (y < 0)  {
            srcRect.top = -y ;
            y = 0 ;
        } else {
            srcRect.top = 0 ;
        }

        p_dest->blitFast(x, y, p_bmp, &srcRect, isTransparent) ;
    }
}

/*------------------------------------------------------------------------*/
void CGraphicsObject::drawClipped(
                          CBmpGraphic *p_dest, 
                          int x, int y,
                          int clipLeft, int clipTop,
                          int clipRight, int clipBottom)
{
    int width, height ;
    RECT srcRect ;

    _debug("CGraphicsObject::drawClipped") ;
    _check(p_surface != NULL) ;
    _check(p_dest != NULL) ;

    width = p_bmp->getWidth() ;
    height = p_bmp->getHeight() ;

    if ((p_surface) && (p_dest))  {
        if (x + width > clipRight)  {
            srcRect.right = clipRight - x;
        } else {
            srcRect.right = width ;
        }

        if (y + height > clipBottom)  {
            srcRect.bottom = clipBottom - y;
        } else {
            srcRect.bottom = height ;
        }
        
        if (x < clipLeft)  {
            srcRect.left = clipLeft-x ;
            x = clipLeft ;
        } else {
            srcRect.left = 0 ;
        }

        if (y < clipTop)  {
            srcRect.top = clipTop-y ;
            y = clipTop ;
        } else {
            srcRect.top = 0 ;
        }
        p_dest->blitFast(x, y, p_bmp, &srcRect, isTransparent) ;
    }
}

/***********************************************/
/*  END OF FILE: CGOBJECT.CPP                  */
/***********************************************/
