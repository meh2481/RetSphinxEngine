#version 430
uniform sampler2D tex;

in vec2 uv;
in vec4 incol;

out vec4 outcol;

void main()
{
	outcol = incol * texture(tex, uv);
}

