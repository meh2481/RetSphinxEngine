#version 430

in vec3 position;
in vec2 texcoord;
in vec3 normal;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
out vec2 uv;
out vec4 incol;

void main()
{
	incol = vec4(1.0, 1.0, 1.0, 1.0);
	uv = texcoord;
	gl_Position = projection * view * model * vec4(position, 1.0);
}
