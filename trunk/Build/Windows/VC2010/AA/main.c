#include "standard.h"
#include "direct.h"
#include <Windows.h>

#include <SDL.h>

int SDL_main(int argc, char *argv[])
{
    int done = FALSE;
    Uint8 *keys;
    int flags;
    SDL_Surface* screen;

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
    while( !done ) {
        SDL_Event event;

        while ( SDL_PollEvent(&event) ) {
            switch (event.type) {
                case SDL_QUIT:
                    done = TRUE;
                    break;
                case SDL_KEYDOWN:
                    if ( event.key.keysym.sym == SDLK_ESCAPE )  {
                        done = TRUE; 
                    } else if (event.key.keysym.sym == SDLK_f) {
                        flags = screen->flags; /* Save the current flags in case toggling fails */
                        screen = SDL_SetVideoMode(0, 0, 0, screen->flags ^ SDL_FULLSCREEN); /*Toggles FullScreen Mode */
                        if(screen == NULL) screen = SDL_SetVideoMode(0, 0, 0, flags); /* If toggle FullScreen failed, then switch back */
                        if(screen == NULL) exit(1); /* If you can't switch back for some reason, then epic fail */                    
                    }
                    break;
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

    return 0;
}

