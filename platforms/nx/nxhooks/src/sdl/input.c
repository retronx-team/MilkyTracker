#include <nxhooks/sdl/input.h>
#include <nxhooks/screen.h>
#include <stdbool.h>

static void _fingerEventToMouseButton(SDL_Event* event, bool down) {
	if(event->tfinger.fingerId == 0) {
		SDL_Event newEvent;
		newEvent.type = down ? SDL_MOUSEBUTTONDOWN : SDL_MOUSEBUTTONUP;
		newEvent.button.state = down ? SDL_PRESSED : SDL_RELEASED;
		newEvent.button.clicks = 1;
		newEvent.button.windowID = 0;
		newEvent.button.timestamp = event->tfinger.timestamp;
		newEvent.button.which = SDL_TOUCH_MOUSEID;
		newEvent.button.button = SDL_BUTTON_LEFT;
		newEvent.button.x = (Sint32)(event->tfinger.x * (float)(NX_DISPLAY_WIDTH));
		newEvent.button.y = (Sint32)(event->tfinger.y * (float)(NX_DISPLAY_HEIGHT));
		memcpy(event, &newEvent, sizeof(SDL_Event));
	}
}

static void _fingerEventToMouseMotion(SDL_Event* event) {
	if(event->tfinger.fingerId == 0) {
		SDL_Event newEvent;
		newEvent.type = SDL_MOUSEMOTION;
		newEvent.motion.state = SDL_BUTTON_LEFT;
		newEvent.motion.windowID = 0;
		newEvent.motion.timestamp = event->tfinger.timestamp;
		newEvent.motion.which = SDL_TOUCH_MOUSEID;
		newEvent.motion.x = (Sint32)(event->tfinger.x * (float)(NX_DISPLAY_WIDTH));
		newEvent.motion.y = (Sint32)(event->tfinger.y * (float)(NX_DISPLAY_HEIGHT));
		newEvent.motion.xrel = (Sint32)(event->tfinger.dx * (float)(NX_DISPLAY_WIDTH));
		newEvent.motion.yrel = (Sint32)(event->tfinger.dy * (float)(NX_DISPLAY_HEIGHT));
		memcpy(event, &newEvent, sizeof(SDL_Event));
	}
}

int nxHooksSDLPollEvents(SDL_Event* event) {
	int result = SDL_PollEvent(event);

	if(result) {
		switch(event->type) {
			case SDL_FINGERDOWN:
				_fingerEventToMouseButton(event, true);
				break;
			case SDL_FINGERUP:
				_fingerEventToMouseButton(event, false);
				break;
			case SDL_FINGERMOTION:
				_fingerEventToMouseMotion(event);
				break;
		}
	}

	return result;
}