#ifndef FONT_H
#define FONT_H

#include <SDL.h>

class BaseRenderer;
class BaseSprite;

struct UnifontGlyph {
    Uint8 width;
    Uint8 data[32];
};

class Font {
public:
    Font(int maxGlyphs);
    ~Font();

    bool load(const char *name);
    bool loadRW(SDL_RWops* hexFile);

    int drawString(const char* text, SDL_Surface* dst, SDL_Rect* dstrect, Uint32 colour);
    int drawString(const char* text, SDL_Surface* dst, SDL_Rect* dstrect, Uint8 r, Uint8 g, Uint8 b);

    int measureString(const char* text);

    BaseSprite *createSprite(BaseRenderer* renderer, const char *text, Uint8 r, Uint8 g, Uint8 b);

private:
    int drawGlyph(Uint32 codepoint, Uint8* dst, int dstPitch, int bpp, Uint32 colour);

    unsigned int _maxGlyphs;
    UnifontGlyph *_unifontGlyph;
};

#endif
