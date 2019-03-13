#pragma once

#include <SDL2/SDL.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	NXHooksSDLCursor_Pointer,
	NXHooksSDLCursor_ResizeHoriz,
	NXHooksSDLCursor_Hand,
	NXHooksSDLCursor_Busy
} NXHooksSDLCursorType;

void nxHooksSDLCursorInit(SDL_Renderer* renderer);
void nxHooksSDLCursorRender();
void nxHooksSDLCursorSet(NXHooksSDLCursorType type);
void nxHooksSDLCursorSetPos(int x, int y);


#ifdef __cplusplus
}
#endif