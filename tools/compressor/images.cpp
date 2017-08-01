#include "main.h"
#include "stb_rect_pack.h"
#include "stb_image.h"
#include <vector>
#include <string>

#define DEFAULT_SZ 1024

typedef struct
{
	int comp;
	int width;
	int height;
	unsigned char* buf;
} Image;

static std::vector<std::string> images;
void addImage(const std::string& img)
{
	images.push_back(img);
}

void packImages()
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
			atlasW >>= 1;

		while(img.height > atlasH)
			atlasH >>= 1;
	}

	stbrp_context context;
	int numNodes = atlasW;

	stbrp_node *nodes = (stbrp_node*) malloc(sizeof(stbrp_node) * numNodes);
	stbrp_rect *rects = (stbrp_rect*) malloc(sizeof(stbrp_rect) * startImages.size());
	do
	{
		stbrp_init_target(&context, atlasW, atlasH, nodes, numNodes);
		
		//TODO

	} while(!stbrp_pack_rects(&context, rects, startImages.size()));

	//Done; pack into image or something

	//TODO

	//Reset so we can call again later
	images.clear();

	//Clean up memory
	for(std::vector<Image>::iterator i = startImages.begin(); i != startImages.end(); i++)
		stbi_image_free(i->buf);
	free(nodes);
}