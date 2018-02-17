#version 430 core

in vec3 position;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 col;
out vec4 incol;

void main()
{
	incol = col;
	gl_Position = projection * view * model * vec4(position, 1.0);
}
