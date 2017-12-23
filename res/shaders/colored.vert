#version 330 core

layout(location = 0) in vec3 position;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
out vec4 incol;

void main()
{
	incol = gl_Color;
	gl_Position = projection * view * model * vec4(position, 1.0);
}
