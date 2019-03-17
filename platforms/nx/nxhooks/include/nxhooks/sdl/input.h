#pragma once

#include <SDL2/SDL.h>

#ifdef __cplusplus
extern "C" {
#endif

int nxHooksSDLPollEvents(SDL_Event* event);
void nxHooksSDLInputInit();
void nxHooksSDLInputExit();
#ifdef __cplusplus
}
#endif