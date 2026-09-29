#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <X11/Xcursor/Xcursor.h>

/* Internal SDL2 layout in the GeForce NOW build (2.32.10). */
struct sdl_cursor_x11 {
    SDL_Cursor *next;
    void *driverdata;
};

SDL_Cursor *SDL_CreateColorCursor(SDL_Surface *surface, int hot_x, int hot_y)
{
    SDL_Cursor *(*system_cursor)(int) = dlsym(RTLD_NEXT, "SDL_CreateSystemCursor");
    SDL_Cursor *(*color_cursor)(SDL_Surface *, int, int) =
        dlsym(RTLD_NEXT, "SDL_CreateColorCursor");
    SDL_Window *(*mouse_focus)(void) = dlsym(RTLD_NEXT, "SDL_GetMouseFocus");
    SDL_Window *(*keyboard_focus)(void) = dlsym(RTLD_NEXT, "SDL_GetKeyboardFocus");
    SDL_bool (*window_info)(SDL_Window *, SDL_SysWMinfo *) =
        dlsym(RTLD_NEXT, "SDL_GetWindowWMInfo");
    const char *(*video_driver)(void) =
        dlsym(RTLD_NEXT, "SDL_GetCurrentVideoDriver");
    const char *mode = getenv("GFN_CURSOR_MODE");

    if ((!mode || strcmp(mode, "arrow") != 0) &&
        surface && surface->pixels && surface->format &&
        surface->format->format == SDL_PIXELFORMAT_ARGB8888 &&
        system_cursor && window_info && video_driver &&
        video_driver() && strcmp(video_driver(), "x11") == 0) {
        SDL_Window *window = mouse_focus ? mouse_focus() : NULL;
        if (!window && keyboard_focus)
            window = keyboard_focus();

        SDL_SysWMinfo info;
        SDL_VERSION(&info.version);
        if (window && window_info(window, &info) &&
            info.subsystem == SDL_SYSWM_X11 &&
            XcursorSupportsARGB(info.info.x11.display)) {
            XcursorImage *image = XcursorImageCreate(surface->w, surface->h);
            if (image) {
                image->xhot = hot_x;
                image->yhot = hot_y;
                for (int y = 0; y < surface->h; ++y)
                    memcpy(image->pixels + y * surface->w,
                           (const Uint8 *)surface->pixels + y * surface->pitch,
                           (size_t)surface->w * 4);

                Cursor native = XcursorImageLoadCursor(info.info.x11.display, image);
                XcursorImageDestroy(image);
                if (native != None) {
                    SDL_Cursor *cursor = system_cursor(SDL_SYSTEM_CURSOR_ARROW);
                    if (cursor) {
                        struct sdl_cursor_x11 *x11 = (struct sdl_cursor_x11 *)cursor;
                        Cursor old = (Cursor)(uintptr_t)x11->driverdata;
                        if (old != None)
                            XFreeCursor(info.info.x11.display, old);
                        x11->driverdata = (void *)(uintptr_t)native;
                        return cursor;
                    }
                    XFreeCursor(info.info.x11.display, native);
                }
            }
        }
    }

    if (system_cursor) {
        SDL_Cursor *cursor = system_cursor(SDL_SYSTEM_CURSOR_ARROW);
        if (cursor)
            return cursor;
    }
    return color_cursor ? color_cursor(surface, hot_x, hot_y) : NULL;
}
