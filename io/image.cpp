#define _CRT_SECURE_NO_DEPRECATE 1
#define _CRT_SECURE_NO_DEPRECATE 1

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_ONLY_PNG
#define STBI_ONLY_BMP
#define STBI_ONLY_TGA
#define STBI_NO_STDIO
#include "stb_image.h"
#include "stb_image_write.h"

#include <stdlib.h>
#include <new>
#include "image.h"

Image::Image(unsigned w, unsigned h)
: _w(w), _h(h), _externalbuf(false)
{
}

Image::~Image()
{
    if(_externalbuf)
        stbi_image_free(data());
}

Image *Image::New(unsigned w, unsigned h)
{
    return Alloc(w, h, size_t(w) * size_t(h));
}

Image *Image::Alloc(unsigned w, unsigned h, size_t extrabytes)
{
    void *p = malloc(sizeof(Image) + extrabytes); // FIXME: use res alloc
    return new (p) Image(w, h);
}

void Image::operator delete(void *p)
{
    free(p);
}

Image::Pixel *Image::data()
{
    Pixel *ptr = reinterpret_cast<Pixel*>(this+1);
    return _externalbuf ? ptr : *((Pixel**)ptr);
}

const Image::Pixel *Image::data() const
{
    const Pixel *ptr = reinterpret_cast<const Pixel*>(this+1);
    return _externalbuf ? ptr : *((const Pixel**)ptr);
}

Image *Image::LoadPNG(const void *buf, size_t sz)
{
    int w, h, comp;
    stbi_uc *raw = stbi_load_from_memory(static_cast<const stbi_uc*>(buf), (int)sz, &w, &h, &comp, STBI_rgb_alpha);
    if(!raw)
        return NULL;
    Image *img = Alloc(w, h, sizeof(void*));
    img->_externalbuf = true;
    *((void**)(img+1)) = raw;
    return img;
}

bool Image::writePNG(const char *fn) const
{
    return WritePNG(fn, data(), w(), h());
}

bool Image::WritePNG(const char *fn, const void *data, unsigned w, unsigned h)
{
    return !!stbi_write_png(fn, w, h, STBI_rgb_alpha, data, 0);
}
