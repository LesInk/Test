#include "standard.h"
#include "direct.h"
#include <Windows.h>
#include "DITALK.H"

#include <SDL.h>

static int G_done = FALSE;
static SDL_Surface* screen;
static SDL_Surface* surface;
static SDL_Surface* largesurface;
static SDL_Rect srcrect = {
        0, 0,
        320, 240
    };
static SDL_Rect largesrcrect = {
        0, 0,
        640, 480
    };
static SDL_Rect destrect = {
        0, 0,
        640, 480
    };
extern T_void KeyboardUpdate(E_Boolean updateBuffers);

void WindowsUpdateMouse(void)
{
    int flags = 0 ;
    int x, y;
    Uint8 state;

    state = SDL_GetMouseState(&x, &y);
    DirectMouseSet(x, y);
    if (state & SDL_BUTTON_LMASK)
        flags |= MOUSE_BUTTON_LEFT;
    if (state & SDL_BUTTON_RMASK)
        flags |= MOUSE_BUTTON_RIGHT;
    if (state & SDL_BUTTON_MMASK)
        flags |= MOUSE_BUTTON_MIDDLE;
    DirectMouseSetButton(flags);
}

void WindowsUpdateEvents(void)
{
    int flags;
    SDL_Event event;

    while ( SDL_PollEvent(&event) ) {
        switch (event.type) {
            case SDL_QUIT:
                G_done = TRUE;
                break;
            case SDL_KEYDOWN:
                if ( event.key.keysym.sym == SDLK_ESCAPE )  {
                    G_done = TRUE; 
                } else if (event.key.keysym.sym == SDLK_f) {
#if 0
                    flags = screen->flags; /* Save the current flags in case toggling fails */
                    screen = SDL_SetVideoMode(0, 0, 0, screen->flags ^ SDL_FULLSCREEN); /*Toggles FullScreen Mode */
                    if(screen == NULL) screen = SDL_SetVideoMode(0, 0, 0, flags); /* If toggle FullScreen failed, then switch back */
                    if(screen == NULL) exit(1); /* If you can't switch back for some reason, then epic fail */                    
#endif
                }
                break;
#if 0
            case SDL_MOUSEMOTION:
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
                DirectMouseSet(event.motion.x, event.motion.y);
                flags = 0;
                if (event.motion.state & SDL_BUTTON_LMASK)
                    flags |= MOUSE_BUTTON_LEFT;
                if (event.motion.state & SDL_BUTTON_RMASK)
                    flags |= MOUSE_BUTTON_RIGHT;
                if (event.motion.state & SDL_BUTTON_MMASK)
                    flags |= MOUSE_BUTTON_MIDDLE;
                DirectMouseSetButton(flags);
                break;
#endif
        }    
        SDL_GetKeyState(NULL);
        //keys = SDL_GetKeyState(NULL);
        
//        if ( keys[SDLK_UP] )    ypos -= 1;
//        if ( keys[SDLK_DOWN] )  ypos += 1;
//        if ( keys[SDLK_LEFT] )  xpos -= 1;
//        if ( keys[SDLK_RIGHT] ) xpos += 1;

//        DrawScene();
    }
}

void WindowsUpdate(char *p_screen, unsigned char *palette)
{
    SDL_Color colors[256];
    int i;
    unsigned char *src = (char *)surface->pixels;
    unsigned char *dst = (char *)largesurface->pixels;
    unsigned char *line;
    int x, y;
    T_word32 tick = clock();
    static T_word32 lastTick = 0xFFFFEEEE;
    if ((tick-lastTick)<10) {
        // 10 ms between frames (top out at 100 ms)
    } else {
        lastTick = tick;
//printf("Update: %d (%d)\n", clock(), TickerGet());

    // Setup the color palette for this update
    for (i=0; i<256; i++) {
        colors[i].r = ((((unsigned int)*(palette++))&0x3F)<<2);
        colors[i].g = ((((unsigned int)*(palette++))&0x3F)<<2);
        colors[i].b = ((((unsigned int)*(palette++))&0x3F)<<2);
    }
    //SDL_SetColors(surface, colors, 0, 256);
    SDL_SetColors(largesurface, colors, 0, 256);

    // Blit the current surface
    for (y=0; y<240; y++) {
        line = src;
        for (x=0; x<320; x++) {
            *(dst++) = *src;
            *(dst++) = *(src++);
        }
        src = line;
        for (x=0; x<320; x++) {
            *(dst++) = *src;
            *(dst++) = *(src++);
        }
    }

    if (SDL_BlitSurface(largesurface, &largesrcrect, screen, &destrect)) {
        printf("Failed blit: %s\n", SDL_GetError());
    }
    SDL_UpdateRect(screen, 0, 0, 0, 0);
    WindowsUpdateEvents();
    WindowsUpdateMouse();
    KeyboardUpdate(TRUE) ;
    Sleep(1);
    }
}


extern T_void game_main(T_word16 argc, char *argv[]);

int SDL_main(int argc, char *argv[])
{
    Uint8 *keys;
    int flags;
    char *pixels;
    int x, y;
    SDL_Color black = { 0, 0, 0, 0 };
    SDL_Color white = { 255, 255, 255, 0 };

    if( SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) < 0)
    {
          printf ("Could not initialize SDL: %s\n",SDL_GetError());
          return 1;
    }

    atexit(SDL_Quit);

    screen = SDL_SetVideoMode(640, 480, 32, SDL_HWSURFACE|SDL_DOUBLEBUF);
    SDL_ShowCursor( SDL_DISABLE ); 

    if(screen == NULL)
    {
          printf("Could not set video mode: %s\n",SDL_GetError());
          return 1;
    }

    surface = SDL_CreateRGBSurface(SDL_SWSURFACE, 320, 240, 8, 0, 0, 0, 0);
    if (surface == NULL) {
        printf("Could not create overlay: %s\n", SDL_GetError());
        return 1;
    }
    largesurface = SDL_CreateRGBSurface(SDL_SWSURFACE, 640, 480, 8, 0, 0, 0, 0);
    if (largesurface == NULL) {
        printf("Could not create overlay: %s\n", SDL_GetError());
        return 1;
    }
    SDL_SetColors(surface, &black, 0, 1);
    SDL_SetColors(surface, &white, 255, 1);
    pixels = (char *)surface->pixels;
    GRAPHICS_ACTUAL_SCREEN = (void *)pixels;
    for (y=0; y<240; y++) {
        for (x=0; x<320; x++, pixels++) {
            if ((x == 0) || (x == 319) || (y == 0) || (y == 239))
                *pixels = 255;
            else
                *pixels = 0;
        }
    }

    game_main(argc, argv);

    return 0;
}

