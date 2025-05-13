#ifndef RENDER_H
#define RENDER_H

#include "compat.h"

class BaseSprite {
public:
	BaseSprite() : _width(0), _height(0) {}
	virtual ~BaseSprite() {}

	void render(int x, int y) { render(NULL, x, y); }
	void render(const SDL_Rect* dstrect) { render(NULL, dstrect); }
	void render(const SDL_Rect* srcrect, int x, int y);
	virtual void render(const SDL_Rect* srcrect, const SDL_Rect* dstrect) = 0;

	inline int getWidth() const { return _width; }
	inline int getHeight() const { return _height; }

protected:
	int _width, _height;
};

class BaseRenderer {
public:
	BaseRenderer() : _width(0), _height(0) {}
	virtual ~BaseRenderer() {}

	static BaseRenderer *create();

	virtual bool init(const char* title, int width, int height) = 0;
	virtual void clear() = 0;
	virtual void present() = 0;

	virtual BaseSprite *loadSprite(const char* fname, Uint32 key);
	virtual BaseSprite* loadSprite(SDL_Surface* surface, Uint32 key) = 0;

	virtual SDL_Surface* createSurface(int width, int height) = 0;

	virtual void msgBox(const char* msg, ...);
	virtual void msgBoxV(const char* msg, va_list ap) = 0;

	inline int getWidth() const { return _width; }
	inline int getHeight() const { return _height; }

protected:
	int _width, _height;
};

#endif
