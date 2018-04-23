#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(push_constant) uniform PushConstantBlock {
    mat4 mvp;	//proj * view * model
} pushConstant;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;

layout(location = 0) out vec4 fragColor;

void main() {
    gl_Position = pushConstant.mvp * vec4(inPosition, 1.0);
	gl_PointSize = 4.0;
    fragColor = inColor;
}