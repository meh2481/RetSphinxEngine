#include "RenderState.h"
#include "opengl-api.h"

void RenderState::apply()
{
	glm::mat4 mvp = projection * view * model;
	glUniformMatrix4fv(uniformId, 1, false, &mvp[0][0]);
}
