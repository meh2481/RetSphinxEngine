#version 430

in vec3 position;
in vec4 color;
in vec2 texcoord;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
out vec2 uv;
out vec4 incol;

void main()
{
	incol = color;
	uv = texcoord;
	gl_Position = projection * view * model * vec4(position, 1.0);
}
