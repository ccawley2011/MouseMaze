#include "font.h"
#include "render.h"

/**
 * Unifont .hex loading code
 *
 * Based on code from testime
 */

static Uint8 dehex(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    else if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    else if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    return 255;
}

static Uint8 dehex2(char c1, char c2)
{
    return (dehex(c1) << 4) | dehex(c2);
}

static Uint8 validate_hex(const char* cp, size_t len, Uint32* np)
{
    Uint32 n = 0;
    for (; len > 0; cp++, len--)
    {
        Uint8 c = dehex(*cp);
        if (c == 255)
            return 0;
        n = (n << 4) | c;
    }
    if (np != NULL)
        *np = n;
    return 1;
}

Font::Font(int maxGlyphs) : _maxGlyphs(maxGlyphs) {
    _unifontGlyph = new UnifontGlyph[_maxGlyphs];
}

Font::~Font()
{
    delete[] _unifontGlyph;
}

bool Font::load(const char* fontname)
{
    SDL_RWops* hexFile;
    bool success;

    hexFile = SDL_RWFromFile(fontname, "rb");
    if (hexFile == NULL)
    {
        return false;
    }

    success = loadRW(hexFile);

    SDL_RWclose(hexFile);

    return success;
}

bool Font::loadRW(SDL_RWops* hexFile)
{
    Uint8 hexBuffer[65];
    Uint32 numGlyphs = 0;
    int lineNumber = 1;
    size_t bytesRead;

    SDL_memset(_unifontGlyph, 0, _maxGlyphs * sizeof(struct UnifontGlyph));

    /* Read all the glyph data into memory to make it accessible later when textures are created. */
    do {
        int i, codepointHexSize;
        size_t bytesOverread;
        Uint8 glyphWidth;
        Uint32 codepoint;

        bytesRead = SDL_RWread(hexFile, hexBuffer, 1, 9);
        if (numGlyphs > 0 && bytesRead == 0)
            break; /* EOF */
        if ((numGlyphs == 0 && bytesRead == 0) || (numGlyphs > 0 && bytesRead < 9))
        {
            SDL_SetError("Unexpected end of hex file");
            return false;
        }

        /* Looking for the colon that separates the codepoint and glyph data at position 2, 4, 6 and 8. */
        if (hexBuffer[2] == ':')
            codepointHexSize = 2;
        else if (hexBuffer[4] == ':')
            codepointHexSize = 4;
        else if (hexBuffer[6] == ':')
            codepointHexSize = 6;
        else if (hexBuffer[8] == ':')
            codepointHexSize = 8;
        else
        {
            SDL_SetError("Could not find codepoint and glyph data separator symbol in hex file on line %d", lineNumber);
            return false;
        }

        if (!validate_hex((const char*)hexBuffer, codepointHexSize, &codepoint))
        {
            SDL_SetError("Malformed hexadecimal number in hex file on line %d", lineNumber);
            return false;
        }

        /* If there was glyph data read in the last file read, move it to the front of the buffer. */
        bytesOverread = 8 - codepointHexSize;
        if (codepointHexSize < 8)
            SDL_memmove(hexBuffer, hexBuffer + codepointHexSize + 1, bytesOverread);
        bytesRead = SDL_RWread(hexFile, hexBuffer + bytesOverread, 1, 33 - bytesOverread);
        if (bytesRead < (33 - bytesOverread))
        {
            SDL_SetError("Unexpected end of hex file");
            return false;
        }
        if (hexBuffer[32] == '\n')
            glyphWidth = 8;
        else
        {
            glyphWidth = 16;
            bytesRead = SDL_RWread(hexFile, hexBuffer + 33, 1, 32);
            if (bytesRead < 32)
            {
                SDL_SetError("Unexpected end of hex file");
                return false;
            }
        }

        if (!validate_hex((const char*)hexBuffer, glyphWidth * 4, NULL))
        {
            SDL_SetError("Malformed hexadecimal glyph data in hex file on line %d", lineNumber);
            return false;
        }

        if (codepoint < _maxGlyphs)
        {
            if (_unifontGlyph[codepoint].width <= 0)
            {
                _unifontGlyph[codepoint].width = glyphWidth;
                /* Pack the hex data into a more compact form. */
                for (i = 0; i < glyphWidth * 2; i++)
                    _unifontGlyph[codepoint].data[i] = dehex2(hexBuffer[i * 2], hexBuffer[i * 2 + 1]);
                numGlyphs++;
            }
        }

        lineNumber++;
    } while (bytesRead > 0);

    return true;
}

template<typename T>
static int unifont_blit(UnifontGlyph* glyph, Uint8* dst, int dstPitch, T colour)
{
    int i, j;
    T* row = (T*)dst;

    for (i = 0; i < glyph->width * 2; i++)
    {
        Uint8 data = glyph->data[i];
        for (j = 0; j < 8; j++)
        {
            if (data & 0x80)
            {
                *row = colour;
            }
            data <<= 1;
            row ++;
        }

        if (glyph->width == 8 || (glyph->width == 16 && i % 2 == 1))
        {
            dst += dstPitch;
            row = (T*)dst;
        }
    }

    return glyph->width;
}

int Font::drawGlyph(Uint32 codepoint, Uint8* dst, int dstPitch, int bpp, Uint32 colour)
{
    switch (bpp) {
    case 1:
        return unifont_blit<Uint8>(&_unifontGlyph[codepoint], dst, dstPitch, colour);
    case 2:
        return unifont_blit<Uint16>(&_unifontGlyph[codepoint], dst, dstPitch, colour);
    case 4:
        return unifont_blit<Uint32>(&_unifontGlyph[codepoint], dst, dstPitch, colour);
    default:
        return 0;
    }
}

int Font::drawString(const char* text, SDL_Surface* dst, SDL_Rect* dstrect, Uint32 colour) {
    int i = 0;
    int len = 0;
    Uint32 codepoint;
    SDL_Rect tmpRect;

    if (dstrect) {
        tmpRect = *dstrect;
    } else {
        tmpRect.x = 0;
        tmpRect.y = 0;
        tmpRect.w = dst->w;
        tmpRect.h = dst->h;
    }

    SDL_LockSurface(dst);

    /* TODO: UTF-8 */
    while ((codepoint = text[i++]))
    {
        int advance = drawGlyph(codepoint, (Uint8 *)dst->pixels + (dst->pitch * tmpRect.y) + (tmpRect.x * dst->format->BytesPerPixel), dst->pitch, dst->format->BytesPerPixel, colour);
        if (!advance)
            break;

        tmpRect.x += advance;
        len += advance;
    }

    SDL_UnlockSurface(dst);

    return len;
}

int Font::drawString(const char* text, SDL_Surface* dst, SDL_Rect* dstrect, Uint8 r, Uint8 g, Uint8 b) {
    return drawString(text, dst, dstrect, SDL_MapRGB(dst->format, r, g, b));
}

int Font::measureString(const char* text) {
    int i = 0;
    int len = 0;
    Uint32 codepoint;

    /* TODO: UTF-8 */
    while ((codepoint = text[i++]))
    {
        len += _unifontGlyph[codepoint].width;
    }

    return len;
}

BaseSprite* Font::createSprite(BaseRenderer* renderer, const char* text, Uint8 r, Uint8 g, Uint8 b) {
    Uint32 colour, key;
    SDL_Surface* surface = renderer->createSurface(measureString(text), 16);
    if (!surface)
        return NULL;

    colour = SDL_MapRGB(surface->format, r, g, b);
    key = SDL_MapRGB(surface->format, ~r, ~g, ~b);

    SDL_FillRect(surface, NULL, key);
    drawString(text, surface, NULL, colour);

    BaseSprite* sprite = renderer->loadSprite(surface, key);
    SDL_FreeSurface(surface);
    return sprite;
}
