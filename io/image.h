#ifndef IO_IMAGE_H
#define IO_IMAGE_H

class Image
{
public:
    union Pixel
    {
        struct { unsigned char r, g, b, a; };
        unsigned char raw[4];
    };

    static Image *New(unsigned w, unsigned h);
    static Image *LoadPNG(const void *buf, size_t sz);
    static bool WritePNG(const char *fn, const void *data, unsigned w, unsigned h);
    ~Image();
    void operator delete(void *p);
    Pixel *data();
    const Pixel *data() const;
    inline unsigned w() const { return _w; }
    inline unsigned h() const { return _h; }
    inline const Pixel& operator()(unsigned x, unsigned y) const { return data()[y*_w + x]; }
    inline       Pixel& operator()(unsigned x, unsigned y)       { return data()[y*_w + x]; }

    bool writePNG(const char *fn) const;

private:
    Image(unsigned w, unsigned h) throw();
    static Image *Alloc(unsigned w, unsigned h, size_t extrabytes);

protected:
    unsigned _w, _h;
    bool _externalbuf;
    // Image data stored here, or pointer to image data if _externalbuf is true
};

#endif
