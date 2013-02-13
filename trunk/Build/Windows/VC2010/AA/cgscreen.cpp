/***********************************************/
/*  FILE: CGSCREEN.C                           */
/***********************************************/

#include "cgscreen.hpp"
#include <assert.h>
#include <stdio.h>

//#define USE_BACK_SCREEN
//#define SLOPPY_FLIP

#define BASIC_DD_X_SIZE             640
#define BASIC_DD_Y_SIZE             480
#define BASIC_DD_BIT_RESOLUTION     8

#ifdef BASIC_DD_FULL_SCREEN
#    define BASIC_DD_COOPERATIVE_LEVEL  (DDSCL_EXCLUSIVE | \
                                         DDSCL_FULLSCREEN | \
                                         DDSCL_ALLOWMODEX)
#else
#    define BASIC_DD_COOPERATIVE_LEVEL  (DDSCL_NORMAL)
#endif

#define INFINITE_LOOP        while(1)

/*------------------------------------------------------------------------*/
CGraphicsScreen::CGraphicsScreen(
                     HWND windowHandle,
                     WORD width,
                     WORD height,
                     E_graphicsScreenType type) 
{
    HRESULT result ;
    static int typeList[GRAPHICS_SCREEN_TYPE_UNKNOWN] = { 8, 16, 24 } ;
    DDCAPS caps ;
    DDCAPS hel ;
    caps.dwSize = sizeof(caps) ;
    hel.dwSize = sizeof(hel) ;
    DDCAPS *p_caps ;

    _debug("CGraphicsScreen::CGraphicsScreen") ;
    _check(type < GRAPHICS_SCREEN_TYPE_UNKNOWN) ;

    /* Clear out all the pointers immediately */
    directDraw = NULL ;
    primarySurface = NULL ;
    secondarySurface = NULL ;
    clipper = NULL ;
    secondaryClipper = NULL ;

    window = windowHandle ;
    this->width = width ;
    this->height = height ;
    this->type = type ;

    DDSURFACEDESC   newSurface ;
    RGNDATA clipList ;
    RECT *p_rects ;
    
    clipList.rdh.dwSize = sizeof(RGNDATAHEADER) ;
    clipList.rdh.iType = RDH_RECTANGLES ;
    clipList.rdh.nCount = 1 ;
    clipList.rdh.nRgnSize = sizeof(RECT) ;
    clipList.rdh.rcBound.left = 0 ;
    clipList.rdh.rcBound.top = 0 ;
    clipList.rdh.rcBound.right = width-1 ;
    clipList.rdh.rcBound.bottom = height-1 ;
    p_rects = (RECT *)clipList.Buffer ;
    p_rects->left = 0 ;
    p_rects->top = 0 ;
    p_rects->right = width-1 ;
    p_rects->bottom = height-1 ;
    directDrawPalette = NULL ;

    clearScreenOnFlip = FALSE ;

    /* Set up the object list */
    numObjects = 0 ;
    lastObject = 0 ;
    freeObject = GRAPHICS_OBJECT_HANDLE_BAD ;
    memset(listObjects, 0, sizeof(listObjects)) ;

    /* Create the backdrop to draw to */
    p_backScreen = new CBmpGraphic(width, height) ;
    _check(p_backScreen != NULL) ;
    if (p_backScreen == NULL)
        return ;

    /* Set up the direct draw services */
    result = DirectDrawCreate(NULL, (LPDIRECTDRAW *)&directDraw, NULL) ;
    _check(result == DD_OK) ;

    if (result == DD_OK)  {
     directDraw->GetCaps(&caps, &hel) ;
    p_caps = &caps ;
    puts("\n\nHardware Capabilities:") ;
    if (p_caps->dwCaps & DDCAPS_3D)
        puts("  DDCAPS_3D") ;
    if (p_caps->dwCaps & DDCAPS_ALIGNBOUNDARYDEST)
        puts("  DDCAPS_ALIGNBOUNDARYDEST") ;
    if (p_caps->dwCaps & DDCAPS_ALIGNBOUNDARYSRC)
        puts("  DDCAPS_ALIGNBOUNDARYSRC") ;
    if (p_caps->dwCaps & DDCAPS_ALIGNSIZEDEST)
        puts("  DDCAPS_ALIGNSIZEDEST") ;
    if (p_caps->dwCaps & DDCAPS_ALIGNSIZESRC)
        puts("  DDCAPS_ALIGNSIZESRC") ;
    if (p_caps->dwCaps & DDCAPS_ALIGNSTRIDE)
        puts("  DDCAPS_ALIGNSTRIDE") ;
    if (p_caps->dwCaps & DDCAPS_ALPHA)
        puts("  DDCAPS_ALPHA") ;
    if (p_caps->dwCaps & DDCAPS_BANKSWITCHED)
        puts("  DDCAPS_BANKSWITCHED") ;
    if (p_caps->dwCaps & DDCAPS_BLT)
        puts("  DDCAPS_BLT") ;
    if (p_caps->dwCaps & DDCAPS_BLTCOLORFILL)
        puts("  DDCAPS_BLTCOLORFILL") ;
    if (p_caps->dwCaps & DDCAPS_BLTDEPTHFILL)
        puts("  DDCAPS_BLTDEPTHFILL") ;
    if (p_caps->dwCaps & DDCAPS_BLTFOURCC)
        puts("  DDCAPS_BLTFOURCC") ;
    if (p_caps->dwCaps & DDCAPS_BLTQUEUE)
        puts("  DDCAPS_BLTQUEUE") ;
    if (p_caps->dwCaps & DDCAPS_BLTSTRETCH)
        puts("  DDCAPS_BLTSTRETCH") ;
    if (p_caps->dwCaps & DDCAPS_CANBLTSYSMEM)
        puts("  DDCAPS_CANBLTSYSMEM") ;
    if (p_caps->dwCaps & DDCAPS_CANCLIP)
        puts("  DDCAPS_CANCLIP") ;
    if (p_caps->dwCaps & DDCAPS_CANCLIPSTRETCHED)
        puts("  DDCAPS_CANCLIPSTRETCHED") ;
    if (p_caps->dwCaps & DDCAPS_COLORKEY)
        puts("  DDCAPS_COLORKEY") ;
    if (p_caps->dwCaps & DDCAPS_COLORKEYHWASSIST)
        puts("  DDCAPS_COLORKEYHWASSIST") ;
    if (p_caps->dwCaps & DDCAPS_GDI)
        puts("  DDCAPS_GDI") ;
    if (p_caps->dwCaps & DDCAPS_NOHARDWARE)
        puts("  DDCAPS_NOHARDWARE") ;
    if (p_caps->dwCaps & DDCAPS_OVERLAY)
        puts("  DDCAPS_OVERLAY") ;
    if (p_caps->dwCaps & DDCAPS_OVERLAYCANTCLIP)
        puts("  DDCAPS_OVERLAYCANTCLIP") ;
    if (p_caps->dwCaps & DDCAPS_OVERLAYFOURCC)
        puts("  DDCAPS_OVERLAYFOURCC") ;
    if (p_caps->dwCaps & DDCAPS_OVERLAYSTRETCH)
        puts("  DDCAPS_OVERLAYSTRETCH") ;
    if (p_caps->dwCaps & DDCAPS_PALETTE)
        puts("  DDCAPS_PALETTE") ;
    if (p_caps->dwCaps & DDCAPS_PALETTEVSYNC)
        puts("  DDCAPS_PALETTEVSYNC") ;
    if (p_caps->dwCaps & DDCAPS_READSCANLINE)
        puts("  DDCAPS_READSCANLINE") ;
//    if (p_caps->dwCaps & DDCAPS_STEREOVIEW)
//        puts("  DDCAPS_STEREOVIEW") ;
    if (p_caps->dwCaps & DDCAPS_VBI)
        puts("  DDCAPS_VBI") ;
    if (p_caps->dwCaps & DDCAPS_ZBLTS)
        puts("  DDCAPS_ZBLTS") ;
    if (p_caps->dwCaps & DDCAPS_ZOVERLAYS)
        puts("  DDCAPS_ZOVERLAYS") ;
    puts("Capabilities 2:") ;
    if (p_caps->dwCaps2 & DDCAPS2_CERTIFIED)
        puts("  DDCAPS2_CERTIFIED") ;
    if (p_caps->dwCaps2 & DDCAPS2_NO2DDURING3DSCENE)
        puts("  DDCAPS2_NO2DDURING3DSCENE") ;
    printf("VidMemTotal: %ld\n", p_caps->dwVidMemTotal) ;
    printf("VidMemFree: %ld\n", p_caps->dwVidMemFree) ;

    p_caps = &hel ;
    puts("\n\nEmulation Capabilities:") ;
    if (p_caps->dwCaps & DDCAPS_3D)
        puts("  DDCAPS_3D") ;
    if (p_caps->dwCaps & DDCAPS_ALIGNBOUNDARYDEST)
        puts("  DDCAPS_ALIGNBOUNDARYDEST") ;
    if (p_caps->dwCaps & DDCAPS_ALIGNBOUNDARYSRC)
        puts("  DDCAPS_ALIGNBOUNDARYSRC") ;
    if (p_caps->dwCaps & DDCAPS_ALIGNSIZEDEST)
        puts("  DDCAPS_ALIGNSIZEDEST") ;
    if (p_caps->dwCaps & DDCAPS_ALIGNSIZESRC)
        puts("  DDCAPS_ALIGNSIZESRC") ;
    if (p_caps->dwCaps & DDCAPS_ALIGNSTRIDE)
        puts("  DDCAPS_ALIGNSTRIDE") ;
    if (p_caps->dwCaps & DDCAPS_ALPHA)
        puts("  DDCAPS_ALPHA") ;
    if (p_caps->dwCaps & DDCAPS_BANKSWITCHED)
        puts("  DDCAPS_BANKSWITCHED") ;
    if (p_caps->dwCaps & DDCAPS_BLT)
        puts("  DDCAPS_BLT") ;
    if (p_caps->dwCaps & DDCAPS_BLTCOLORFILL)
        puts("  DDCAPS_BLTCOLORFILL") ;
    if (p_caps->dwCaps & DDCAPS_BLTDEPTHFILL)
        puts("  DDCAPS_BLTDEPTHFILL") ;
    if (p_caps->dwCaps & DDCAPS_BLTFOURCC)
        puts("  DDCAPS_BLTFOURCC") ;
    if (p_caps->dwCaps & DDCAPS_BLTQUEUE)
        puts("  DDCAPS_BLTQUEUE") ;
    if (p_caps->dwCaps & DDCAPS_BLTSTRETCH)
        puts("  DDCAPS_BLTSTRETCH") ;
    if (p_caps->dwCaps & DDCAPS_CANBLTSYSMEM)
        puts("  DDCAPS_CANBLTSYSMEM") ;
    if (p_caps->dwCaps & DDCAPS_CANCLIP)
        puts("  DDCAPS_CANCLIP") ;
    if (p_caps->dwCaps & DDCAPS_CANCLIPSTRETCHED)
        puts("  DDCAPS_CANCLIPSTRETCHED") ;
    if (p_caps->dwCaps & DDCAPS_COLORKEY)
        puts("  DDCAPS_COLORKEY") ;
    if (p_caps->dwCaps & DDCAPS_COLORKEYHWASSIST)
        puts("  DDCAPS_COLORKEYHWASSIST") ;
    if (p_caps->dwCaps & DDCAPS_GDI)
        puts("  DDCAPS_GDI") ;
    if (p_caps->dwCaps & DDCAPS_NOHARDWARE)
        puts("  DDCAPS_NOHARDWARE") ;
    if (p_caps->dwCaps & DDCAPS_OVERLAY)
        puts("  DDCAPS_OVERLAY") ;
    if (p_caps->dwCaps & DDCAPS_OVERLAYCANTCLIP)
        puts("  DDCAPS_OVERLAYCANTCLIP") ;
    if (p_caps->dwCaps & DDCAPS_OVERLAYFOURCC)
        puts("  DDCAPS_OVERLAYFOURCC") ;
    if (p_caps->dwCaps & DDCAPS_OVERLAYSTRETCH)
        puts("  DDCAPS_OVERLAYSTRETCH") ;
    if (p_caps->dwCaps & DDCAPS_PALETTE)
        puts("  DDCAPS_PALETTE") ;
    if (p_caps->dwCaps & DDCAPS_PALETTEVSYNC)
        puts("  DDCAPS_PALETTEVSYNC") ;
    if (p_caps->dwCaps & DDCAPS_READSCANLINE)
        puts("  DDCAPS_READSCANLINE") ;
//    if (p_caps->dwCaps & DDCAPS_STEREOVIEW)
//        puts("  DDCAPS_STEREOVIEW") ;
    if (p_caps->dwCaps & DDCAPS_VBI)
        puts("  DDCAPS_VBI") ;
    if (p_caps->dwCaps & DDCAPS_ZBLTS)
        puts("  DDCAPS_ZBLTS") ;
    if (p_caps->dwCaps & DDCAPS_ZOVERLAYS)
        puts("  DDCAPS_ZOVERLAYS") ;
    puts("Capabilities 2:") ;
    if (p_caps->dwCaps2 & DDCAPS2_CERTIFIED)
        puts("  DDCAPS2_CERTIFIED") ;
    if (p_caps->dwCaps2 & DDCAPS2_NO2DDURING3DSCENE)
        puts("  DDCAPS2_NO2DDURING3DSCENE") ;
    printf("VidMemTotal: %ld\n", p_caps->dwVidMemTotal) ;
    printf("VidMemFree: %ld\n", p_caps->dwVidMemFree) ;
       /* Put the screen in exclusive full screen mode */
        result = directDraw->SetCooperativeLevel(
                windowHandle,
                BASIC_DD_COOPERATIVE_LEVEL) ;
        _check(result == DD_OK) ;

        if (result == DD_OK)  {
#ifdef BASIC_DD_FULL_SCREEN
            result = directDraw->SetDisplayMode(width, height, typeList[type]) ;
#endif
            /* Go into our favorite video mode:  640x480x8BIT */
            if (result == DD_OK)  {
                /* Set up the primary surface with 1 back surface */
                memset(&newSurface, 0, sizeof(newSurface)) ;
                newSurface.dwSize = sizeof(newSurface) ;
#ifdef BASIC_DD_FULL_SCREEN
                newSurface.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT ;
                newSurface.dwBackBufferCount = 1 ;
                newSurface.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | 
                                            DDSCAPS_FLIP | 
                                            DDSCAPS_COMPLEX ;
#else
                newSurface.dwFlags = DDSD_CAPS ;
                newSurface.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE ;
#endif

                result = directDraw->CreateSurface(
                        &newSurface,
                        &primarySurface,
                        NULL) ;
                _check(result == DD_OK) ;

                if (result == DD_OK)  {
                    /* When we created the primary surface we created one */
                    /* back buffer.  This back buffer we will use for */
                    /* the second drawing surface */
#ifdef BASIC_DD_FULL_SCREEN
                    DDSCAPS ddscaps;
                    ddscaps.dwCaps = DDSCAPS_BACKBUFFER ;
                    result = primarySurface->GetAttachedSurface(&ddscaps, &secondarySurface) ;
#else
                    newSurface.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH ;
                    newSurface.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN ;
                    newSurface.dwHeight = height ;
                    newSurface.dwWidth = width;
                    result = directDraw->CreateSurface(
                                  &newSurface, 
                                  &secondarySurface, 
                                  NULL);
#endif

                    _check(result == DD_OK) ;

                    if (result == DD_OK)  {
//        ddrval = IDirectDraw_SetDisplayMode( lpdd,
//            GameMode.cx, GameMode.cy, GameBPP);
                        
#ifndef BADIC_DD_FULL_SCREEN
                        result = directDraw->CreateClipper(0, &clipper, NULL);
                        _check(result == DD_OK) ;
                        if (result == DD_OK)  {
                            clipper->SetHWnd(0, windowHandle) ;
                            primarySurface->SetClipper(clipper) ;
#if 0
                            /* Got here means that everything is setup */
                            /* correctly. */
                            result = directDraw->CreateClipper(0, &secondaryClipper, NULL) ;
                            _check(result == DD_OK) ;
                            if (result == DD_OK)  {
                                secondaryClipper->SetClipList(&clipList, 0) ;
                                secondarySurface->SetClipper(secondaryClipper) ;
                            }
#endif
                        }
#endif
                    }
                }
            }
        }
    } else {
        MessageBox(windowHandle, "Cannot initialized Direct Draw.\nMake sure you have direct draw installed.", "Direct Draw Error", MB_OK) ;
        exit(1) ;
    }
}

/*------------------------------------------------------------------------*/
CGraphicsScreen::~CGraphicsScreen()
{
    T_graphicsObjectHandle index ;

    _debug("CGraphicsScreen::~CGraphicsScreen") ;

    if (p_backScreen)
        delete p_backScreen ;

    if (directDraw != NULL)  {
        /* Free all the graphic objects */
        /* Clear the free list into null pointers */
        while(freeObject != GRAPHICS_OBJECT_HANDLE_BAD)  {
            index = listObjects[freeObject].index ;
            listObjects[freeObject].p_object = NULL ;
            freeObject = index ;
        } ;

        /* Go through the list and free non-null entries */
        for (index=0; index<lastObject; index++)  {
            delete listObjects[index].p_object ;
            listObjects[index].p_object = NULL ;
        }

        if (clipper != NULL)  {
            clipper->Release() ;
            clipper = NULL ;
        }
        if (secondaryClipper != NULL)  {
            secondaryClipper->Release() ;
            secondaryClipper = NULL ;
        }
        if (primarySurface != NULL)  {
            primarySurface->Release() ;
            primarySurface = NULL ;
        }
        if (secondarySurface != NULL)  {
            secondarySurface->Release() ;
            secondarySurface = NULL ;
        }
        directDraw->Release() ;
        directDraw = NULL ;
    }
}

/*------------------------------------------------------------------------*/
void CGraphicsScreen::Flip(int mode)
{
    RECT source;
    RECT rcWindow ;
static int test = 1 ;    
    _debug("CGraphicsScreen::Flip") ;

    if (!p_backScreen)
        return ;

    if (mode == 1)
        directDraw->WaitForVerticalBlank(DDWAITVB_BLOCKEND, NULL) ;

    /* Determine the destination window location */
    GetClientRect(window, &rcWindow);
    ClientToScreen(window, (LPPOINT)&rcWindow);
    rcWindow.right = rcWindow.left+width-1 ;
    rcWindow.bottom = rcWindow.top+height-1 ;

    /* Determine the source window location */
    source.left = 0 ;
    source.top = 0 ;
    source.right = width-1 ;
    source.bottom = height-1 ;

if (test)  {
#ifdef BASIC_DD_FULL_SCREEN
#ifndef SLOPPY_FLIP
    HRESULT result ;
    while(1)  {
        result = primarySurface->Flip(NULL, DDFLIP_WAIT) ;
        if (result == DD_OK)
            break;
    }
#endif
#else
    /* Blit the screen */
    primarySurface->Blt(
                    &rcWindow,              // dest rect
                    secondarySurface,     // src surface
                    &source,                   // src rect (all of it)
                    (mode==0)?DDBLT_WAIT:0,
                    NULL);
#endif
}
//test ^= 1 ;

    if (clearScreenOnFlip == TRUE)  {
        DDBLTFX     ddbltfx;
        ddbltfx.dwSize = sizeof( ddbltfx );
        ddbltfx.dwFillColor = 0;
        secondarySurface->Blt(
                            NULL,                    // dest rect
                            NULL,                   // src surface
                            NULL,                   // src rect
                            DDBLT_COLORFILL | DDBLT_WAIT,
                            &ddbltfx);

    }
}

/*------------------------------------------------------------------------*/
#ifndef NDEBUG
void CGraphicsScreen::Test(void)
{
    HDC hdc ;
    RECT rect ;
    static int value = 0 ;
    char line[80] ;
    HBRUSH brush ;

    _debug("CGraphicsScreen::Test") ;

    secondarySurface->Restore() ;
    brush = CreateSolidBrush(0x00FF0000) ;
    if (secondarySurface->GetDC(&hdc) == DD_OK) {
        rect.left = 5 ;
        rect.top = 5 ;
        rect.right = 300 ;
        rect.bottom = 300 ;
        FrameRect(hdc, &rect, brush) ;

        rect.left = 50 ;
        rect.top = 50 ;
        rect.right = 200 ;
        rect.bottom = 200 ;
        FillRect(hdc, &rect, brush) ;
        wsprintf(line, "Hello %d", value++) ;
        SetTextColor(hdc, 0x00FFFFFF) ;
        SetBkColor(hdc, 0x00FF0000) ;
        DrawText(hdc, line, strlen(line), &rect, DT_LEFT) ;
        secondarySurface->ReleaseDC(hdc) ;
    }
}
#endif

/*------------------------------------------------------------------------*/
HDC CGraphicsScreen::GetDC(void)
{
    HDC hdc ;

    _debug("CGraphicsScreen::GetDC") ;

#ifndef SLOPPY_FLIP
    secondarySurface->Restore() ;
    secondarySurface->GetDC(&hdc) ;
#else
    primarySurface->Restore() ;
    primarySurface->GetDC(&hdc) ;
#endif
    return hdc ;
}

/*------------------------------------------------------------------------*/
void CGraphicsScreen::ReleaseDC(HDC dc)
{
    _debug("CGraphicsScreen::ReleaseDC") ;
        
    _check(dc != NULL) ;

#ifndef SLOPPY_FLIP
    secondarySurface->ReleaseDC(dc) ;
#else
    primarySurface->ReleaseDC(dc) ;
#endif
}

/*------------------------------------------------------------------------*/
T_graphicsObjectHandle CGraphicsScreen::allocObjectSlot(void)
{
    T_graphicsObjectHandle newObject = GRAPHICS_OBJECT_HANDLE_BAD ;

    _debug("CGraphicsScreen::allocObjectSlot") ;
    _check(numObjects < MAX_GRAPHICS_OBJECTS) ;

    /* Are there any free object slots available? */
    if (freeObject == GRAPHICS_OBJECT_HANDLE_BAD)  {
        /* Ok, allocate from the end. */
        /* Is there room at the end? */
        if (lastObject < MAX_GRAPHICS_OBJECTS)  {
            /* Yes, there is room. */
            /* Take that position. */
            newObject = lastObject ;
            lastObject++ ;
        }
    } else {
        /* There is a free slot. */
        /* Take that free position */
        newObject = freeObject ;

        /* Update the next free object */
        freeObject = listObjects[newObject].index ;
    }

    if (newObject != GRAPHICS_OBJECT_HANDLE_BAD)
        numObjects++ ;

    return newObject ;
}

/*------------------------------------------------------------------------*/
void CGraphicsScreen::deallocObjectSlot(WORD objectIndex) 
{
    _debug("CGraphicsScreen::deallocObjectSlot") ;
    _check(numObjects > 0) ;
    _check(objectIndex != GRAPHICS_OBJECT_HANDLE_BAD) ;
    _check(objectIndex < MAX_GRAPHICS_OBJECTS) ;

    /* Put this object on the free list */
    listObjects[objectIndex].index = freeObject ;
    freeObject = objectIndex ;

    numObjects-- ;
}

/*------------------------------------------------------------------------*/
T_graphicsObjectHandle CGraphicsScreen::createGraphicsObject(char *p_filename)
{
    T_graphicsObjectHandle gObject ;
    CGraphicsObject *p_gObject ;

    _debug("CGraphicsScreen::createGraphicsObject(filename)") ;
    _check(p_filename != NULL) ;

    /* Try getting a slot for this new object */
    gObject = allocObjectSlot() ;
    if (gObject != GRAPHICS_OBJECT_HANDLE_BAD)  {
        /* Create the object */
        p_gObject = new CGraphicsObject((LPDIRECTDRAW)directDraw, p_filename) ;

        /* Don't keep it if the object is empty. */
        if (p_gObject->isEmpty())  {
            delete p_gObject ;
            p_gObject = NULL ;
        }

        if (p_gObject)  {
            /* Store the newly created object in the list */
            listObjects[gObject].p_object = p_gObject ;
        } else {
            /* Couldn't create.  Destroy this object */
            deallocObjectSlot(gObject) ;
            gObject = GRAPHICS_OBJECT_HANDLE_BAD ;
        }
    }

    return gObject ;
}

/*------------------------------------------------------------------------*/
T_graphicsObjectHandle CGraphicsScreen::createGraphicsObject(CBmpGraphic *p_bmp)
{
    T_graphicsObjectHandle gObject ;
    CGraphicsObject *p_gObject ;

    _debug("CGraphicsScreen::createGraphicsObject(BMP)") ;
    _check(p_bmp != NULL) ;

    /* Try getting a slot for this new object */
    gObject = allocObjectSlot() ;
    if (gObject != GRAPHICS_OBJECT_HANDLE_BAD)  {
        /* Create the object */
        p_gObject = new CGraphicsObject((LPDIRECTDRAW)directDraw, p_bmp) ;
        if (p_gObject)  {
            /* Store the newly created object in the list */
            listObjects[gObject].p_object = p_gObject ;
        } else {
            /* Couldn't create.  Destroy this object */
            deallocObjectSlot(gObject) ;
            gObject = GRAPHICS_OBJECT_HANDLE_BAD ;
        }
    }

    return gObject ;
}

/*------------------------------------------------------------------------*/
void CGraphicsScreen::destroyGraphicsObject(T_graphicsObjectHandle gObject)
{
    _debug("CGraphicsScreen::destroyGraphicsObject") ;
    _check(gObject != GRAPHICS_OBJECT_HANDLE_BAD) ;
    _check(gObject < MAX_GRAPHICS_OBJECTS) ;

    if ((gObject != GRAPHICS_OBJECT_HANDLE_BAD) &&
        (gObject < MAX_GRAPHICS_OBJECTS))  {
        /* Delete the related C class object */
        delete listObjects[gObject].p_object ;
        listObjects[gObject].p_object = NULL ;

        /* Now remove it from the list */
        deallocObjectSlot(gObject) ;
    }
}

/*------------------------------------------------------------------------*/
void CGraphicsScreen::drawObject(
         T_graphicsObjectHandle gObject,
         int x,
         int y)
{
    _debug("CGraphicsScreen::drawObject") ;
    _check(gObject != GRAPHICS_OBJECT_HANDLE_BAD) ;
    _check(gObject < MAX_GRAPHICS_OBJECTS) ;

    if ((gObject != GRAPHICS_OBJECT_HANDLE_BAD) &&
        (gObject < MAX_GRAPHICS_OBJECTS))  {
        /* Pass on the command with the appropriate surface */
        listObjects[gObject].p_object->draw(
#ifdef USE_BACK_SCREEN
            p_backScreen,
#else
            secondarySurface, 
#endif
            x, y, 
            width, height) ;
    }
}

/*------------------------------------------------------------------------*/
void CGraphicsScreen::drawObjectClipped(
         T_graphicsObjectHandle gObject,
         int x,
         int y,
         int clipLeft,
         int clipTop,
         int clipRight,
         int clipBottom) 
{
    _debug("CGraphicsScreen::drawObjectClipped") ;
    _check(gObject != GRAPHICS_OBJECT_HANDLE_BAD) ;
    _check(gObject < MAX_GRAPHICS_OBJECTS) ;

    if ((gObject != GRAPHICS_OBJECT_HANDLE_BAD) &&
        (gObject < MAX_GRAPHICS_OBJECTS))  {
        /* Pass on the command with the appropriate surface */
        listObjects[gObject].p_object->drawClipped(
#ifdef USE_BACK_SCREEN
            p_backScreen,
#else
            secondarySurface, 
#endif
            x, y, 
            clipLeft, clipTop, clipRight, clipBottom) ;
    }
}
/*------------------------------------------------------------------------*/
void CGraphicsScreen::getGraphicsObjectSize(
                 T_graphicsObjectHandle gObject,
                 WORD *p_width,
                 WORD *p_height)
{
    _debug("CGraphicsScreen::drawObject") ;

    if (validateGraphicsObjectHandle(gObject) == TRUE)  {
        /* Pass on the command with the appropriate surface */
        listObjects[gObject].p_object->getSize(p_width, p_height) ;
    }
}

/*------------------------------------------------------------------------*/
BOOL CGraphicsScreen::validateGraphicsObjectHandle(
                           T_graphicsObjectHandle gObject)
{
    _debug("CGraphicsScreen::validateGraphicsObjectHandle") ;
    _check(gObject != GRAPHICS_OBJECT_HANDLE_BAD) ;
    _check(gObject < MAX_GRAPHICS_OBJECTS) ;

    if ((gObject != GRAPHICS_OBJECT_HANDLE_BAD) &&
        (gObject < MAX_GRAPHICS_OBJECTS))
        return TRUE ;
    return FALSE ;
}

/*------------------------------------------------------------------------*/
void CGraphicsScreen::getClientRect(RECT *p_rect)
{
    GetClientRect(window, p_rect) ;
}

/*------------------------------------------------------------------------*/
void CGraphicsScreen::setGraphicsObjectTransparency(
                 T_graphicsObjectHandle gObject,
                 BOOL trans) 
{
    _debug("CGraphicsScreen::setGraphicsObjectTransparency") ;

    if (validateGraphicsObjectHandle(gObject) == TRUE)  {
        /* Pass on the command with the appropriate surface */
        listObjects[gObject].p_object->setTransparency(trans) ;
    }
}

/*------------------------------------------------------------------------*/
void CGraphicsScreen::setGraphicsObjectBMP(
                 T_graphicsObjectHandle gObject,
                 CBmpGraphic *p_bmp)
{
    _debug("CGraphicsScreen::setGraphicsObjectBMP") ;
    _check(p_bmp != NULL) ;

    if (validateGraphicsObjectHandle(gObject) == TRUE)  {
        /* Pass on the command with the appropriate surface */
        listObjects[gObject].p_object->changeBMP(p_bmp) ;
    }
}
/*------------------------------------------------------------------------*/
void CGraphicsScreen::setPalette(LPRGBQUAD p_newPalette)
{
    _debug("CGraphicsScreen::setPalette") ;
    _check(p_newPalette != NULL) ;
    _check(this != NULL) ;

    for (int i=0; i<256; i++)  {
        palette[i].peRed = p_newPalette[i].rgbRed ;
        palette[i].peGreen = p_newPalette[i].rgbGreen ;
        palette[i].peBlue = p_newPalette[i].rgbBlue ;
        palette[i].peFlags = PC_NOCOLLAPSE ;

/*
        palette[i].peRed = i ;
        palette[i].peGreen = i ;
        palette[i].peBlue = i ;
*/
    }
    if (directDrawPalette == NULL)
        directDraw->CreatePalette(DDPCAPS_8BIT|DDPCAPS_ALLOW256, palette, &directDrawPalette, NULL) ;
    directDrawPalette->SetEntries(0, 0, 256, palette) ;
    primarySurface->SetPalette(directDrawPalette) ;
    if (p_backScreen)  {
        p_backScreen->forcePalette(p_newPalette) ;
        p_backScreen->fixUpPalette() ;
    }
//    primarySurface->RealizePalette() ;
//    secondarySurface->SetPalette(directDrawPalette) ;
}

/*------------------------------------------------------------------------*/
void CGraphicsScreen::drawObjectRotated(
         T_graphicsObjectHandle srcObject,
         T_graphicsObjectHandle destObject,
         WORD angle)
{
    _debug("CGraphicsScreen::drawObject") ;

    if (validateGraphicsObjectHandle(srcObject) == TRUE)  {
        if (validateGraphicsObjectHandle(destObject) == TRUE)  {
            /* Pass on the command with the appropriate surfaces */
            listObjects[srcObject].p_object->drawRotated(
                listObjects[destObject].p_object, 
                angle) ;
        }
    }
}

/*------------------------------------------------------------------------*/
void CGraphicsScreen::drawBack(void)
{
    _debug("CGraphicsScreen::drawBack") ;
    _check(p_backScreen != NULL) ;


#ifdef USE_BACK_SCREEN
    if (p_backScreen)  {
        /* Draw the back screen to the secondary screen */
#if 1
#ifndef SLOPPY_FLIP
        HDC hdc ;
        if (secondarySurface->GetDC(&hdc) == DD_OK)  {
            p_backScreen->drawToDCClipped(hdc, 0, 0, 0, 0, width, height) ;
//            p_backScreen->drawToDCClipped(hdc, 0, 0, 9, 9, 471, 471) ;
            secondarySurface->ReleaseDC(hdc) ;
        }
#else
        HDC hdc ;
        if (primarySurface->GetDC(&hdc) == DD_OK)  {
            p_backScreen->drawToDCClipped(hdc, 0, 0, 0, 0, width, height) ;
//            p_backScreen->drawToDCClipped(hdc, 0, 0, 9, 9, 471, 471) ;
            primarySurface->ReleaseDC(hdc) ;
        }
#endif
#else
        DDSURFACEDESC surfaceDesc ;
        HRESULT result ;
        surfaceDesc.dwSize = sizeof(DDSURFACEDESC) ;
        RECT area = { 0, 0, width, height } ;
        if ((result = secondarySurface->Lock(&area, &surfaceDesc, DDLOCK_WAIT, NULL)) == DD_OK)  {
            BYTE *p_from = (BYTE *)p_backScreen->getRawData() ;
            BYTE *p_to = (BYTE *)surfaceDesc.lpSurface ;
            p_to += width * (height-1) ;

            for (int i=height; i; i--)  {
                memcpy(p_to, p_from, width) ;
                p_from += width ;
                p_to -= surfaceDesc.lPitch ;
            }
            secondarySurface->Unlock(NULL) ;
        } else {
            printf("result = %08lX\n", result) ;
        }
#endif
    }
#endif
}

/*------------------------------------------------------------------------*/
void CGraphicsScreen::clear(void)
{
    _debug("CGraphicsScreen::clear") ;

/*
    DDSURFACEDESC surfaceDesc ;
    HRESULT result ;
    surfaceDesc.dwSize = sizeof(DDSURFACEDESC) ;
    RECT area = { 0, 0, width, height } ;
    if ((result = secondarySurface->Lock(&area, &surfaceDesc, DDLOCK_WAIT, NULL)) == DD_OK)  {
        BYTE *p_to = (BYTE *)surfaceDesc.lpSurface ;
//        p_to += width * (height-1) ;
        p_to += surfaceDesc.lPitch * (height-1) ;

        for (int i=height; i; i--)  {
            memset(p_to, 0, width) ;
            p_to -= surfaceDesc.lPitch ;
        }
        secondarySurface->Unlock(NULL) ;
    } else {
        printf("result = %08lX\n", result) ;
    }
*/
        DDBLTFX     ddbltfx;
        ddbltfx.dwSize = sizeof( ddbltfx );
        ddbltfx.dwFillColor = 0;
        secondarySurface->Blt(
                            NULL,                    // dest rect
                            NULL,                   // src surface
                            NULL,                   // src rect
                            DDBLT_COLORFILL | DDBLT_WAIT,
                            &ddbltfx);

}

/***********************************************/
/*  END OF FILE: CGSCREEN.C                    */
/***********************************************/
