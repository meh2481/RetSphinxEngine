#include "lattice.h"

void lattice::reset()
{
	latticeVert* ptr = vertex;
	latticeVert* ptruv = UV;
	GLfloat segsizex = 1.0f/(GLfloat)(numx);
	GLfloat segsizey = 1.0f/(GLfloat)(numy);
	for(uint32 iy = 0; iy <= numy; iy++)
	{
		for(uint32 ix = 0; ix <= numx; ix++)
		{
			(*ptruv).x = (GLfloat)(ix) * segsizex;
			(*ptruv).y = (GLfloat)(iy) * segsizey;
			(*ptr).x = (GLfloat)(ix) * segsizex;
			(*ptr).y = (GLfloat)(iy) * segsizey;
			(*ptr).x -= 0.5;
			(*ptr).y -= 0.5;
			ptr++;
			ptruv++;
		}
	}
	bind();
}

void lattice::bind()
{
	GLfloat* ptr = m_vertex;
	GLfloat* ptruv = m_UV;
	
	latticeVert* vertptr = vertex;
	latticeVert* uvptr = UV;
	for(uint32 iy = 0; iy < numy; iy++)
	{
		for(uint32 ix = 0; ix < numx; ix++)
		{
			//Tex coords
			latticeVert* uvul = &UV[ix+iy*(numx+1)];
			latticeVert* uvur = &UV[(ix+1)+iy*(numx+1)];
			latticeVert* uvbr = &UV[(ix+1)+(iy+1)*(numx+1)];
			latticeVert* uvbl = &UV[(ix)+(iy+1)*(numx+1)];
			
			//Upper left
			*ptruv++ = uvul->x;
			*ptruv++ = uvul->y;
			
			//Upper right
			*ptruv++ = uvur->x;
			*ptruv++ = uvur->y;
			
			//Lower right
			*ptruv++ = uvbr->x;
			*ptruv++ = uvbr->y;
			
			//Lower left
			*ptruv++ = uvbl->x;
			*ptruv++ = uvbl->y;
			
			uvptr++;
			
			
			//Vertex coords
			latticeVert* vertul = &vertex[ix+iy*(numx+1)];
			latticeVert* vertur = &vertex[(ix+1)+iy*(numx+1)];
			latticeVert* vertbr = &vertex[(ix+1)+(iy+1)*(numx+1)];
			latticeVert* vertbl = &vertex[(ix)+(iy+1)*(numx+1)];
			
			//cout << ix << ',' << iy << endl;
			
			//Upper left
			*ptr++ = vertul->x;
			*ptr++ = vertul->y;
			
			//Upper right
			*ptr++ = vertur->x;
			*ptr++ = vertur->y;
			
			//Lower right
			*ptr++ = vertbr->x;
			*ptr++ = vertbr->y;
			
			
			//Lower left
			*ptr++ = vertbl->x;
			*ptr++ = vertbl->y;
			
			vertptr++;
		}
	}
}

void lattice::setup(uint32 x, uint32 y)
{
	m_vertex = new GLfloat[NUMLATTICEPOINTS(x*y)];
	m_UV = new GLfloat[NUMLATTICEPOINTS(x*y)];
	numx = x;
	numy = y;
	
	vertex = new latticeVert[(x+1)*(y+1)];
	UV = new latticeVert[(x+1)*(y+1)];
	
	reset();
}

lattice::~lattice()
{
	delete [] m_vertex;
	delete [] m_UV;
	delete [] vertex;
	delete [] UV;
}

//Ultra-fast render pass go!
void lattice::renderTex(GLuint tex)
{
	glBindTexture(GL_TEXTURE_2D, tex);
	glVertexPointer(2, GL_FLOAT, 0, m_vertex);
    glTexCoordPointer(2, GL_FLOAT, 0, m_UV);
    glDrawArrays(GL_QUADS, 0, numx * numy * 4);
}