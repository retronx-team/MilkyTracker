#include <nxhooks/sdl/input.h>
#include <nxhooks/screen.h>
#include <switch.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#define NX_MAX_CONTROLLERS 8
#define NX_AXIS_DEADZONE 3000
#define NX_AXIS_FAST_SPEEDSTEP (INT16_MAX / NX_SCREEN_WIDTH * 50)
#define NX_AXIS_SLOW_SPEEDSTEP (NX_AXIS_FAST_SPEEDSTEP * 3)

static SDL_GameController* g_controllers[NX_MAX_CONTROLLERS];
static SDL_TimerID g_controllermouse_timer;
static int16_t g_axis_x = 0;
static int16_t g_axis_y = 0;
static bool g_axis_fastspeed = false;
static bool g_speedtoggle_held = false;
static bool g_clicktoggle_held = false;

SDL_Window* SDL_GetFocusWindow(void);
int SDL_SendMouseMotion(SDL_Window* window, int mouseID, int relative, int x, int y);
int SDL_SendMouseButton(SDL_Window* window, int mouseID, uint8_t state, uint8_t button);
int SDL_SendKeyboardKey(uint8_t state, SDL_Scancode scancode);

static void _splitJoycons() {
	for (int i = 0; i < NX_MAX_CONTROLLERS; i++) {
		hidSetNpadJoyAssignmentModeSingleByDefault((HidControllerID)i);
	}
	hidSetNpadJoyHoldType(HidJoyHoldType_Default);
}

uint32_t _timerSendControllerMouseMotionEvents(uint32_t interval, void *param) {
	SDL_Window* window = SDL_GetFocusWindow();

	if(window != NULL) {
		if(g_axis_x != 0 || g_axis_y != 0) {
			bool fastspeed = g_axis_fastspeed;
			if(g_speedtoggle_held) {
				fastspeed = !fastspeed;
			}

			int16_t speedstep = fastspeed ? NX_AXIS_FAST_SPEEDSTEP : NX_AXIS_SLOW_SPEEDSTEP;

			SDL_SendMouseMotion(window, SDL_TOUCH_MOUSEID, 1, g_axis_x / speedstep, g_axis_y / speedstep);
		}
	}

	return interval;
}

static void _mapControllerButton(SDL_GameControllerButton button, bool down) {
	switch(button) {
		case SDL_CONTROLLER_BUTTON_LEFTSTICK:
		case SDL_CONTROLLER_BUTTON_RIGHTSTICK:
			g_speedtoggle_held = down;
			break;
		case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
		case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
			g_clicktoggle_held = down;
			break;
		case SDL_CONTROLLER_BUTTON_BACK:
		case SDL_CONTROLLER_BUTTON_GUIDE:
		case SDL_CONTROLLER_BUTTON_START:
			if(!down) {
				g_axis_fastspeed = !g_axis_fastspeed;
			}
			break;
		case SDL_CONTROLLER_BUTTON_B: // switch A
		case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
			SDL_SendKeyboardKey(down, SDL_SCANCODE_RETURN);
			break;
		case SDL_CONTROLLER_BUTTON_A: // switch B
		case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
			SDL_SendKeyboardKey(down, SDL_SCANCODE_SPACE);
			break;
		case SDL_CONTROLLER_BUTTON_X: // switch Y
		case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
			SDL_SendKeyboardKey(down, SDL_SCANCODE_LCTRL);
			break;
		case SDL_CONTROLLER_BUTTON_Y: // switch X
		case SDL_CONTROLLER_BUTTON_DPAD_UP:
			SDL_SendKeyboardKey(down, SDL_SCANCODE_LSHIFT);
			break;
	}
}

static void _SendControllerMouseClick(bool down) {
	SDL_Window* window = SDL_GetFocusWindow();

	if(window != NULL) {
		if(down) {
			SDL_SendMouseButton(window, SDL_TOUCH_MOUSEID, SDL_PRESSED, g_clicktoggle_held ? SDL_BUTTON_RIGHT : SDL_BUTTON_LEFT);
		} else {
			SDL_SendMouseButton(window, SDL_TOUCH_MOUSEID, SDL_RELEASED, SDL_BUTTON_LEFT);
			SDL_SendMouseButton(window, SDL_TOUCH_MOUSEID, SDL_RELEASED, SDL_BUTTON_RIGHT);
		}
	}
}

static void _fingerUpdateMouseCoords(SDL_Event* event) {
	SDL_Window* window = SDL_GetFocusWindow();

	if(window != NULL) {
		SDL_WarpMouseInWindow(window, (Sint32)(event->tfinger.x * (float)(NX_SCREEN_WIDTH)), (Sint32)(event->tfinger.y * (float)(NX_SCREEN_HEIGHT)));
	}
}

int nxHooksSDLPollEvents(SDL_Event* event) {
	_splitJoycons();
	int result = SDL_PollEvent(event);

	if(result) {
		switch(event->type) {
			case SDL_CONTROLLERAXISMOTION:
				if(event->caxis.axis == SDL_CONTROLLER_AXIS_LEFTX || event->caxis.axis == SDL_CONTROLLER_AXIS_RIGHTX) {
					g_axis_x = abs(event->caxis.value) >= NX_AXIS_DEADZONE ? event->caxis.value : 0;
				} else if(event->caxis.axis == SDL_CONTROLLER_AXIS_LEFTY || event->caxis.axis == SDL_CONTROLLER_AXIS_RIGHTY) {
					g_axis_y = abs(event->caxis.value) >= NX_AXIS_DEADZONE ? event->caxis.value : 0;
				} else if(event->caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT || event->caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT) {
					_SendControllerMouseClick(abs(event->caxis.value) >= NX_AXIS_DEADZONE);
				}
				result = 0;
				break;
			case SDL_CONTROLLERBUTTONDOWN:
			case SDL_CONTROLLERBUTTONUP:
				_mapControllerButton(event->cbutton.button, event->cbutton.state);
				result = 0;
				break;
			case SDL_FINGERDOWN:
				_fingerUpdateMouseCoords(event);
				_SendControllerMouseClick(true);
				result = 0;
				break;
			case SDL_FINGERUP:
				_fingerUpdateMouseCoords(event);
				_SendControllerMouseClick(false);
				result = 0;
				break;
			case SDL_FINGERMOTION:
				_fingerUpdateMouseCoords(event);
				result = 0;
				break;
		}
	}

	return result;
}

void nxHooksSDLInputInit() {
	_splitJoycons();

	if (SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER | SDL_INIT_TIMER) < 0) {
		fprintf(stderr, "[nxhooks] Couldn't initialize required SDL subsystems: %s\n", SDL_GetError());
	}

	int numJoysticks = SDL_NumJoysticks();
	for (int i = 0; i < NX_MAX_CONTROLLERS; i++) {
		if(i < numJoysticks && SDL_IsGameController(i)) {
			g_controllers[i] = SDL_GameControllerOpen(i);
		} else {
			g_controllers[i] = NULL;
		}
	}

	g_controllermouse_timer = SDL_AddTimer(16, _timerSendControllerMouseMotionEvents, NULL);
}

void nxHooksSDLInputExit() {
	SDL_RemoveTimer(g_controllermouse_timer);

	SDL_QuitSubSystem(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER | SDL_INIT_TIMER);
}