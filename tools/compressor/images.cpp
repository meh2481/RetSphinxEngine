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
#define BPP 4

typedef struct
{
	int comp;
	int width;
	int height;
	unsigned char* buf;
} Image;

//-----------------------------------------------------------------------------------------------
// Helper functions
//-----------------------------------------------------------------------------------------------
void copyImage(unsigned char* buf, Image* img, unsigned short xPos, unsigned short yPos, int atlasW)
{
	assert(img->comp == STBI_rgb_alpha || img->comp == STBI_rgb);
	unsigned char* src = img->buf;
	for(int y = 0; y < img->height; y++)
	{
		unsigned char* dst = &buf[(atlasW * BPP * (y + yPos)) + (xPos * BPP)];
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

void packImage(stbrp_rect *rects, int rectSz, std::vector<Image>* images, int curAtlas, int atlasW, int atlasH, const std::string& filename)
{
	unsigned char* destBuf = (unsigned char*)malloc(atlasW * atlasH * BPP);
	for(int i = 0; i < rectSz; i++)
	{
		stbrp_rect r = rects[i];
		if(r.was_packed)
		{
			Image img = images->at(r.id);
			copyImage(destBuf, &img, r.x, r.y, atlasW);	//Copy image data over to dest buf
		}
	}

	//Save img
	std::ostringstream oss;
	oss << filename << curAtlas << ".png";
	if(!stbi_write_png(oss.str().c_str(), atlasW, atlasH, BPP, destBuf, atlasW * BPP))
		std::cout << "stbi_write_png error while saving " << filename << ' ' << curAtlas << std::endl;

	free(destBuf);
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
	int atlasW = DEFAULT_SZ;
	int atlasH = DEFAULT_SZ;

	std::vector<Image> startImages;
	for(std::vector<std::string>::iterator i = images.begin(); i != images.end(); i++)
	{
		Image img;

		img.comp = 0;
		img.width = 0;
		img.height = 0;
		img.buf = stbi_load(i->c_str(), &img.width, &img.height, &img.comp, 0);

		startImages.push_back(img);

		//Scale up atlas size as necessary
		while(img.width > atlasW)
			atlasW <<= 1;

		while(img.height > atlasH)
			atlasH <<= 1;
	}

	stbrp_context context;
	int numNodes = atlasW;

	stbrp_node *nodes = (stbrp_node*) malloc(sizeof(stbrp_node) * numNodes);
	stbrp_rect *rects = (stbrp_rect*) malloc(sizeof(stbrp_rect) * startImages.size());
	int rectsLeft = startImages.size();
	//Init rects
	for(int i = 0; i < rectsLeft; i++)
	{
		rects[i].id = i;
		rects[i].w = startImages[i].width;
		rects[i].h = startImages[i].height;
	}

	//TODO: Sort rects by area, so largest ones first

	//Main packing loop
	int packed = 1;
	int curAtlas = 0;
	do
	{
		stbrp_init_target(&context, atlasW, atlasH, nodes, numNodes);
		
		packed = stbrp_pack_rects(&context, rects, rectsLeft);

		//TODO At some point, we'll want to check if only one image was packed and just store it by itself in a separate texture

		//Pack into image
		packImage(rects, rectsLeft, &startImages, curAtlas++, atlasW, atlasH, filename);

		//Pull out done rects and update rectsLeft. If list is empty, packed will be 1 and we'll break out anyway. Probably. It'll be fine.
		if(!packed)	//Only worry about removing done rects if we're not done yet
			removeDone(rects, &rectsLeft);

	} while(!packed);

	//Reset so we can call this again later for another .pak file
	images.clear();

	//Clean up memory
	for(std::vector<Image>::iterator i = startImages.begin(); i != startImages.end(); i++)
		stbi_image_free(i->buf);
	free(nodes);
	free(rects);
}