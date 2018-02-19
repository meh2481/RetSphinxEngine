#pragma once
#include "glmx.h"

class RenderState
{
public:
    unsigned int programId;
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
    unsigned int modelId;
    unsigned int viewId;
    unsigned int projectionId;
};
