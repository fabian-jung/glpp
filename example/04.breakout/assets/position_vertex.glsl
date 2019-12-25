#version 330 core
layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 tex;

uniform vec2 position;

out vec2 v_tex;

void main()
{
	gl_Position.xy = pos+position;
	v_tex = tex;
}
