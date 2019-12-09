#version 330 core
layout (location = 0) in vec2 aPos;
uniform vec2 offset;

void main()
{
	gl_Position.xy = aPos + offset;
}
