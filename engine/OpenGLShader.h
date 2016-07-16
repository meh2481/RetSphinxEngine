#pragma once
#include "SDL_opengl.h"

namespace OpenGLShader
{
	GLuint loadShaders(const char * vertex_file_path, const char * fragment_file_path);
}