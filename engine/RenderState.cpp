#include "RenderState.h"
#include "opengl-api.h"

void RenderState::apply() const
{
    glUniformMatrix4fv(modelId, 1, false, &model[0][0]);
    glUniformMatrix4fv(viewId, 1, false, &view[0][0]);
    glUniformMatrix4fv(projectionId, 1, false, &projection[0][0]);
}
