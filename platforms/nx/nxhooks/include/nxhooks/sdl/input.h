#pragma once

#include <SDL2/SDL.h>

#ifdef __cplusplus
extern "C" {
#endif

int nxHooksSDLPollEvents(SDL_Event* event);

#ifdef __cplusplus
}
#endif