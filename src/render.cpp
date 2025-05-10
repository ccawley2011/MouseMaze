#include "render.h"

#ifdef _WIN32
#include <windows.h>
#endif

void BaseSprite::render(const SDL_Rect* srcrect, int x, int y)
{
	SDL_Rect dstrect;
	dstrect.x = x;
	dstrect.y = y;
	dstrect.w = srcrect ? srcrect->w : _width;
	dstrect.h = srcrect ? srcrect->h : _height;
	render(srcrect, &dstrect);
}

void BaseRenderer::msgBox(const char* msg, ...) {
	va_list ap;

	va_start(ap, msg);
	msgBoxV(msg, ap);
	va_end(ap);
}

BaseSprite* BaseRenderer::loadSprite(const char* fname, Uint32 key) {
	BaseSprite *sprite;
	SDL_Surface* bmp = SDL_LoadBMP(fname);
	if (!bmp)
		return NULL;

	sprite = loadSprite(bmp, key);
	SDL_FreeSurface(bmp);
	return sprite;
}

#if SDL_VERSION_ATLEAST(2, 0, 0)

class Sprite : public BaseSprite {
public:
	Sprite(Renderer* renderer, SDL_Surface *surface, Uint32 key) : BaseSprite(), _renderer(renderer) {
		SDL_SetColorKey(surface, SDL_TRUE, key);
		_width = surface->w;
		_height = surface->h;
		_texture = SDL_CreateTextureFromSurface(_renderer->getRenderer(), surface);
	}

	~Sprite() {
		SDL_DestroyTexture(_texture);
	}

	void render(const SDL_Rect* srcrect, const SDL_Rect* dstrect) {
		SDL_RenderCopy(_renderer->getRenderer(), _texture, srcrect, dstrect);
	}

private:
	Renderer* _renderer;
	SDL_Texture* _texture;
};

Renderer::~Renderer() {
	SDL_DestroyRenderer(_renderer);
	_renderer = NULL;

	SDL_DestroyWindow(_window);
	_window = NULL;

	SDL_free(_title);
	_title = NULL;

	SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

bool Renderer::init(const char* title, int width, int height) {
	_width = width;
	_height = height;
	_title = SDL_strdup(title);

	if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0)
		return false;

	_window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_RESIZABLE);
	if (!_window)
		return false;

	_renderer = SDL_CreateRenderer(_window, -1, 0);
	if (!_renderer)
		return false;

	SDL_RenderSetLogicalSize(_renderer, width, height);

	return true;
}

void Renderer::clear() {
	SDL_RenderClear(_renderer);
}

void Renderer::present() {
	SDL_RenderPresent(_renderer);
}

BaseSprite *Renderer::loadSprite(SDL_Surface *surface, Uint32 key) {
	return new Sprite(this, surface, key);
}

SDL_Surface* Renderer::createSurface(int width, int height) {
	SDL_RendererInfo info;
	if (SDL_GetRendererInfo(_renderer, &info))
		return NULL;

	return SDL_CreateRGBSurfaceWithFormat(0, width, height, SDL_BITSPERPIXEL(info.texture_formats[0]), info.texture_formats[0]);
}

void Renderer::msgBoxV(const char* msg, va_list ap) {
	char fullMsg[1024];
	SDL_vsnprintf(fullMsg, 1024, msg, ap);

	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, _title, fullMsg, _window);
}
#else

#include "palette.h"

class Sprite : public BaseSprite {
public:
	Sprite(Renderer *renderer, SDL_Surface *surface, Uint32 key) : BaseSprite(), _renderer(renderer) {
		SDL_SetColorKey(surface, (SDL_SRCCOLORKEY | SDL_RLEACCEL), key);
		_width = surface->w;
		_height = surface->h;
		_surface = SDL_DisplayFormat(surface);
	}

	~Sprite() {
		SDL_FreeSurface(_surface);
	}

	void render(const SDL_Rect* srcrect, const SDL_Rect* dstrect) {
		SDL_BlitSurface(_surface, (SDL_Rect*)srcrect, _renderer->getScreen(), (SDL_Rect*)dstrect);
	}

private:
	Renderer* _renderer;
	SDL_Surface* _surface;
};

Renderer::~Renderer() {
	SDL_free(_title);
	_title = NULL;
}

bool Renderer::init(const char* title, int width, int height) {
	_width = width;
	_height = height;

	_screen = SDL_SetVideoMode(width, height, 8, SDL_HWSURFACE | SDL_HWPALETTE | SDL_DOUBLEBUF
#ifdef __NDS__
		| SDL_BOTTOMSCR
#endif
	);
	if (!_screen)
		return false;

	SDL_WM_SetCaption(title, NULL);
	SDL_SetPalette(_screen, SDL_LOGPAL | SDL_PHYSPAL, riscos_palette_256, 0, SDL_arraysize(riscos_palette_256));

	_title = SDL_strdup(title);
	_background = SDL_MapRGB(_screen->format, 0x00, 0x00, 0x00);
	return true;
}

void Renderer::clear() {
	SDL_FillRect(_screen, NULL, _background);
}

void Renderer::present() {
	SDL_Flip(_screen);
}

BaseSprite *Renderer::loadSprite(SDL_Surface *surface, Uint32 key) {
	return new Sprite(this, surface, key);
}

SDL_Surface* Renderer::createSurface(int width, int height) {
	return SDL_CreateRGBSurface(0, width, height, 32, 0xFF0000, 0xFF00, 0xFF, 0xFF000000);
}

void Renderer::msgBoxV(const char* msg, va_list ap) {
#ifdef _WIN32
	char fullMsg[1024];
	SDL_vsnprintf(fullMsg, 1024, msg, ap);

	MessageBoxA(NULL, fullMsg, _title, MB_ICONERROR);
#else
	fprintf(stderr, "ERROR: ");
	vfprintf(stderr, msg, ap);
	fprintf(stderr, "\n");
#endif
}
#endif
