#version 330 core

layout(location = 0) in vec3 position;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
out vec2 uv;
out vec4 incol;

void main()
{
	incol = gl_Color;
	uv = gl_MultiTexCoord0.xy;
	gl_Position = projection * view * model * vec4(position, 1.0);
}
