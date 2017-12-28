#include "main.h"
#define STBRP_STATIC
#define STB_RECT_PACK_IMPLEMENTATION
#include "stb_rect_pack.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "stb_image.h"
#include "squish.h"
#include <vector>
#include <string>
#include <assert.h>
#include <iostream>
#include <sstream>
#include <SDL_opengl.h>
#include <SDL_opengl_glext.h>

#define DEFAULT_SZ 9    //512*512
#define BYTES_PER_PIXEL_RGBA 4
#define BYTES_PER_PIXEL_RGB  3

extern bool g_bImageOut;

typedef struct
{
    int comp;
    int width;
    int height;
    unsigned char* buf;
    uint64_t hash;
} ImageHelper;

//-----------------------------------------------------------------------------------------------
// Helper functions
//-----------------------------------------------------------------------------------------------
void copyImage(unsigned char* buf, ImageHelper* img, unsigned short xPos, unsigned short yPos, int atlasW, int bytesPerPixel)
{
    //Offset by 1 in each dir and stretch first/last pixels
    assert(img->comp == STBI_rgb_alpha || img->comp == STBI_rgb);
    unsigned char* src = img->buf;
    for(int y = 0; y < img->height + 2; y++)
    {
        unsigned char* dst = &buf[(atlasW * bytesPerPixel * (y + yPos)) + (xPos * bytesPerPixel)];
        for(int x = 0; x < img->width + 2; x++)
        {
            *dst++ = *src++;    //R
            *dst++ = *src++;    //G
            *dst++ = *src++;    //B
            if(img->comp == STBI_rgb_alpha && bytesPerPixel == BYTES_PER_PIXEL_RGBA)
                *dst++ = *src++;    //A
            else if(bytesPerPixel == BYTES_PER_PIXEL_RGBA)
                *dst++ = 255;    //Alpha of 255

            //Back up on first and last pixels of row
            if(x == 0 || x == img->width)
                src -= img->comp;    //Slight hack assumes STB comp = bytes per pixel
        }
        //Back up on first and last rows
        if(y == 0 || y == img->height)
            src -= img->width * img->comp;
    }
}

void createTexturesForAtlas(unsigned char* uncompressedBuf, stbrp_rect *rects, int rectSz, std::vector<ImageHelper>* images, int atlasSzPixels, uint64_t atlasId, int bytesPerPixel)
{
    for(int i = 0; i < rectSz; i++)
    {
        stbrp_rect r = rects[i];
        if(r.was_packed)
        {
            ImageHelper img = images->at(r.id);
            copyImage(uncompressedBuf, &img, r.x, r.y, atlasSzPixels, bytesPerPixel);    //Copy image data over to dest buf, in the proper location

            //Store coords
            TextureHeader* rc = (TextureHeader*)malloc(sizeof(TextureHeader));
            rc->atlasId = atlasId;

            float actualX = r.x + 1.0f;    // +1 offset here for UVs
            float actualY = r.y + 1.0f;

            //Get UV coordinates
            float top = actualY / (float)atlasSzPixels;
            float left = actualX / (float)atlasSzPixels;
            float right = (actualX + (float)img.width) / (float)atlasSzPixels;
            float bottom = (actualY + (float)img.height) / (float)atlasSzPixels;

            //CCW winding from lower left, x/y format
            rc->coordinates[0] = left;
            rc->coordinates[1] = bottom;
            rc->coordinates[2] = right;
            rc->coordinates[3] = bottom;
            rc->coordinates[4] = left;
            rc->coordinates[5] = top;
            rc->coordinates[6] = right;
            rc->coordinates[7] = top;

            //Add texture to pak
            CompressionHelper textureHelper;
            textureHelper.header.type = RESOURCE_TYPE_IMAGE;
            textureHelper.id = img.hash;
            createCompressionHelper(&textureHelper, (unsigned char*)rc, sizeof(TextureHeader));
            addHelper(textureHelper);
        }
    }
}

int getBytesPerPixel(stbrp_rect *rects, int rectSz, std::vector<ImageHelper>* images)
{
    //TODO: Probably want to pack only RGB->RGB and RGBA->RGBA unless there's some kind of space saving we can do?
    for(int i = 0; i < rectSz; i++)
    {
        stbrp_rect r = rects[i];
        if(r.was_packed)
        {
            ImageHelper img = images->at(r.id);
            if(img.comp == STBI_rgb_alpha)
                return BYTES_PER_PIXEL_RGBA;
        }
    }
    return BYTES_PER_PIXEL_RGB;
}

void packImage(stbrp_rect *rects, int rectSz, std::vector<ImageHelper>* images, int curAtlas, int atlasSz, const std::string& filename)
{
    //First pass: See if RGB or RGBA
    int bytesPerPixel = getBytesPerPixel(rects, rectSz, images);

    int atlasSzPixels = 1 << atlasSz;

    //Create destination buffer for atlas
    //TODO: Using BYTES_PER_PIXEL_RGBA past here, as squish requires rgba even if dxt1. Should go back to using bytesPerPixel when re-adding raw compression
    size_t bufferSize = atlasSzPixels * atlasSzPixels * BYTES_PER_PIXEL_RGBA;
    unsigned char* uncompressedBuf = (unsigned char*)malloc(bufferSize);
    memset(uncompressedBuf, 0, bufferSize);    //Clear dest buf

    //Create compression helper for atlas
    std::ostringstream oss;
    oss << filename << curAtlas << ".atlas";    //Assign an .atlas extension for this atlas, just so it's a different resource ID from anything else
    CompressionHelper atlasHelper;
    atlasHelper.header.type = RESOURCE_TYPE_IMAGE_ATLAS;
    atlasHelper.id = Hash::hash(oss.str().c_str());

    //Create Textures that point to locations in this atlas
    createTexturesForAtlas(uncompressedBuf, rects, rectSz, images, atlasSzPixels, atlasHelper.id, BYTES_PER_PIXEL_RGBA);

    //Compress with libsquish
    int flags = squish::kDxt5;
    if(bytesPerPixel == BYTES_PER_PIXEL_RGB)
        flags = squish::kDxt1;
    int compressedSize = squish::GetStorageRequirements(atlasSzPixels, atlasSzPixels, flags) + sizeof(AtlasHeader);
    unsigned char* compressedBuf = (unsigned char*)malloc(compressedSize);
    std::cout << "squishing atlas " << curAtlas << " (" << atlasSzPixels << ") with flags " << flags << std::endl;
    squish::CompressImage(uncompressedBuf, atlasSzPixels, atlasSzPixels, compressedBuf + sizeof(AtlasHeader), flags);

    //Output PNG if in testing mode
    if(g_bImageOut)
    {
        std::cout << "unsquishing atlas " << curAtlas << " (" << atlasSzPixels << ") with flags " << flags << std::endl;
        memset(uncompressedBuf, 0, bufferSize);
        squish::DecompressImage(uncompressedBuf, atlasSzPixels, atlasSzPixels, compressedBuf + sizeof(AtlasHeader), flags);
        std::ostringstream oss2;
        oss2 << filename << " - atlas " << curAtlas << ".png";
        std::cout << "Save " << oss2.str() << std::endl;
        if(!stbi_write_png(oss2.str().c_str(), atlasSzPixels, atlasSzPixels, BYTES_PER_PIXEL_RGBA, uncompressedBuf, atlasSzPixels * BYTES_PER_PIXEL_RGBA))
            std::cout << "stbi_write_png error while saving " << oss2.str() << ' ' << curAtlas << std::endl;
    }

    free(uncompressedBuf);

    //Create header for atlas
    AtlasHeader* header = (AtlasHeader*)compressedBuf;
    header->height = atlasSz;
    header->width = atlasSz;
    if(bytesPerPixel == BYTES_PER_PIXEL_RGB)
        header->mode = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
    else
        header->mode = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;

    //Add atlas to .pak
    createCompressionHelper(&atlasHelper, compressedBuf, compressedSize);
    addHelper(atlasHelper);
}

void removeDone(stbrp_rect *rects, int* rectsz)
{
    for(int i = 0; i < *rectsz; i++)
    {
        if(rects[i].was_packed)
        {
            if(i != *rectsz - 1)    //If we're on last item, don't copy over ourself
                memcpy(&rects[i], &rects[*rectsz - 1], sizeof(stbrp_rect));    //Copy last rect to here
            i--;
            (*rectsz)--;
        }
    }
}

//-----------------------------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------------------------

static std::vector<std::string> images;
void addImage(const std::string& img)
{
    images.push_back(img);
}

void packImages(const std::string& filename)
{
    int atlasSz = DEFAULT_SZ;

    std::vector<ImageHelper> startImages;
    for(std::vector<std::string>::iterator i = images.begin(); i != images.end(); i++)
    {
        ImageHelper img;

        img.comp = 0;
        img.width = 0;
        img.height = 0;
        img.buf = stbi_load(i->c_str(), &img.width, &img.height, &img.comp, 0);
        img.hash = Hash::hash(i->c_str());

        startImages.push_back(img);

        //Scale up atlas size as necessary
        while(img.width + 2 > 1 << atlasSz)
            atlasSz++;

        while(img.height + 2 > 1 << atlasSz)
            atlasSz++;
    }

    stbrp_context context;
    int atlasSzPixels = 1 << atlasSz;

    //TODO: Take texture bpp's into account when determining which atlas to pack them into

    stbrp_node *nodes = (stbrp_node*)malloc(sizeof(stbrp_node) * atlasSzPixels);
    stbrp_rect *rects = (stbrp_rect*)malloc(sizeof(stbrp_rect) * startImages.size());
    int rectsLeft = startImages.size();
    //Init rects
    for(int i = 0; i < rectsLeft; i++)
    {
        // +2 pixels on each side so we can stretch image and avoid UV rounding errors
        rects[i].id = i;
        rects[i].w = startImages[i].width + 2;
        rects[i].h = startImages[i].height + 2;
    }

    //Main packing loop
    int packed = 1;
    int curAtlas = 0;
    do
    {
        stbrp_init_target(&context, atlasSzPixels, atlasSzPixels, nodes, atlasSzPixels);

        packed = stbrp_pack_rects(&context, rects, rectsLeft);

        //TODO At some point, we'll want to check if only one image was packed and just store it by itself in a separate texture

        //Pack into image
        packImage(rects, rectsLeft, &startImages, curAtlas++, atlasSz, filename);

        //Pull out done rects and update rectsLeft. If list is empty, packed will be 1 and we'll break out anyway. Probably. It'll be fine.
        if(!packed)    //Only worry about removing done rects if we're not done yet
            removeDone(rects, &rectsLeft);

    } while(!packed);

    //Reset so we can call this again later for another .pak file
    images.clear();

    //Clean up memory
    for(std::vector<ImageHelper>::iterator i = startImages.begin(); i != startImages.end(); i++)
        stbi_image_free(i->buf);
    free(nodes);
    free(rects);
}
