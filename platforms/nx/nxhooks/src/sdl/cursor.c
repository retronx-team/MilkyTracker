#include <nxhooks/sdl/cursor.h>

#define _CURSOR_SIZE 16

typedef struct {
	int hotspot_x;
	int hotspot_y;
	char map[_CURSOR_SIZE*_CURSOR_SIZE+1];
} _NXCursor;

static _NXCursor g_nxcursor_pointer = {
        0, 0,
        "000000          "
        "0222220         "
        "0111120         "
        "011120          "
        "0111120         "
        "01101120        "
        " 00 01120       "
        "     01120      "
        "      01120     "
        "       010      "
        "        0       "
        "                "
        "                "
        "                "
        "                "
        "                "
};

static _NXCursor g_nxcursor_resizehoriz = {
        7, 7,
        "                "
        "                "
        "                "
        "                "
        "   00      00   "
        "  0220    0220  "
        " 02120000002120 "
        "0211122222211120"
        "0111111111111110"
        " 01110000001110 "
        "  0110    0110  "
        "   00      00   "
        "                "
        "                "
        "                "
        "                "
};

static _NXCursor g_nxcursor_hand = {
        5, 1,
        "                "
        "     0          "
        "    020         "
        "    020         "
        "    0200        "
        "  0 020200      "
        " 02002020200    "
        " 022020202020   "
        "  02222222020   "
        "  02222222220   "
        "   022222220    "
        "    02222220    "
        "    0222220     "
        "    0222220     "
        "                "
        "                "
};

static _NXCursor g_nxcursor_busy = {
        7, 7,
        "    2222 2      "
        "  222222222     "
        " 22000022222    "
        " 222202222222   "
        "2222022222222   "
        " 2200002000022  "
        "  2222222202222 "
        " 2222222202222  "
        "  222222000022  "
        "    222222222   "
        "       2222     "
        "     222        "
        "    222222      "
        "     22 2  22   "
        "           222  "
        "            2   "
};

static SDL_Renderer* g_current_renderer = NULL;
static SDL_Texture* g_cursor_texture = NULL;
static const _NXCursor* g_cursor_current = NULL;
static int g_cursor_x = -1;
static int g_cursor_y = -1;

void nxHooksSDLCursorInit(SDL_Renderer* renderer) {
	g_current_renderer = renderer;
	nxHooksSDLCursorSet(NXHooksSDLCursor_Pointer);
}

void nxHooksSDLCursorSet(NXHooksSDLCursorType type) {
	switch (type) {
		case NXHooksSDLCursor_Pointer:
			g_cursor_current = &g_nxcursor_pointer;
			break;
		case NXHooksSDLCursor_ResizeHoriz:
			g_cursor_current = &g_nxcursor_resizehoriz;
			break;
		case NXHooksSDLCursor_Hand:
			g_cursor_current = &g_nxcursor_hand;
			break;
		case NXHooksSDLCursor_Busy:
			g_cursor_current = &g_nxcursor_busy;
			break;
	}

	if(g_cursor_texture != NULL) {
		SDL_DestroyTexture(g_cursor_texture);
	}

	g_cursor_texture = SDL_CreateTexture(g_current_renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, _CURSOR_SIZE, _CURSOR_SIZE);

	uint32_t pixels[_CURSOR_SIZE*_CURSOR_SIZE];

	for (int i = 0; i < (_CURSOR_SIZE*_CURSOR_SIZE); i++) {
		switch (g_cursor_current->map[i]) {
			case '0':
				pixels[i] = 0xFF000000;
				break;
			case '1':
				pixels[i] = 0xFFDF2020;
				break;
			case '2':
				pixels[i] = 0xFFCFAFFF;
				break;
			default:
				pixels[i] = 0;
				break;
		}
	}

	SDL_UpdateTexture(g_cursor_texture, NULL, pixels, _CURSOR_SIZE * sizeof(pixels[0]));
	SDL_SetTextureBlendMode(g_cursor_texture, SDL_BLENDMODE_BLEND);
}

void nxHooksSDLCursorRender() {
	if(g_cursor_current != NULL && g_cursor_texture != NULL && g_cursor_x >= 0 && g_cursor_y >= 0) {
		SDL_Rect dst;
		dst.x = g_cursor_x - g_cursor_current->hotspot_x;
		dst.y = g_cursor_y - g_cursor_current->hotspot_y;
		dst.w = _CURSOR_SIZE;
		dst.h = _CURSOR_SIZE;
		SDL_RenderCopy(g_current_renderer, g_cursor_texture, NULL, &dst);
	}
}

void nxHooksSDLCursorSetPos(int x, int y) {
	g_cursor_x = x;
	g_cursor_y = y;
}

#undef _CURSOR_SIZE