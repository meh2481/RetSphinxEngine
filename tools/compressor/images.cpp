#include "main.h"
#define STBRP_STATIC
#define STB_RECT_PACK_IMPLEMENTATION
#include "stb_rect_pack.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "stb_image.h"
#include <vector>
#include <string>
#include <assert.h>
#include <iostream>
#include <sstream>

#define DEFAULT_SZ 2048
#define BYTES_PER_PIXEL 4

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
void copyImage(unsigned char* buf, ImageHelper* img, unsigned short xPos, unsigned short yPos, int atlasW)
{
	//TODO: Offset by 1 in each dir and stretch last pixels
	assert(img->comp == STBI_rgb_alpha || img->comp == STBI_rgb);
	unsigned char* src = img->buf;
	for(int y = 0; y < img->height; y++)
	{
		unsigned char* dst = &buf[(atlasW * BYTES_PER_PIXEL * (y + yPos)) + (xPos * BYTES_PER_PIXEL)];
		for(int x = 0; x < img->width; x++)
		{
			*dst++ = *src++;	//R
			*dst++ = *src++;	//G
			*dst++ = *src++;	//B
			if(img->comp == STBI_rgb_alpha)
				*dst++ = *src++;	//A
			else
				*dst++ = 255;	//Alpha of 255
		}
	}
}

void packImage(stbrp_rect *rects, int rectSz, std::vector<ImageHelper>* images, int curAtlas, int atlasW, int atlasH, const std::string& filename)
{
	//Create destination buffer for atlas
	size_t bufferSize = atlasW * atlasH * BYTES_PER_PIXEL + sizeof(TextureHeader);
	unsigned char* destBuf = (unsigned char*)malloc(bufferSize);
	memset(destBuf, 0, bufferSize);	//Clear dest buf
	
	//Create compression helper for atlas
	std::ostringstream oss;
	oss << filename << curAtlas << ".bin";
	CompressionHelper atlasHelper;
	atlasHelper.header.type = RESOURCE_TYPE_IMAGE_ATLAS;
	atlasHelper.id = Hash::hash(oss.str().c_str());
	
	//Create Textures that point to locations in this atlas
	for(int i = 0; i < rectSz; i++)
	{
		stbrp_rect r = rects[i];
		if(r.was_packed)
		{
			ImageHelper img = images->at(r.id);
			copyImage(destBuf + sizeof(TextureHeader), &img, r.x, r.y, atlasW);	//Copy image data over to dest buf, in the proper location

			//Store coords
			TextureRect* rc = (TextureRect*)malloc(sizeof(TextureRect));
			rc->atlasId = atlasHelper.id;
			
			float actualX = r.x;
			float actualY = r.y;	//Y is inverted

			//Get UV coordinates
			float top = actualY / (float)atlasH;
			float left = actualX / (float)atlasW;
			float right = (actualX + (float)img.width) / (float)atlasW;
			float bottom = (actualY + (float)img.height) / (float)atlasH;

			//CCW winding from lower left (Inverted Y for some reason), x/y format
			rc->coordinates[0] = left;
			rc->coordinates[1] = bottom;
			rc->coordinates[2] = right;
			rc->coordinates[3] = bottom;
			rc->coordinates[4] = right;
			rc->coordinates[5] = top;
			rc->coordinates[6] = left;
			rc->coordinates[7] = top;

			//Add texture to pak
			CompressionHelper textureHelper;
			textureHelper.header.type = RESOURCE_TYPE_IMAGE;
			textureHelper.id = img.hash;
			createCompressionHelper(&textureHelper, (unsigned char*)rc, sizeof(TextureRect));
			addHelper(textureHelper);
		}
	}

	//Create header for atlas
	TextureHeader* header = (TextureHeader*)destBuf;
	assert(atlasH == atlasW);
	header->height = atlasH;
	header->width = atlasW;
	header->bpp = BYTES_PER_PIXEL;
	header->format = TEXTURE_FORMAT_RAW;	//TODO Support different texture formats

	//Add atlas to .pak
	createCompressionHelper(&atlasHelper, destBuf, bufferSize);
	addHelper(atlasHelper);
}

void removeDone(stbrp_rect *rects, int* rectsz)
{
	for(int i = 0; i < *rectsz; i++)
	{
		if(rects[i].was_packed)
		{
			if(i != *rectsz - 1)	//If we're on last item, don't copy over ourself
				memcpy(&rects[i], &rects[*rectsz - 1], sizeof(stbrp_rect));	//Copy last rect to here
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
		while(img.width > atlasSz)
			atlasSz <<= 1;

		while(img.height > atlasSz)
			atlasSz <<= 1;
	}

	stbrp_context context;
	int numNodes = atlasSz;

	//TODO: Take texture bpp's into account when determining which atlas to pack them into

	stbrp_node *nodes = (stbrp_node*) malloc(sizeof(stbrp_node) * numNodes);
	stbrp_rect *rects = (stbrp_rect*) malloc(sizeof(stbrp_rect) * startImages.size());
	int rectsLeft = startImages.size();
	//Init rects
	for(int i = 0; i < rectsLeft; i++)
	{
		//TODO: +2 pixels on each side and stretch
		rects[i].id = i;
		rects[i].w = startImages[i].width;
		rects[i].h = startImages[i].height;
	}

	//Main packing loop
	int packed = 1;
	int curAtlas = 0;
	do
	{
		stbrp_init_target(&context, atlasSz, atlasSz, nodes, numNodes);
		
		packed = stbrp_pack_rects(&context, rects, rectsLeft);

		//TODO At some point, we'll want to check if only one image was packed and just store it by itself in a separate texture

		//Pack into image
		packImage(rects, rectsLeft, &startImages, curAtlas++, atlasSz, atlasSz, filename);

		//Pull out done rects and update rectsLeft. If list is empty, packed will be 1 and we'll break out anyway. Probably. It'll be fine.
		if(!packed)	//Only worry about removing done rects if we're not done yet
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