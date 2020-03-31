#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 color;
out vec2 c;

void main()
{
	c = color.xy;
	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
