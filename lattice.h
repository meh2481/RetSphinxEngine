#include "globaldefs.h"

#define NUMLATTICEPOINTS(x)		x*12

struct latticeVert
{
	GLfloat x, y;
};

class lattice
{
	void setup(uint32 x, uint32 y);
	
	GLfloat* m_vertex;
	GLfloat* m_UV;
	
public:
	latticeVert* vertex;
	latticeVert* UV;
	
	uint32 numx, numy;
	
	lattice(uint32 x, uint32 y) {setup(x,y);};
	lattice() {setup(10,10);};
	~lattice();
	
	void renderTex(GLuint tex);
	void bind();
	void reset();
	
	//Point getVertex(uint32 x, uint32 y);
	//void setVertex(uint32 x, uint32 y);
};